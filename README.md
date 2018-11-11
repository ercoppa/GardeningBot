# GardeningBot
Yet another gardening bot based on ESP8266 NodeMCU v3

## Hardware
* ESP8266 NodeMCU v3 ($3 on AliExpress)
* Water Pump  QR30B DC 5V 2.3W 220L/H ($6 on AliExpress)
* PowerBank 2200mAh PURO BB22C1WHI (€6 on Ebay): no led when discharging, powered on even when current drain is few mA
* Capacitive Soil Moisture Sensor ($1.5 on AliExpress)
* 2 x 1A 1000V Diode 1N4007 ($0.60 x 100 pcs on AliExpress)
* 2 x BC337 BJT ($3 x 200 pcs on AliExpress)
* 40x60mm FR-4 2.54mm Double Side Prototype PCB (€1 on Ebay)
* 2 x Aluminum electrolytic capacitor 470uF (€4 x 120 pcs on Ebay)
* Multilayer Ceramic Capacitor 10pF + 100nF (€4 x 120 pcs on AliExpress)
* Water Tank 5L

## Software
See source code `WifiGardeningBot.ino`

## Notes
* ESP8266 NodeMCU v3 has a lower power usage (~1.8mA) when in deep sleep mode compared to NodeMCU v1 and NodeMCU v2. On v1 and v2, you need to [remove the power regulator](https://tinker.yeoman.com.au/2016/05/29/running-nodemcu-on-a-battery-esp8266-low-power-consumption-revisited/) to get a small power usage in deep sleep. With a ~1.8mA current drain, you get more than 2 weeks when using a 2200mAh power bank.
* [Soil Moisture Sensors](https://www.sparkfun.com/products/13322) that measure resistance give totally [unreliable readings](https://mtlynch.io/greenpithumb/) just after just 1-2 months. Avoid them. Capacitive soil sensors are better, but still sometime unreliable. After many and long experiments, I use capacitive sensors but only to get *feedback* about the soil moisture without using this data to decide whether it it the right time to activate the water pump. 
* Finding a power banks that: 
  1. do not waste power (e.g., flashing LEDs) and
  2. keep working even with a very low current load  
  
is hard.
