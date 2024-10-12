import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_TYPE
from .. import Sauna360UARTComponent, CONF_SAUNA360_ID

DEPENDENCIES = ["sauna360"]

sensor_ns = cg.esphome_ns.namespace("sensor")
Sensor = sensor_ns.class_("Sensor", cg.EntityBase)

TYPES = "actual_temp"

SAUNA360_SENSOR_SCHEMA = sensor.SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(),
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = sensor.SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(Sensor),
        cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(Sauna360UARTComponent),
        cv.Required(CONF_TYPE): cv.one_of(*TYPES, lower=True),
    }
).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    paren = yield cg.get_variable(config[CONF_SAUNA360_ID])
    var = cg.new_Pvariable(config[CONF_ID])

    yield sensor.register_sensor(var, config)

    if config[CONF_TYPE] == "actual_temp":
        cg.add(paren.register_actual_temp(var))