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
  void set_temperature_setting_sensor(sensor::Sensor *sensor) { this->temperature_setting_sensor_ = sensor; }
  void on_temperature_setting(uint16_t temperature_setting) override {
    if (this->temperature_setting_sensor_ != nullptr) {
      if (this->temperature_setting_sensor_->get_state() != temperature_setting) {
        this->temperature_setting_sensor_->publish_state(temperature_setting);
      }
    }
   }
  void set_remaining_time_sensor(sensor::Sensor *sensor) { this->remaining_time_sensor_ = sensor; }
  void on_remaining_time(uint16_t remaining_time) override {
    if (this->remaining_time_sensor_ != nullptr) {
      if (this->remaining_time_sensor_->get_state() != remaining_time) {
        this->remaining_time_sensor_->publish_state(remaining_time);
      }
    }
   }

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *temperature_setting_sensor_{nullptr};
  sensor::Sensor *remaining_time_sensor_{nullptr};
};

}  // namespace sauna360
}  // namespace esphome