#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "sauna360.h"

namespace esphome {
namespace sauna360_uart_component {

static const char *TAG = "sauna360";

void Sauna360UARTComponent::setup() {

}

void Sauna360UARTComponent::loop() {
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
}

void Sauna360UARTComponent::handle_char_(uint8_t c) {
  if (c == 0x9C) {
    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());
    this->rx_message_.clear();
    this->handle_frame_(frame);
    return;
  }
  this->rx_message_.push_back(c);
}

void Sauna360UARTComponent::handle_frame_(std::vector<uint8_t> frame) {
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
   if (crc16be(packet.data(), 16, 0xffff, 0x90d9, false, false)){
    packet.pop_back();
    this->handle_packet_(packet);
   }
   else {
    ESP_LOGCONFIG(TAG, "CRC ERROR");
   }
   frame.clear();
}

void Sauna360UARTComponent::handle_packet_(std::vector<uint8_t> packet) {
  // Decode the packet, but skip keepalive handshakes
  size_t len = packet.size();
  if (len <= 2){
    packet.clear();
    return;
  }
  int address = packet[0];
  int packetType = packet[1];
  //ESP_LOGCONFIG(TAG, "Adress: hex %.2X dec %d" ,address ,address);
  //ESP_LOGCONFIG(TAG, "Type: hex %.2X dec %d" ,packetType ,packetType);
  //ESP_LOGCONFIG(TAG, "Packet: %08x" ,packet);

  uint16_t code = ((uint16_t) packet[2]) << 8; // add MSB 
  code |= ((uint16_t) packet[3]); // add LSB
  //ESP_LOGCONFIG(TAG, "Code: %.2X" ,code);

  uint32_t data = ((uint32_t) packet[4]) << 24; // put the MSB in place
  data |= ((uint32_t) packet[5]) << 16; // add next byte
  data |= ((uint32_t) packet[6]) << 8; // add next byte
  data |= ((uint32_t) packet[7]); // add LSB"
  //ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  
/*
   // Only take codes from the heater to control
  if ((packetType != 0x07) & (packetType != 0x09)){
    packet.clear();
    return;
  }
*/

  if (code == 0x6000){
    // Found temperature data point. Split into set point and actual value
    int actualTemp = (data & 0x00007FF) / 9.0;
    ESP_LOGCONFIG(TAG, "Actual temp: %d" ,actualTemp);
    this->temperature_sensor_->publish_state(actualTemp);
    int setPointTemp = ((data >> 11) & 0x00007FF) / 9.0;
    ESP_LOGCONFIG(TAG, "Set temp: %d" ,setPointTemp);
    //update_setPointTemp(setPointTemp);
  }

  else if (code == 0x3400){
    // Found state code.
    ESP_LOGCONFIG(TAG, "State code: %08x" ,data);

    uint32_t ready = (data & 0x00000001);
    ESP_LOGCONFIG(TAG, "Ready: %zu" ,ready);
    uint32_t light = (data & 0x00000008);
    ESP_LOGCONFIG(TAG, "Light: %zu" ,ready);
    uint32_t heater = (data & 0x00000010);
    ESP_LOGCONFIG(TAG, "Heater: %zu" ,ready);
  }

  else if (code == 0x9400){
    //Found total operation time minute upcounter.
    ESP_LOGCONFIG(TAG, "Uptime: %dmin" ,data);
  }

  else if (code == 0x9401){
    //Found bathing time minute downcounter.
    ESP_LOGCONFIG(TAG, "Remaining time: %dmin" ,data);
  }

  else {
    //ESP_LOGCONFIG(TAG, "Packet not defined: %02x %02x %04x %08x" ,address ,packetType, code, data);
  }

  packet.clear();
}

void Sauna360UARTComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");

}


}  // namespace sauna360_uart_component
}  // namespace esphome