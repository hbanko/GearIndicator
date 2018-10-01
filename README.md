# GearIndicator
Display's the current gear a Suzuki DL1000 is using

1. Project aim
This GearIndicator was built for a 2003 Suzuki DL1000. It alwats annoyed not to know in what gear I am currently in. A lot of motorcycles got the Luxury of a gear display. But unfortunately no the old DL1000. There are some ready made solution you can buy that are suitable for many different types of motorcycles. But they come with a price tag and usually are > AUD $150. The good thing is that many motorcycles got a "signal wire" that indicates to the ECU which gear is in use. For the DL1000 this wire would have a voltage between 0 and 5 volts. All that needs to be done is measure the voltage on a micro controller and display something on an LED matrix display.

2. Voltage measurement
The ESP 8266 can measure a voltage between 0 and 1 volt on his ADC. To be able to read the voltage from the signal wire I had to build a voltage divider. There are some online tools available but its not a difficult calculation. All you need are two resistor. In my case I started with a 100 and 470 ohm combination. That worked on the bench, but once hooked up to the motorcycle it started to confuse the ECU. What I learned is that it is smarter to use high ohm resistor as this will interfere less with the signal on the wire.

TBC
