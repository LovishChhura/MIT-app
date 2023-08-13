#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <ESP32Time.h>

ESP32Time rtc(3600);  // offset in seconds GMT+1



LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the I2C address to 0x27 and dimensions to 20x4



int counter1 = 0;  // Counter controlled by Button 1

int t_Start = 0;

int t_stop = 0;

int optoPin2 = 13;

int optoPinrst = 33;

// int button1Pin = 18; // Button 1 pin

int prod_ButtonPin = 26;  // Button 2 pin

// int resetButtonPin = 13; // Reset button pin

// int ledPin = 4; // LED pin



//int button1State = LOW;

int optoPin2State = LOW;

//int lastButton1State = LOW;

int lastoptoPin2State = LOW;



int p_b_state = LOW;

int p_b_laststate = LOW;

unsigned long lastDebounceTime1 = 0;

unsigned long debounceDelay = 50;

unsigned long p_on_time = 0;

unsigned long prod_time = 0;

unsigned long n_prod_time = 0;

unsigned long eff_time = 0;

unsigned long var1 = 0;

unsigned long var2 = 0;



int counter1Address = 0;  // Address in the EEPROM emulation to store the counter1 value

int cycleTimeAddress = sizeof(counter1);  // Address in the EEPROM emulation to store the cycle time value



unsigned long cycleStartTime = 0;  // To store the time when the button is pressed

unsigned long cycleTime = 0;  // To store the calculated cycle time

//*by me
unsigned long newVar2 = 0;
// unsigned long var2 = 0;
unsigned long lastVar2Update = 0;
unsigned long var2UpdateInterval = 1000;

AsyncWebServer server(80);


const char* ssid = "DeviceL1";

const char* password = "";

// void handleRoot() {

//   //String message = "test theddd !";

//   // server.send(200, "text/plain", message);
// }


//by me

// void handleRoot(AsyncWebServerRequest* request) {
//   String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(var1));
//   request->send(200, "text/plain", message);
// }

void handleRoot(AsyncWebServerRequest* request) {
  if (request->method() == HTTP_GET) {
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(var1));
    request->send(200, "text/plain", message);
  } else if (request->method() == HTTP_POST) {
    String postBody = request->getParam("plain")->value();
    newVar2 = postBody.toInt();
    Serial.print("Received POST with value: ");
    Serial.println(newVar2);
    request->send(200, "text/plain", "Var2 updated");
  }
}


void setup() {

  pinMode(optoPin2, INPUT);

  pinMode(optoPinrst, INPUT);



  //String message = " 1234567";

  //server.send(200, "text/plain", message);



  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(ip);

  server.on("/", HTTP_GET, handleRoot);
  server.begin();



  // server.on("/", handleRoot);

  // server.begin();

  Serial.println("Server started");

  lcd.begin(20, 4);

  lcd.init();

  lcd.backlight();



  rtc.setTime(0, 0, 23, 12, 6, 2023);



  EEPROM.begin(sizeof(counter1) + sizeof(cycleTime));  // Initialize EEPROM with the desired data size



  // Read the counter and cycle time values from EEPROM during initialization

  EEPROM.get(counter1Address, counter1);

  EEPROM.get(cycleTimeAddress, cycleTime);



  if (counter1 < 0 || counter1 > 9999) {

    // Invalid counter1 value stored in EEPROM, reset to zero

    counter1 = 0;
  }

  //pinMode(button1Pin, INPUT_PULLUP);

  pinMode(optoPin2, INPUT_PULLUP);



  //pinMode(resetButtonPin, INPUT_PULLUP);

  pinMode(optoPinrst, INPUT_PULLUP);

  //pinMode(ledPin, OUTPUT);
}

void loop()

{
  var2 = newVar2;
  lcd.setCursor(10, 3);

  lcd.print(var2);



  // server.handleClient();

  // String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(rtc.getTime()) + "," + String(var1));



  // server.send(200, "text/plain", message);



  /*

button1State = digitalRead(button1Pin);

 if (button1State != lastButton1State)

   {

    if (button1State == LOW)

     {

      */



  optoPin2State = digitalRead(optoPin2);

  if (optoPin2State != lastoptoPin2State)

  {

    if (optoPin2State == LOW)

    {

      cycleStartTime = millis();

      //     lcd.setCursor(0, 2);

      // lcd.print(rtc.getTime());
    }

    //if (button1State == HIGH)

    if (optoPin2State == HIGH)

    {
      lcd.setCursor(10, 1);

      lcd.print("PRT1:" + String(counter1));

      cycleTime = millis() - cycleStartTime;  // Calculate the cycle time

      cycleTime = cycleTime / 1000;

      var1 = cycleTime + var1;

      counter1 = counter1 + 1;

      //     lcd.setCursor(0, 3);

      //     lcd.print(rtc.getTime());

      lcd.setCursor(0, 1);

      lcd.print("C.T.:" + String(cycleTime));
    }

    //lastButton1State = button1State;

    lastoptoPin2State = optoPin2State;
  }




  p_on_time = millis() / 60000;

  prod_time = var1 / 60;

  n_prod_time = p_on_time - prod_time;

  lcd.setCursor(0, 0);

  lcd.print(rtc.getTime());

  lcd.setCursor(10, 0);

  lcd.print(p_on_time);

  lcd.setCursor(0, 2);

  lcd.print("P.T.: " + String(prod_time));

  lcd.setCursor(10, 2);

  lcd.print("NP.T.: " + String(n_prod_time));

  lcd.setCursor(0, 3);

  lcd.print(var1);




  //if (digitalRead(resetButtonPin) == LOW) {

  if (digitalRead(optoPinrst) == LOW) {

    counter1 = 0;

    cycleTime = 0;

    lcd.setCursor(0, 1);

    lcd.print("    ");

    lcd.setCursor(0, 1);

    lcd.print("C.T.:" + String(cycleTime) + " ");

    lcd.setCursor(10, 1);

    lcd.print("   ");

    lcd.setCursor(10, 1);

    lcd.print("PRT1:" + String(counter1) + " ");



    // Store the reset counter values in EEPROM

    EEPROM.put(counter1Address, counter1);

    EEPROM.commit();
  }
}
