import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
# from esphome.const import

from .. import (
    sauna360_ns,
    SAUNA360Component,
    CONF_SAUNA360_ID,
)

SAUNA360HeaterOnButton = sauna360_ns.class_("SAUNA360HeaterOnButton", button.Button)
SAUNA360HeaterOffButton = sauna360_ns.class_("SAUNA360HeaterOffButton", button.Button)
SAUNA360HeaterStandbyButton = sauna360_ns.class_("SAUNA360HeaterStandbyButton", button.Button)
SAUNA360HeaterPowerToggleButton = sauna360_ns.class_("SAUNA360HeaterPowerToggleButton", button.Button)
SAUNA360LightPowerToggleButton = sauna360_ns.class_("SAUNA360LightPowerToggleButton", button.Button)

CONF_SAUNA_ON = "elite_heater_on"
CONF_SAUNA_OFF = "elite_heater_off"
CONF_SAUNA_STANDBY = "elite_heater_standby"
CONF_SAUNA_POWER_TOGGLE = "pure_power_toggle"
CONF_SAUNA_LIGHT_TOGGLE = "pure_light_toggle"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_SAUNA360_ID): cv.use_id(SAUNA360Component),
    cv.Optional(CONF_SAUNA_ON): button.button_schema(
        SAUNA360HeaterOnButton,
        icon="mdi:power-on",
    ),
    cv.Optional(CONF_SAUNA_OFF): button.button_schema(
        SAUNA360HeaterOffButton,
        icon="mdi:power-off",
    ),
    cv.Optional(CONF_SAUNA_STANDBY): button.button_schema(
        SAUNA360HeaterStandbyButton,
        icon="mdi:power",
    ),
    cv.Optional(CONF_SAUNA_POWER_TOGGLE): button.button_schema(
        SAUNA360HeaterPowerToggleButton,
        icon="mdi:power",
    ),
    cv.Optional(CONF_SAUNA_LIGHT_TOGGLE): button.button_schema(
        SAUNA360LightPowerToggleButton,
        icon="mdi:power",
    ),
}


async def to_code(config):
    sauna360_component = await cg.get_variable(config[CONF_SAUNA360_ID])
    if elite_heater_on := config.get(CONF_SAUNA_ON):
        b = await button.new_button(elite_heater_on)
        await cg.register_parented(b, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_elite_heater_on_button(b))
    if elite_heater_off := config.get(CONF_SAUNA_OFF):
        b = await button.new_button(elite_heater_off)
        await cg.register_parented(b, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_elite_heater_off_button(b))
    if elite_heater_standby := config.get(CONF_SAUNA_STANDBY):
        b = await button.new_button(elite_heater_standby)
        await cg.register_parented(b, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_elite_heater_standby_button(b))
    if pure_power_toggle := config.get(CONF_SAUNA_POWER_TOGGLE):  
        b = await button.new_button(pure_power_toggle)
        await cg.register_parented(b, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_pure_power_toggle_button(b))
    if pure_light_toggle := config.get(CONF_SAUNA_LIGHT_TOGGLE):  
        b = await button.new_button(pure_light_toggle)
        await cg.register_parented(b, config[CONF_SAUNA360_ID])
        cg.add(sauna360_component.set_pure_light_toggle_button(b))