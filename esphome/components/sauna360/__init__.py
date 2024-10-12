import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_TEMPERATURE

DEPENDENCIES = ["uart"]


AUTO_LOAD = ["sensor"]
MULTI_CONF = True

CONF_ACTUAL_TEMPERATURE_STATE = "actual_temp"
CONF_SAUNA360 = "sauna360"

sauna360_uart_component_ns = cg.esphome_ns.namespace("sauna360_uart_component")
Sauna360UARTComponent = sauna360_uart_component_ns.class_(
    "Sauna360UARTComponent", cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(Sauna360UARTComponent)})
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)