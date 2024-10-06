#include "esphome/core/log.h"
#include "sauna360.h"

namespace esphome {
namespace Sauna360_uart_component {

static const char *TAG = "sauna360_uart_component.component";

void Sauna360UARTComponent::setup() {

}

void Sauna360UARTComponent::loop() {

}

void Sauna360UARTComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Sauna360 UART component");
}

}  // namespace sauna360_uart_component
}  // namespace esphome
