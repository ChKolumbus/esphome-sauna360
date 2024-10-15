#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace sauna360 {

class SAUNA360Listener {
 public:
   virtual void on_temperature(uint16_t temperature){};
};

class SAUNA360Component : public uart::UARTDevice, public Component {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void register_listener(SAUNA360Listener *listener) { this->listeners_.push_back(listener); }

  protected:
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    void handle_packet_(std::vector<uint8_t> packet);
    std::vector<uint8_t> rx_message_;
    std::vector<SAUNA360Listener *> listeners_{};
};


}  // namespace sauna360
}  // namespace esphome