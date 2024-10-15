#pragma once

#include "../sauna360.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sauna360 {

class SAUNA360Sensor : public SAUNA360Listener, public Component, sensor::Sensor {
 public:
  void dump_config() override;
  void set_temperature_sensor(sensor::Sensor *sensor) { this->temperature_sensor_ = sensor; }
  void on_temperature(uint16_t temperature) override {
    if (this->temperature_sensor_ != nullptr) {
      if (this->temperature_sensor_->get_state() != temperature) {
        this->temperature_sensor_->publish_state(temperature);
      }
    }
   }

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
};

}  // namespace sauna360
}  // namespace esphome