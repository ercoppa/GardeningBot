# GardeningBot
Yet another gardening bot based on ESP8266 NodeMCU v3

## Requirements
* The gardening system should water a single plant.
* When watering, a telegram message should be sent (using WiFi) to notify this event. Additionally, the telegram message should report the soil mosture level before and after watering. 
* The system should water the plant every X hours. 
* The system should be powered by a cheap USB power bank and last at least a couple of weeks.
* The system should pump the water from a 5L water tank (placed 1 meter below the plant)

## Hardware
* ESP8266 NodeMCU v3 ($3 on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/esp8266-nodemcu-v3.png" width="200px">
* Water Pump  QR30B DC 5V 2.3W 220L/H ($6 on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/water-pump.png" width="200px">
* PowerBank 2200mAh PURO BB22C1WHI (€6 on Ebay): no led when discharging, powered on even when current drain is few mA <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/powerbank.png" width="200px">
* Capacitive Soil Moisture Sensor ($1.5 on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/sensor.png" width="200px">
* 2 x 1A 1000V Diode 1N4007 ($0.60 x 100 pcs on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/diode.png" width="200px">
* 2 x BC337 BJT ($3 x 200 pcs on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/bjt.png" width="200px">
* 40x60mm FR-4 2.54mm Double Side Prototype PCB (€1 on Ebay) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/pcb.png" width="200px">
* 2 x Aluminum electrolytic capacitor 470uF (€4 x 120 pcs on Ebay) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/cap1.png" width="200px">
* Multilayer Ceramic Capacitor 10pF + 100nF (€4 x 120 pcs on AliExpress) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/cap2.png" width="200px">
* Water Tank 5L <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/tank.png" width="200px">
* Kit DIY Micro Drip Irrigation System (€8 on Wish) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/irrigation.jpg" width="200px">
* Clear Airtight Lunch Box (€2) <br/><img src="https://github.com/ercoppa/GardeningBot/raw/master/images/lunch-box.png" width="200px">
* Jumper wires + resistors

## Software
See source code `WifiGardeningBot.ino`

## Result

### System #1
<img src="https://github.com/ercoppa/GardeningBot/raw/master/images/G2.jpg" width="700px">

### System #2
<img src="https://github.com/ercoppa/GardeningBot/raw/master/images/G1.jpg" width="700px">

## Notes
* ESP8266 NodeMCU v3 has a lower power usage (~1.8mA) when in deep sleep mode compared to NodeMCU v1 and NodeMCU v2. On v1 and v2, you need to [remove the power regulator](https://tinker.yeoman.com.au/2016/05/29/running-nodemcu-on-a-battery-esp8266-low-power-consumption-revisited/) to get a small power usage in deep sleep. With a ~1.8mA current drain, you get more than 2 weeks when using a 2200mAh power bank.
* [Soil Moisture Sensors](https://www.sparkfun.com/products/13322) that measure resistance give totally [unreliable readings](https://mtlynch.io/greenpithumb/) just after 1-2 months. Avoid them. Capacitive soil sensors are better, but still sometimes unreliable. After many and long experiments, I have used capacitive sensors but only to get *feedback* (via telegram messages) about the soil moisture. The readings are **NOT** used by the system to decide whether it is the right time to activate the water pump. If you want to build a system that waters a plant based on the soil mosture level then you need to buy reliable (=professional) but expensive sensors.
* Finding (cheap) power banks that: (i) do not waste power (e.g., flashing LEDs) and (ii) keep working even with a very low current load is *really* hard. 
