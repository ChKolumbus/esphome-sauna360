#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "sauna360.h"

#define MAX_QUEUE_SIZE 10

namespace esphome {
namespace sauna360 {

static const char *TAG = "sauna360";

void SAUNA360Component::setup() {
#ifdef USE_TIME
  if (this->time_id_ != nullptr) {
    this->time_id_->add_on_time_sync_callback([this] { this->send_time(); });
  }
#endif
}

void SAUNA360Component::loop() {
  const uint32_t now = millis();

  while (this->available()) {
    this->last_rx_ = now;
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }

  // Send packets during bus silence, best value sound far is 40
  if (this->rx_message_.empty() && (now - this->last_rx_ > 100) && (now - this->last_tx_ > 500)) {
#ifdef USE_TIME
    // Only build time packet when bus is silent and queue is empty to make sure we can send it right away
    if (this->send_time_requested_ && this->tx_queue_.empty() && this->do_send_time_())
      this->send_time_requested_ = false;
#endif
    // Send the next packet in the queue
    if (!this->tx_queue_.empty()) {

      auto packet = std::move(this->tx_queue_.front());
      this->tx_queue_.pop();
      ESP_LOGCONFIG(TAG, "TX QUEUE: %s" ,format_hex_pretty(packet).c_str()); // for debug2
      this->write_array(packet);
      this->flush();

      this->last_tx_ = now;
    }
  }
}

void SAUNA360Component::handle_char_(uint8_t c) {
  if (c == 0x9C) {
    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());
    size_t len = frame.size(); // for debug
    if (len >= 6){ESP_LOGCONFIG(TAG, "FRAME: %s" ,format_hex_pretty(frame).c_str());} // for debug
    this->rx_message_.clear();
    this->handle_frame_(frame);
    return;
  }
  this->rx_message_.push_back(c);
}

void SAUNA360Component::handle_frame_(std::vector<uint8_t> frame) {
  // Decode the frame
  std::vector<uint8_t> packet;
  bool isEscaped = false;
  for (int i = 1; i < frame.size()-1; i++) {
    uint8_t d = frame[i];
    if (d==0x91){
      isEscaped = true;
      continue;
    }
    else if (isEscaped){
      isEscaped = false;
      if (d == 0x63){
        // The EOF byte
        d = 0x9c;
      }
      else if (d == 0x67){
        // The SOF byte
        d = 0x98;
      }
      else if (d == 0x6E){
        // The ESC byte itself
        d = 0x91;
      }
      else{
        ESP_LOGCONFIG(TAG, "Unknown escape sequence: %02x" ,d);
      packet.push_back(0x91);
      }
    }
    packet.push_back(d);
  }

   if (crc16be(packet.data(), 16, 0xffff, 0x90d9, false, false)){
     packet.pop_back();
     this->handle_packet_(packet);
   }
   else {
    ESP_LOGCONFIG(TAG, "CRC ERROR");
   }
   frame.clear();
}

void SAUNA360Component::handle_packet_(std::vector<uint8_t> packet) {
  // Decode the packet, but skip keepalive handshakes
  size_t len = packet.size();
  if (len <= 2){
    packet.clear();
    return;
  }
  uint8_t address = packet[0];
  uint8_t packetType = packet[1];

  uint16_t code = ((uint16_t) packet[2]) << 8; // add MSB 
  code |= ((uint16_t) packet[3]); // add LSB

  uint32_t data = ((uint32_t) packet[4]) << 24; // add MSB
  data |= ((uint32_t) packet[5]) << 16; // next byte
  data |= ((uint32_t) packet[6]) << 8; // next byte
  data |= ((uint32_t) packet[7]); // LSB"

   // Only take codes from the heater to control
  if ((packetType == 0x07) || (packetType == 0x09)) {
    packet.clear();
    return;
  }

  if (code == 0x6000){
    // temperature data point. Split into set point and actual value
    int actualTemp = (data & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature(actualTemp);}
    int setPointTemp = ((data >> 11) & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature_setting(setPointTemp);}
  }

  else if (code == 0x3400){
    //status code
  }

  else if (code == 0x9400){
    //total operation time minute upcounter.
    ESP_LOGCONFIG(TAG, "Uptime: %dmin" ,data);
  }

  else if (code == 0x9401){
    //Fbathing time minute downcounter.
    for (auto &listener : listeners_) {listener->on_remaining_time(data);}
  }

  else if (code == 0x4200){
    //  probably clock
  }

  else if (code == 0x6001) {
    // Extract the second byte and the lower nibble of the third byte

    uint8_t second_byte = (data & 0xFF00) >> 8;
    uint8_t third_byte_lower_nibble = (data & 0x00F0) >> 4;

    // Calculate the humidity setting based on the pattern and adjust by subtracting 5
    uint16_t humidity_setting = (second_byte - 0x30) * 2 + (third_byte_lower_nibble / 8) - 5;

    ESP_LOGCONFIG(TAG, "Humidity setting: %d", humidity_setting);
    for (auto &listener : listeners_) {listener->on_humidity(humidity_setting);}
  }

  else if (code == 0x7180) {
    // Extract humidity percentage from the data
    uint16_t humidity_percentage = data & 0xFFFF; // Assuming the humidity is in the least significant byte

    ESP_LOGCONFIG(TAG, "Humidity percentage: %d%%", humidity_percentage);
    for (auto &listener : listeners_) {listener->on_humidity_percentage(humidity_percentage);}
  }

  else {
    ESP_LOGCONFIG(TAG, "Packet: %s" ,format_hex_pretty(packet).c_str());
    ESP_LOGCONFIG(TAG, "Code: %04x" ,code);
    ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  }

  packet.clear();
}

 void SAUNA360Component::send() {
 // not in use yet
}

void SAUNA360Component::apply_elite_heater_on_action() {
  //not sure yet, send data value changed in somepoint? just to keep values before if changes again?
  //#98 40 07 70 00 00 04 00 40 74 91 6E 9C
  // 98.40.07.70.00.00.00.00.40.95.59.9C
  this->flush();
  //std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x40, 0x74, 0x91, 0x6E, 0x9C });
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x40, 0x95, 0x59, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER ON");
    return;
  }

void SAUNA360Component::apply_elite_heater_off_action() {
  // 98 40 07 70 00 00 04 00 80 6C D6 9C
  // 98.40.07.70.00.00.00.00.80.8D.1E.9C
  this->flush();
  //std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x04, 0x00, 0x80, 0x6C, 0xD6, 0x9C });
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x80, 0x8D, 0x1E, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER OFF");
    return;
  }

void SAUNA360Component::apply_elite_heater_standby_action() {
  //#98 40 07 70 00 00 00 00 C0 0A 9C
  // 98.40.07.70.00.00.00.00.C0.0A.94.9C
  this->flush();
  //std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0A, 0x9C });
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0A, 0x94, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER STANDBY");
    return;
  }


void SAUNA360Component::apply_pure_power_toggle_action() {
  this->flush();

  // Define the sequence of packets to send
  std::vector<std::vector<uint8_t>> packets = {
    {0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0x0A, 0x9C},
    // Add additional packets here if necessary
  };

  // Log current queue size
  ESP_LOGCONFIG(TAG, "TX QUEUE SIZE BEFORE PUSH: %d", this->tx_queue_.size());

  for (const auto& packet : packets) {
    if (this->tx_queue_.size() < MAX_QUEUE_SIZE) {
      this->tx_queue_.push(packet);
      ESP_LOGCONFIG(TAG, "Queued FRAME: %s", format_hex_pretty(packet).c_str());
    } else {
      ESP_LOGW(TAG, "TX Queue full. Cannot queue FRAME: %s", format_hex_pretty(packet).c_str());
    }
  }

  ESP_LOGCONFIG(TAG, "PURE POWER TOGGLED");
}

void SAUNA360Component::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");

}

}  // namespace sauna360
}  // namespace esphome
