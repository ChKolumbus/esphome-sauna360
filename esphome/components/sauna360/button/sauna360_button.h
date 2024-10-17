#pragma once

#include "esphome/components/button/button.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360 {

class SAUNA360HeaterOnButton : public button::Button, public Parented<SAUNA360Component> {
 public:
  SAUNA360HeaterOnButton() = default;

 protected:
  void press_action() override;
};

class SAUNA360HeaterOffButton : public button::Button, public Parented<SAUNA360Component> {
 public:
  SAUNA360HeaterOffButton() = default;

 protected:
  void press_action() override;
};

}  // namespace sauna360
}  // namespace esphome