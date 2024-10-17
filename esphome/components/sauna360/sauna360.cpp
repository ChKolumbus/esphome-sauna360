#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "sauna360.h"

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

  // Discard stale data
//if (!this->rx_message_.empty() && (now - this->last_rx_ > 50)) {
  if (!this->rx_message_.empty() && (now - this->last_rx_ > 50)) {
    ESP_LOGD(TAG, "Discarding %d bytes of unparsed data", this->rx_message_.size());
    this->rx_message_.clear();
  }

  while (this->available()) {
    this->last_rx_ = now;
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
    // Send packets during bus silence
//if (this->rx_message_.empty() && (now - this->last_rx_ > 50) && (now - this->last_rx_ < 100) && (now - this->last_tx_ > 200)) {
  if (this->rx_message_.empty() && (now - this->last_rx_ > 50) && (now - this->last_rx_ < 100) && (now - this->last_tx_ > 200)) {
#ifdef USE_TIME
    // Only build time packet when bus is silent and queue is empty to make sure we can send it right away
    if (this->send_time_requested_ && this->tx_queue_.empty() && this->do_send_time_())
      this->send_time_requested_ = false;
#endif
    // Send the next packet in the queue
    if (!this->tx_queue_.empty()) {
      auto packet = std::move(this->tx_queue_.front());
      this->tx_queue_.pop();

      this->write_array(packet);
      this->flush();

      this->last_tx_ = now;
    }
  }
}

void SAUNA360Component::handle_char_(uint8_t c) {
  if (c == 0x9C) {
    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());
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
      // print('Unknown escape sequence: 91 ' + '{:02x}'.format(d))
      packet.push_back(0x91);
      }
    }
    packet.push_back(d);
  }
  // Check the CRC
  // uint16_t esphome::crc16be (const uint8_t * data, uint16_t	len, uint16_t	crc, uint16_t poly,	bool refin,	bool refout) 	
  // uint8_t *crc_packet = packet.data();
   //size_t len = packet.size(); // for debug
   //if (len >= 4){ESP_LOGCONFIG(TAG, "Packet CRC: %s" ,format_hex_pretty(packet).c_str());} // for debug
   
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
  //ESP_LOGCONFIG(TAG, "Adress: hex %.2X dec %d" ,address ,address);
  //ESP_LOGCONFIG(TAG, "Type: hex %.2X dec %d" ,packetType ,packetType);


  uint16_t code = ((uint16_t) packet[2]) << 8; // add MSB 
  code |= ((uint16_t) packet[3]); // add LSB
  //ESP_LOGCONFIG(TAG, "Code: %.2X" ,code);

  uint32_t data = ((uint32_t) packet[4]) << 24; // put the MSB in place
  data |= ((uint32_t) packet[5]) << 16; // add next byte
  data |= ((uint32_t) packet[6]) << 8; // add next byte
  data |= ((uint32_t) packet[7]); // add LSB"
  //ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  

   // Only take codes from the heater to control // hide code 4200 clock?
  if ((packetType == 0x07) || (packetType == 0x09) || (code == 0x4200)) {
    packet.clear();
    return;
  }
  ESP_LOGCONFIG(TAG, "Packet: %s" ,format_hex_pretty(packet).c_str());
  ESP_LOGCONFIG(TAG, "Code: %04x" ,code);
  ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  if (code == 0x6000){
    // Found temperature data point. Split into set point and actual value
    int actualTemp = (data & 0x00007FF) / 9.0;
    //ESP_LOGCONFIG(TAG, "Actual temp: %d" ,actualTemp);
    for (auto &listener : listeners_) {listener->on_temperature(actualTemp);}
    int setPointTemp = ((data >> 11) & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature_setting(setPointTemp);}
    //ESP_LOGCONFIG(TAG, "Set temp: %d" ,setPointTemp);
  }

  else if (code == 0x3400){
    // Found state code.
    ESP_LOGCONFIG(TAG, "State code: %08x" ,data);

    bool ready = (data & 0x00000001) != 0;
    ESP_LOGCONFIG(TAG, "Ready: %zu" ,ready);
    uint32_t light = (data & 0x00000008) != 0;
    ESP_LOGCONFIG(TAG, "Light: %zu" ,ready);
    bool heater = (data & 0x00000010) != 0;
    ESP_LOGCONFIG(TAG, "Heater: %zu" ,ready);
  }

  else if (code == 0x9400){
    //Found total operation time minute upcounter.
    ESP_LOGCONFIG(TAG, "Uptime: %dmin" ,data);
  }

  else if (code == 0x9401){
    //Found bathing time minute downcounter.
    //ESP_LOGCONFIG(TAG, "Remaining time: %dmin" ,data);
    for (auto &listener : listeners_) {listener->on_remaining_time(data);}
  }

  else {
    //ESP_LOGCONFIG(TAG, "Packet: %s" ,format_hex_pretty(packet).c_str());
    //ESP_LOGCONFIG(TAG, "Code: %04x" ,code);
    //ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  }

  packet.clear();
}

 void SAUNA360Component::send() {

/*
  // packet for send queue. All fields are big-endian except for the little-endian checksum.
  std::vector<uint8_t> packet;
  packet.reserve(6 + 3 * data_len);

  packet.push_back(this->address_ >> 8);
  packet.push_back(this->address_ >> 0);
  packet.push_back(device_address >> 8);
  packet.push_back(device_address >> 0);

  for (int i = 0; i < data_len; i++) {
    packet.push_back(data[i].id);
    packet.push_back(data[i].value >> 8);
    packet.push_back(data[i].value >> 0);
  }

  auto crc = crc16(packet.data(), packet.size());
  packet.push_back(crc >> 0);
  packet.push_back(crc >> 8);

  this->tx_queue_.push(packet);
  return true;
  */
}

void SAUNA360Component::apply_heater_on_action() {
  //#98 40 07 70 00 00 04 00 40 74 91 6E 9C
  //data: [0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x04, 0x00, 0x40, 0x74, 0x91, 0x6E, 0x9C]
  //  40.07.70.00.00.00.00.40 
  std::vector<uint8_t> packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x40, 0x74, 0x91, 0x6E, 0x9C });
  this->tx_queue_.push(packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER ON");
    return;
  }

void SAUNA360Component::apply_heater_off_action() {
  //#98 40 07 70 00 00 04 00 80 6C D6 9C
  //data: [0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x04, 0x00, 0x80, 0x6C, 0xD6, 0x9C]
  // 40.07.70.00.00.00.00.80
  std::vector<uint8_t> packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x04, 0x00, 0x80, 0x6C, 0xD6, 0x9C });
  this->tx_queue_.push(packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER OFF");
    return;
  }

void SAUNA360Component::apply_heater_standby_action() {
  //40.07.70.00.00.00.00.C0.0A
  //#98 40 07 70 00 00 00 00 C0 0A 9C
  //data: [0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0A, 0x9C]
  // 40.07.70.00.00.00.00.80
  std::vector<uint8_t> packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0A, 0x9C });
  this->tx_queue_.push(packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER STANDBY");
    return;
  }
void SAUNA360Component::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");

}


}  // namespace sauna360
}  // namespace esphome