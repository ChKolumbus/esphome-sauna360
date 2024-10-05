
# ESPHome Sauna360

ESPHome rs485 gateway component for brands of Sauna360 electrical sauna heaters.  

ESPHome is a system to control your microcontrollers by simple yet powerful configuration files and control them remotely through Home Automation systems like Home assistant and openHAB.  

Brands, Sauna360, Helo, Tylö, TyloHelo, finnleo, amerec  

which use Adapter circuit board OLEA 103 and PURE/ELITE control panels.  
![Screenshot 2024-10-05 170232](https://github.com/user-attachments/assets/ef990b1d-3291-422f-9d9e-5d07e3631fae)


Known models which have OLEA 103(not tested):  

Himalaya Elite, Roxx Elite, helo Steam, Helo Steam Pro, Rocher Elite, Contactor box WE30, WE40, WE 50, WE 53, WE 52.  
With contactor box you can make your traditional sauna heater to work with PURE/ELITE control panels.


Tested with:  
Helo Roxx, with ELITE and PURE 2.0 panels
atom lite + tail485  

Connect cable directly to free control panel RJ10 plug and your device. Max ouput power is not known so connect devices at your OWN RISK!  
![Screenshot 2024-10-05 170649](https://github.com/user-attachments/assets/1033c7dd-9285-4b00-a9e4-8e67a3d91566)  
Measured with 40A DC clamp so not very presice on lower currents:  
Elite panel measured power around 200mA @ 12v  
Pure panel very low idle power, peak 100mA  
Atom lite spec power 500mA @ 5v so with converter 12V->5V takes 208mA from supply so should be pretty safe to use.   
Every panel connector seems to have own voltage output/or fuse i know cause i blacked out one connector shortcircuiting when developing.  
Every panel connector seems to send same messages if panel is connected to another.  
If errors in communications or long wire use twisted pair cable like LIYY (TP) 2 x 2 x 0,14   

Cable pinout, telephone cable RJ10:  
![atomtail](https://github.com/user-attachments/assets/3376d275-32a4-4ce1-be8a-635d06be098f)  
![rj10pins](https://github.com/user-attachments/assets/e0e3bc6f-94d8-4074-9381-20c301f4cd1e)  
(Colors may vary, so measuring voltage is advised)  
|PIN|DESIGNATION|COLOR|
|--|--|--|
|1|A|BLACK|
|2|B|RED|
|3|12V|GREEN|
|4|GND|YELLOW|


or directly between panel cable (4 x 0,15 mm² on manual and actual LIYY (TP) 2 x 2 x 0,14)
![Screenshot 2024-10-05 170711](https://github.com/user-attachments/assets/668430a8-a657-47a2-b79a-1f6c7468b21d)  
(Colors may vary, so measuring voltage is advised)  
|PIN|DESIGNATION|COLOR|
|--|--|--|
|1|A|YELLOW|
|2|B|BROWN|
|3|12V|WHITE|
|4|GND|GREEN|

It is recommended to put ESPhome Wifi device outside of sauna. Typically there is foil behind woodpanels so wifi reception might be poor in saunaroom and devices probably don't like to heat up.

