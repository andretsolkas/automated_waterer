#include "WiFiEsp.h"
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//All things WiFi module needs
char ssid[] = SECRET_SSID;    //  your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiEspClient  client;

#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11); // RX, TX
#define ESP_BAUDRATE  9600

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


//All things waterer needs
#define WATERPUMP 6 //pump connected to pin 6
#define SENSOR A0 //soil sensor analog pin connected to pin A0
#define PORTNUMBER  9600 // opens serial port, sets data rate to 9600 bps
#define HOUR 3600

const int  AirValue = 740; //min air moisture value
const int  WaterValue = 250; //max water moisture value
long watered_recently = 0; //flag for recently watered plant, mostly failsafe for overwatering
int val_percent, counter;
double avg_moisture;

void setup() {
  Serial.begin(PORTNUMBER); 
  pinMode(6,OUTPUT); //Set pin 6 as OUTPUT pin
  pinMode(SENSOR,INPUT); //Set pin A0 as input pin, to receive data from Soil moisture sensor.
  digitalWrite(WATERPUMP,HIGH);//pump should be off initally, relay is on Normally Open mode
  while(!Serial){
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }  
  
  // initialize serial for ESP module  
  setEspBaudRate(ESP_BAUDRATE);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  Serial.print("Searching for ESP8266..."); 
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  Serial.println("found it!");
    
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  counter = 0;
  avg_moisture = 0;
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  //Check soil moisture and water if needed
  int val = analogRead(SENSOR); //stores the value received from Soil moisture sensor in variable val 
  Serial.print("Plant has been watered in the past 24 hours: ");
  if ( watered_recently > 0 )
  {
    Serial.println("Yes");  
  }
  else
  {
    Serial.println("No");
  }
  
  val_percent = map(val, AirValue, WaterValue, 0, 100);  
 
  if ( val_percent >= 80 && watered_recently == -1 )
  {    
    Serial.print("Moisture is high so pump will off, moisture value is: ");
    Serial.println(val_percent); //print soil moisture sensor value
    digitalWrite(WATERPUMP,HIGH); //pump goes off, relay is on Normally Open mode
    watered_recently = 24UL * HOUR; //after plant is watered, wait at least 24 hours before watering again
  }
  else if( val_percent <= 60 && watered_recently == 0 )
  {
    Serial.print("Moisture is low so pump should turn on, moisture value is: ");
    Serial.println(val_percent);//print soil moisture sensor value
    digitalWrite(WATERPUMP,LOW); //pump goes on, relay is on Normally Open mode
    watered_recently = -1;
  }
  else
  {
    Serial.print("Moisture value is: ");
    Serial.println(val_percent); 
  }
  counter += 1;
  avg_moisture += val_percent;
  if (counter > 19)
  {
    avg_moisture = avg_moisture / 20;
    counter = 0;
    Serial.println("Sending average moisture value to ThingSpeak...");
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeField(myChannelNumber, 1, (int) avg_moisture, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    avg_moisture = 0;
  }
  
  if (watered_recently > 0)
  {
    watered_recently = watered_recently - 1; 
  }
  
  delay(1000); // Wait 1 second to update the channel again
}

// This function attempts to set the ESP8266 baudrate. Boards with additional hardware serial ports
// can use 115200, otherwise software serial is limited to 19200.
void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");

  for(int i = 0; i < 6; i++){
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial1.print(",8,1,0,0\r\n");
    delay(100);  
  }
    
  Serial1.begin(baudrate);
}
