/*
*Smart Plant Watering System main module
*SmartPlantWateringSystemUsingArduinouno.ino    create on: 28/10/2018
*Copyright (C) 2007 Free Software Foundation, Inc. <arduinounomagic@gmail.com>
*
*For more detail please visit:https://www.arduinounomagic.com/2018/10/smart-plant-watering-system-using.html
*
*for more projects please visit://www.arduinounomagic.com
*/

#define WATERPUMP 6 //pump connected to pin 6
#define SENSOR A0 //soil sensor analog pin connected to pin A0
#define PORTNUMBER  9600 // opens serial port, sets data rate to 9600 bps
#define HOUR 3600

const int  AirValue = 740; //min air moisture value
const int  WaterValue = 250; //max water moisture value
long watered_recently = 0; //flag for recently watered plant, mostly failsafe for overwatering
int val_percent;

void setup()
{
  Serial.begin(PORTNUMBER); 
  pinMode(6,OUTPUT); //Set pin 6 as OUTPUT pin
  pinMode(SENSOR,INPUT); //Set pin A0 as input pin, to receive data from Soil moisture sensor.
  digitalWrite(WATERPUMP,HIGH);//pump should be off initally, relay is on Normally Open mode
}

void loop() 
{ 
  
  int val = analogRead(SENSOR); //stores the value received from Soil moisture sensor in variable val 
  //Serial.print("Plant has been watered in the past 24 hours: ");
  if ( watered_recently > 0 )
  {
    //Serial.println("Yes");  
  }
  else
  {
    //Serial.println("No");
  }
  
  //val = 1023 - val; // low moisture equals low val
  val_percent = map(val, AirValue, WaterValue, 0, 100);  
 
  //if( val >= 450 )
  if ( val_percent >= 80 && watered_recently == -1 )
  {    
    //Serial.print("Moisture is high so pump will off, moisture value is: ");
    //Serial.println(val_percent); //print soil moisture sensor value
    digitalWrite(WATERPUMP,HIGH); //pump goes off, relay is on Normally Open mode
    watered_recently = 24UL * HOUR; //after plant is watered, wait at least 24 hours before watering again
  }
  //else if( val <= 320 && watered_recently == 0 )
  else if( val_percent <= 60 && watered_recently == 0 )
  {
    //Serial.print("Moisture is low so pump should turn on, moisture value is: ");
    //Serial.println(val_percent);//print soil moisture sensor value
    digitalWrite(WATERPUMP,LOW); //pump goes on, relay is on Normally Open mode
    watered_recently = -1;
  }
  else
  {
    //Serial.print("Moisture value is: ");
    //Serial.println(val_percent); 
  }
  delay(1000); //Wait for a second and then continue the loop
  if (watered_recently > 0)
  {
    watered_recently = watered_recently - 1; 
  }
}
