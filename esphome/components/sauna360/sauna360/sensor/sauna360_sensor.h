
#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/preferences.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360_uart_component {

class Sauna360Sensor : public Component {
 public:
  void update_actualTemp(int value);
  void update_setPointTemp(int value);

  void dump_config() override;
  void setup() override;

 protected:
  SUB_SENSOR(setPointTemp)
  SUB_SENSOR(actualTemp)
};

}  // namespace sauna360_uart_component
}  // namespace esphome
