import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart
from esphome.const import (
    CONF_ID, 
    CONF_FLOW_CONTROL_PIN,
)
from esphome import pins

DEPENDENCIES = ["uart"]

sauna360_ns = cg.esphome_ns.namespace("sauna360")
SAUNA360Component = sauna360_ns.class_("SAUNA360Component", cg.Component, uart.UARTDevice)

CONF_SAUNA360_ID = "sauna360_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SAUNA360Component),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
  "sauna360_uart",
  require_tx=True,
  require_rx=True,
  parity="EVEN",
  stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    if CONF_FLOW_CONTROL_PIN in config:
      pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
      cg.add(var.set_flow_control_pin(pin))