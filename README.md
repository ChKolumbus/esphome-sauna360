# ESPHome Sauna360

ESPHome rs485 gateway component for brands of Sauna360 electrical sauna heaters.

ESPHome is a system to control your microcontrollers by simple yet powerful configuration files and control them remotely through Home Automation systems like Home assistant and openHAB.

Brands, Sauna360, Helo, Tylö, TyloHelo, finnleo, amerec

which use Adapter circuit board OLEA 103 and PURE/ELITE control panels.

Known models (not tested):
Himalaya Elite, Roxx Elite, helo Steam, Helo Steam Pro, Rocher Elite, Contactor box WE30, WE40, WE 50, WE 53, WE 52.

Tested with:
Helo Roxx Elite
atom lite + tail485

Connect cable directly to free control panel RJ10 plug and your device. Max ouput power is not known so connect devices at your OWN RISK!
Measured with 40A DC clamp so not very presice on lower currents:
Elite panel measured power around 200mA @ 12v
Pure panel very low idle power, peak 100mA
Atom lite spec power 500mA @ 5v so with converter 12V->5V takes 208mA from supply so should be pretty safe to use. 
Every panel connector seems to have own voltage output/or fuse i know cause i blacked out one connector shortcircuiting when developing.
Every panel connector seems to send same messages if panel is connected to another.
If errors in communications or long wire use twisted pair cable like LIYY (TP) 2 x 2 x 0,14 

Cable pinout, telephone cable RJ10:
(Colors may vary, so measuring voltage is advised)
PIN 1: A    BLACK
PIN 2: B    RED
PIN 3: 12V  GREEN
PIN 4: GND  YELLOW

or directly between panel cable (4 x 0,15 mm² on manual and actual LIYY (TP) 2 x 2 x 0,14)
(Colors may vary, so measuring voltage is advised)
PIN 1: A    YELLOW
PIN 2: B    BROWN
PIN 3: 12V  WHITE
PIN 4: GND  GREEN

It is recommended to put ESPhome Wifi device outside of sauna. Typically there is foil behind woodpanels so wifi reception might be poor in saunaroom and devices probably don't like to heat up.
