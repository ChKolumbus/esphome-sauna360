#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace sauna360_uart_component {

class Sauna360UARTComponent : public uart::UARTDevice, public Component {
  public:
    virtual void update_actualTemp(int value) {}
    virtual void update_setPointTemp(int value) {}
    void setup() override;
    void loop() override;
    void dump_config() override;

  protected:
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    void handle_packet_(std::vector<uint8_t> packet);
    std::vector<uint8_t> rx_message_;
};


}  // namespace sauna360_uart_component
}  // namespace esphome