#include <WiFi.h>
#include <PubSubClient.h>//callback    //reconnect 
#include <DS3231.h>
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>


/* change it with your ssid-password */
const char* ssid     = "Kharbanda net 2";
const char* password = "sunilsushil@";
/* this is the IP of PC/raspberry where you installed MQTT Server 
on Wins use "ipconfig" 
on Linux use "ifconfig" to get its IP address */
const char* mqtt_server = "192.168.0.6";
/* topics */
#define TOPIC "esp32"
/* create an instance of PubSubClient client */

/*#define SERIAL2_RXPIN 16
#define SERIAL2_TXPIN 17*/

DS3231 c;
RTCDateTime dt;
PZEM004Tv30 pzem(&Serial2);
LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  

}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.end();
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);


 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);

  c.begin();

  // Set sketch compiling time
  c.setDateTime(__DATE__, __TIME__);

  // initialize the LCD, 
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
   lcd.setCursor (0,0); //
   lcd.print("LCD2004 Test"); 
   lcd.setCursor (0,1); //   
 
   lcd.print("Please Wait - 3");  
   lcd.setCursor (0,1); // 
   delay(1000);        
   lcd.print("Please Wait - 2");  
   delay(1000); 
   lcd.setCursor (0,1); //      
   lcd.print("Please Wait - 1");  
   delay(1000);       
}
char* toCharArray(String str){
  return &str[0];
}
void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  dt = c.getDateTime();

  // For leading zero look to DS3231_dateformat example      
                     
  lcd.clear();// clearn previous values from screen
  lcd.setCursor (0,0); //character zero, line 1

  Serial.print("Date: ");
  Serial.print(dt.day);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.year);    Serial.println(" ");
  
  lcd.print(dt.day);   lcd.print("-");
  lcd.print(dt.month);  lcd.print("-");
  lcd.print(dt.year);    lcd.print(" ");
  Serial.print("Time: ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println("");
  lcd.print(dt.hour);   lcd.print(":");
  lcd.print(dt.minute); lcd.print(":");
  lcd.print(dt.second); lcd.print("");


    
    lcd.setCursor (0,1); //character 0, line 2
    String svoltage;
    float voltage = pzem.voltage();
    svoltage=String(voltage);
    if( !isnan(voltage) ){
        Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
        client.publish("esp32/Voltage", toCharArray(svoltage));

        lcd.print("V: "); lcd.print(voltage); lcd.print("V");
    } else {
        Serial.println("Error reading voltage");
        client.publish("esp32/Voltage","Error reading voltage","false");
        lcd.print("Error");
    }

    lcd.setCursor (10,1); //character 11, line 2
    String scurrent;
    float current = pzem.current();
    scurrent=(String)current;
    if( !isnan(current) ){
        Serial.print("Current: "); Serial.print(current); Serial.println("A");
        client.publish("esp32/Current", toCharArray(scurrent));
        lcd.print("C: "); lcd.print(current); lcd.print("A");
    } else {
        Serial.println("Error reading current");
        client.publish("esp32/Current","Error reading current");
        lcd.print("Error");
    }

   lcd.setCursor (0,2); //character 0, line 3
    String spower;
    float power = pzem.power();
    spower=(String)power;
    if( !isnan(power) ){
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
        client.publish("esp32/Power", toCharArray(spower));
        lcd.print("P: "); lcd.print(power); lcd.print("W");
    } else {
        Serial.println("Error reading power");
        client.publish("esp32/Power","Error reading power");
        lcd.print("Error");
    }

    lcd.setCursor (10,2); //character 11, line 3
    String senergy;
    float energy = pzem.energy();
    senergy=(String)energy;
    if( !isnan(energy) ){
        Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
        client.publish("esp32/Energy", toCharArray(senergy));
        lcd.print("E: "); lcd.print(energy,3); lcd.print("kWh");
    } else {
        Serial.println("Error reading energy");
        client.publish("esp32/Energy","Error reading energy");;
        lcd.print("Error");
    }

    lcd.setCursor (0,3); //character 0, line 4
    String sfrequency;
    float frequency = pzem.frequency();
    sfrequency=(String)frequency;
    
    if( !isnan(frequency) ){
        Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
        client.publish("esp32/Frequency", toCharArray(sfrequency));
        lcd.print("F: "); lcd.print(frequency, 1); lcd.print("Hz");
    } else {
        Serial.println("Error reading frequency");
        client.publish("esp32/Frequency","Error reading frequency");
        lcd.print("Error");
    }

    lcd.setCursor (10,3); //character 11, line 4
    String spf;
    float pf = pzem.pf();
    spf=(String)pf;
    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
        client.publish("esp32/PF", toCharArray(spf));
        lcd.print("PF: "); lcd.print(pf);
    } else {
        Serial.println("Error reading power factor");
        client.publish("esp32/PF","Error reading power factor");
        lcd.print("Error");
    }

    Serial.println();
    delay(2000);

  client.loop();
  }  
