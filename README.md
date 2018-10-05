# GearIndicator
Display's the current gear a Suzuki DL1000 is using

1. Project aim

This GearIndicator was built for a 2003 Suzuki DL1000. It always annoyed not to know in what gear I am currently using. A lot of motorcycles got the Luxury of a gear display. But unfortunately not the old DL1000. There are some ready made solution you can buy that are suitable for many different types of motorcycles. But they come with a price tag and usually are > AUD $150. The good thing is that many motorcycles got a "signal wire" that indicates to the ECU which gear is in use. For the DL1000 this wire would have a voltage between 0 and 5 volts. All that needs to be done is measure the voltage on a micro controller and have it display something on an LED matrix display.

2. Voltage measurement

The ESP 8266 can measure a voltage between 0 and 1 volt on his ADC. To be able to read the voltage from the signal wire I had to build a voltage divider. There are some online tools available but its not a difficult calculation. All you need are two resistors. In my case I started with a 100 and 470 ohm combination. That worked on the bench, but once hooked up to the motorcycle it started to confuse the ECU. What I learned is that it is smarter to use high ohm resistors as this will interfere less with the signal on the wire. I had good results with a combination of 47k and 22k.

3. Function

The program does what is promised. Based on the voltage detected on the ADC it will show N or 1 to 6 on the 8x8 matrix display. It also display a Suzuki logo at start which can be changed of course. One button can be used to switch between high and low brightness (day/night). I would have rather used a photo resistor for that but the ESP8266 only got one ADC. The numbers/signs are generated with an online tool - the link to it is in the code. As we got wireless I am making use of it. There are the following options:

http://ip/demo - show all signs/numbers sequentially on the display

http://ip/learn - this starts a 3 minute long process reading values from the ADC pin and storing them in gears.txt. This is the time where you start your motorcycle and shift through all gears

http://ip/show - displays the values we just stored in gears.txt. This is the tool you use to change the values in the program code for every gear. It is unlikely that my values work for you - they are also dependant on the voltage divider you use.

http://ip/reset - deletes the gears.txt file and might require a reboot of the ESP. It is actually unnecessary - everytime you start /learn it will overwrite the old values. But keep in mind that this does not change your program code - thats always something that needs to be done.

4. Wiring

It is pretty easy to wire it all up. The voltage divider has 3 different wires. One will go to your motorcycles gear position switch. On a VStrom thats usually a pink wire that can be located on the left side close to the tank. The "input" of the voltage divider need to be connected to this wire. The ground of the voltage divider should go to the battery (don't put it on the frame) and the remaining one is the "output". It will go on pin AD0 on your ESP8266 (thats the ADC). Whats left is the power and the display. To provide the ESP8266 with 5V power we bought a regulator. It will get 12V from your motorcycle and the 5V it outputs will go to VCC on your ESP. You need to find a power line that is providing 12V when ingition is turned on. GND can be connected to your battery. The display has to be hooked up to the ESP power as well and there are 3 lines to control it. They are labeled on the little circuit board. Data In goes to pin 7 (GPIO13), CLK to pin 5 (GPIO14) and CS to pin 8 (CS).

5. Parts List

ESP8266 (e.g. NodeMCU eBay $8 or Jaycar XC3800 $39), 8x8 MAX7219 LED Matrix (Jaycar XC4499), Small case, 47K Resistor, 22K Resistor, 1nf capacitor (optional), 5 Volt regulator (Jaycar ZV1505) and some wires
