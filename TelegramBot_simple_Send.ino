#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//_______________________ Initialize Telegram BOT
#define BOTtoken "5214355646:AAEbkjgO9eG7XLAdUax0jDR_VUIlGgFEeso"  //token of TestBOT
#define BOTname "LilSmartHome"
#define BOTusername "lilsmarthomebot"

//_______________________ Initialize Wifi connection to the router
char ssid[] = "Keenetic-5525";                      // your network SSID (name)
char password[] = "EYZco21EYZco21";                 // your network key

TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 5000;                                //mean time between scan messages
long Bot_lasttime;                                  //last time messages' scan has been done
long offset;
long chat_id;

Adafruit_BME280 bme;

float temperature, humidity, pressure;
int waterSensorADC = 0;
boolean waterAlertFlag = false;
const int analogInPin = A0;

/*********************************************
*                     setup()
**********************************************/
void setup() {

    Serial.begin(115200);
    bme.begin(0x76);
    delay(3000);
    
    // attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
    
    bot.begin();      // launch Bot functionalities
    offset = (bot.message[1][0]).toInt();                   // initial setup of an offset-key
}

/*********************************************
*                    loop()
**********************************************/
void loop() {
    // amount of messages read per time  (update_id, name_id, name, lastname, chat_id, text)
    // message   [i]                     [     0,      1,      2,      3,        4,     5   ]
    
    if (millis() > Bot_lasttime + Bot_mtbs)  {
        
        
        bot.getUpdates(String(offset));                     // checking new messages by interval
        executeMessage();
        
        printToSerial();                                    // printing our message array to Serial
        checkForWater();
        
        Bot_lasttime = millis();                            // Update next interval
    }
    

}


/*********************************************
*                    checkForWater()
**********************************************/
String checkForWater()   {
    if(waterSensorADC > 300)    {
        waterAlertFlag = true;
    }
    else  {
        waterAlertFlag = false;
    }
    
//    Serial.println("From checkForWater method.");
    
}

/*********************************************
*                    getClimate()
**********************************************/
String getClimate()   {
    String result;
    
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
    
    result = "T = " + String(temperature) + "°C" + "\t\t";
    result += "P = " + String(pressure) + "hPa" + "\t\t";
    result += "H = " + String(humidity) + "%";
    
    return result;
}


/*********************************************
*                    executeMessage()
**********************************************/
void executeMessage()   {

    String startMessage = "Wellcome from @lilsmarthomebot, your personal Bot on ESP8266 board.&#10;sdfsdf";
                    //  startMessage += "\/start - Print this message.\r";
                    //  startMessage += "\/climate - Get the climate info.\r";
    String climateMessage = getClimate();
    String waterAlertMessage = "ALERT! WATER!";
    
    offset = (bot.message[1][0]).toInt();                   // initial setup of an offset-key

    for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
        //        bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length());
        
        if (bot.message[i][5] == "\/start") {
            bot.sendMessage(bot.message[i][4], startMessage, "");
        }
        if (bot.message[i][5] == "\/climate") {
            bot.sendMessage(bot.message[i][4], climateMessage, "");
        }
//        else if(bot.message[i][5] != "")    {
//            bot.sendMessage(bot.message[i][4], "Send a correct command. Check it with \/start.", "");
//        }
        
    }

    if(waterAlertFlag)  {
        bot.sendMessage(bot.message[1][4], waterAlertMessage, "");
    }

    bot.message[0][0] = "";     // All messages have been replied - reset new messages
    // IDK mb it isn't need
    offset++;                   // increasing an offset
}


/*********************************************
*                    printToSerial()
**********************************************/
void printToSerial()    {
    
    // checking some special method
    Serial.print("offset =  ");
    Serial.println(offset);
    
    for (int i = 0; i < 3; i++)  {
        // Printing a message array
        for (int j = 0; j < 6; j++)  {
            Serial.print("message[");
            Serial.print(i);
            Serial.print("][");
            Serial.print(j);
            Serial.print("] = ");
            Serial.println(bot.message[i][j]);
        }
        Serial.println();
    }
    
    Serial.print("ADC value = ");
    waterSensorADC = analogRead(analogInPin);
    Serial.println(waterSensorADC);
    
    Serial.print("waterAlertFlag value = ");
    Serial.println(waterAlertFlag);
    
    Serial.println(getClimate());
    //    bot.sendMessage(bot.message[1][4], "hello! from ESP", "");
    //    bot.message[0][0] = "";                             // marking all messages as read
}
