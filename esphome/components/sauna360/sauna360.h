#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sauna360_uart_component {

class Sauna360UARTComponent : public uart::UARTDevice, public Component {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void set_temperature_sensor(sensor::Sensor *temperature_sensor) { this->temperature_sensor_ = temperature_sensor; }

  protected:
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    void handle_packet_(std::vector<uint8_t> packet);
    std::vector<uint8_t> rx_message_;
    sensor::Sensor *temperature_sensor_{nullptr};
};


}  // namespace sauna360_uart_component
}  // namespace esphome