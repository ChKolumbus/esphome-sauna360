import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE,
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)

from .. import Sauna360UARTComponent, sauna360_uart_component_ns

DEPENDENCIES = ["sauna360"]

CONF_SAUNA360_ID = "sauna360_id"
CONF_DEVICES = "devices"

Sauna360Sensor = sauna360_uart_component_ns.class_("Sauna360Sensor", cg.Component)

DEVICE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Sauna360Sensor),

        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(Sauna360UARTComponent),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SAUNA360_ID])
    if devices_cfg := config.get(CONF_DEVICES):
        for device_cfg in devices_cfg:
            var = cg.new_Pvariable(device_cfg[CONF_ID])
            await cg.register_component(var, device_cfg)
            cg.add(var.set_id(device_cfg[CONF_DEVICE]))
            if CONF_TEMPERATURE in device_cfg:
                sens = await sensor.new_sensor(device_cfg[CONF_TEMPERATURE])
                cg.add(var.actual_temp_sensor_(sens))
            if CONF_TEMPERATURE in device_cfg:
                sens = await sensor.new_sensor(device_cfg[CONF_TEMPERATURE])
                cg.add(var.set_temp_sensor_(sens))
            cg.add(parent.add_device(var, device_cfg[CONF_DEVICE]))
