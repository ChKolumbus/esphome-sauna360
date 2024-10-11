#include "esphome/core/log.h"
#include "sauna360_sensor.h"

namespace esphome {
namespace sauna360_uart_component {

static const char *const TAG = "sauna360";

void Sauna360Sensor::actualTemp(int value) {
  if (this->actual_temp_sensor_) {
      this->actual_temp_sensor_->publish_state(value);
  }
}

void Sauna360Sensor::setPointTemp(int value) {
  if (this->set_temp_sensor_) {
      this->set_temp_sensor_->publish_state(value);
  }
}

void Sauna360Sensor::dump_config() {
  LOG_SENSOR("  ", "Actual Temperature", this->actual_temp_sensor_);
  LOG_SENSOR("  ", "Set temperature", this->set_temp_sensor_);
}

}  // namespace sauna360_uart_component
}  // namespace esphome
