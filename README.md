# automated_waterer
Automated plant watering using a capacitive moisture sensor and uploading data to ThingSpeak

The project consisted of a moisture sensor, a 12V submersible water pump, a WiFi module and an Arduino Uno logic board.

Remarks:
- Two separate humidity sensors died from humidity finding its way into their PCB, ultimately giving incosistent measurements. If the project is to be recreated for any non-recreational reason, please invest in a good soil humidity sensor.
- Be careful of the Vin of the WiFi module, an appropriate shield may be needed to avoid damage due to high/incosistent voltage.
- Max and min humidity values were tested in a dry and wet environment respectively, measures will vary between different sensors.


PS: The code of the .ino files is mistakenly identifies as C++
