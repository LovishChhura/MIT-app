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

//my variables
String currentRtcTime;
int rtaddress = 10;
const int maxStringLength = 50;
unsigned long sec = 0;
unsigned long min1 = 0;
unsigned long hr = 0;
int rtcHourAddress = +sizeof(int);
int rtcMinuteAddress = rtcHourAddress + sizeof(int);
int rtcSecondAddress = rtcMinuteAddress + sizeof(int);
int storedHour, storedMinute, storedSecond;
int Ttimead = rtcSecondAddress + sizeof(int);
int Ctimeadd = Ttimead + sizeof(int);
int Prt1add = Ctimeadd + sizeof(int);
int PTadd = Prt1add + sizeof(int);
int NPTadd = PTadd + sizeof(int);
int var1add = NPTadd + sizeof(int);
int counter1add = var1add + sizeof(int);
unsigned long p_on_timeval, prod_timeval, n_prod_timeval, cycleTimeval, var1val, counter1val;
int milliFlag = 0;
unsigned long resetcycleStartTime = 0;
String myvar2;
int getsec, getmin, gethr;
unsigned long p_o = 0;
int p_oadd = counter1add + sizeof(int);
unsigned long p_min = 0;
int p_minadd = p_oadd + sizeof(int);

//modes
unsigned long mode = 1;
int mtime = 0, setime = 0;
unsigned long mcycleStartTime = 0, scycleStartTime = 0;
unsigned long mcycleTime = 0, scycleTime = 0;
int mcycleadd = p_minadd + sizeof(int), scycleadd = mcycleadd + sizeof(int);

AsyncWebServer server(80);


const char* ssid = "DeviceL1";

const char* password = "";



void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void resetparams() {
  counter1 = 0;

  cycleTime = 0;

  /*my*/
  counter1val = 0;
  cycleTime = 0;
  var1 = 0;
  var1val = 0;
  resetcycleStartTime = millis();
  milliFlag = 1;
  p_o = 0;
  p_min = 0;

  lcd.setCursor(0, 1);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("C.T.:" + String(cycleTime) + "  ");

  lcd.setCursor(10, 1);
  lcd.print("   ");

  lcd.setCursor(10, 1);
  lcd.print("PRT1:" + String(counter1) + "  ");

  rtc.setTime(0, 0, 23, 12, 6, 2023);

  lcd.setCursor(10, 0);
  lcd.print(String(p_on_time) + " ");

  lcd.setCursor(0, 2);
  lcd.print("P.T.: " + String(prod_time) + " ");

  lcd.setCursor(10, 2);
  lcd.print("NP.T.: " + String(n_prod_time) + " ");

  lcd.setCursor(0, 3);
  lcd.print(String(var1) + " ");

  lcd.setCursor(15, 0);
  lcd.print(String(p_o) + " ");

  lcd.setCursor(17, 0);
  lcd.print(String(p_min) + " ");
  // Store the reset counter values in EEPROM

  EEPROM.put(counter1Address, counter1);

  EEPROM.commit();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  pinMode(optoPin2, INPUT);

  pinMode(optoPinrst, INPUT);



  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(ip);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(var1) + "," + String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()) + "," + String(p_o) + "," + String(p_min));
    request->send(200, "text/plain", message);
  });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* j = request->getParam(0);
    String postBody = j->value();

    if (postBody == "reset") {
      AsyncWebParameter* k = request->getParam(1);
      getsec = (k->value()).toInt();
      AsyncWebParameter* l = request->getParam(2);
      getmin = (l->value()).toInt();
      AsyncWebParameter* m = request->getParam(3);
      gethr = (m->value()).toInt();
      rtc.setTime(getsec, getmin, (gethr - 1), 12, 6, 2023);
      AsyncWebParameter* n = request->getParam(4);
      p_o = (n->value()).toInt();
      AsyncWebParameter* o = request->getParam(5);
      p_min = (o->value()).toInt();
      request->send(200, "text/plain", "time reset successful");
      lcd.setCursor(12, 3);

      // lcd.print(String(gethr) + ":" + String(getmin) + ":" + String(getsec));
      lcd.print(String(p_min));
    } else {
      request->send(400, "text/plain", "invalid action");
    }



    request->send(200);
  });




  server.onNotFound(notFound);

  server.begin();



  // server.on("/", handleRoot);

  // server.begin();

  Serial.println("Server started");

  lcd.begin(20, 4);

  lcd.init();

  lcd.backlight();



  rtc.setTime(0, 0, 23, 12, 6, 2023);



  // EEPROM.begin(sizeof(counter1) + sizeof(cycleTime));  // Initialize EEPROM with the desired data size
  EEPROM.begin(512);



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

  storedHour = int(EEPROM.read(rtcHourAddress));
  storedMinute = int(EEPROM.read(rtcMinuteAddress));
  storedSecond = int(EEPROM.read(rtcSecondAddress));
  rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);
  myvar2 = String(storedHour) + ":" + String(storedMinute) + ":" + String(storedSecond);
  lcd.setCursor(3, 3);
  lcd.print(myvar2);

  p_on_timeval = EEPROM.read(Ttimead);
  prod_timeval = EEPROM.read(PTadd);
  n_prod_timeval = EEPROM.read(NPTadd);
  cycleTimeval = EEPROM.read(Ctimeadd);
  var1val = EEPROM.read(var1add);
  counter1val = EEPROM.read(counter1add);
  p_o = EEPROM.read(p_oadd);
  p_min = EEPROM.read(p_min);
  mcycleTime = EEPROM.read(mcycleadd);
  scycleTime = EEPROM.read(scycleadd);

  if (counter1val <= 0) {
    counter1 = 0;
    counter1val = 0;
  } else {
    counter1 = counter1val - 1;
  }
  if (var1val <= 0) {
    var1 = 0;
    var1val = 0;
  } else {
    var1 = var1val - 2;
  }
  if (cycleTimeval <= 0) {
    cycleTime = 0;
    cycleTimeval = 0;
  } else {
    cycleTime = cycleTimeval - 2;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void loop()

{
  if (mode == 1) {
    lcd.setCursor(15, 0);

    lcd.print(p_o);

    lcd.setCursor(17, 0);

    lcd.print(p_min);

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
      }

      //if (button1State == HIGH)

      if (optoPin2State == HIGH)

      {
        lcd.setCursor(10, 1);

        lcd.print("PRT1:" + String(counter1));

        // if (milliFlag == 1) {

        //   milliFlg = 0;
        // } else {

        //   p_on_time = millis() / 60000;
        // }

        cycleTime = millis() - cycleStartTime;  // Calculate the cycle time

        cycleTime = (cycleTime / 1000);
        cycleTimeval = cycleTime;

        var1 = cycleTime + var1;
        var1val = var1;

        counter1 = counter1 + 1;
        counter1val = counter1;


        lcd.setCursor(0, 1);

        lcd.print("C.T.:" + String(cycleTime));
      }

      //lastButton1State = button1State;

      lastoptoPin2State = optoPin2State;
    }




    // p_on_time = millis() / 60000;
    p_on_time = rtc.getMinute() + (int(rtc.getHour()) * 60);

    if (var1 == 0) {
      prod_time = 0;
    } else {
      prod_time = var1 / 60;
    }



    n_prod_time = p_on_time - prod_time;

    lcd.setCursor(0, 0);

    lcd.print(rtc.getTime());

    lcd.setCursor(10, 0);

    lcd.print(p_on_time);

    lcd.setCursor(0, 2);
    cv
      lcd.print("P.T.: " + String(prod_time));

    lcd.setCursor(10, 2);

    lcd.print("NP.T.: " + String(n_prod_time));

    lcd.setCursor(0, 3);

    lcd.print(var1);

    currentRtcTime = rtc.getTime();
    sec = rtc.getSecond();
    min1 = rtc.getMinute();
    hr = rtc.getHour();

    // myvar = 75;
    // myvaraddress = 10 + sizeof(int);
    // storedHour += hr;
    // storedMinute += min1;
    // storedSecond += sec;

    EEPROM.write(rtcHourAddress, hr);
    EEPROM.write(rtcMinuteAddress, min1);
    EEPROM.write(rtcSecondAddress, sec);

    EEPROM.write(Ttimead, p_on_timeval);
    EEPROM.write(PTadd, prod_timeval);
    EEPROM.write(NPTadd, n_prod_timeval);
    EEPROM.write(Ctimeadd, cycleTimeval);
    EEPROM.write(var1add, var1val);
    EEPROM.write(counter1add, counter1val);
    EEPROM.write(p_oadd, p_o);
    EEPROM.write(p_minadd, p_min);

    EEPROM.commit();

    if (int(rtc.getHour()) >= 12) {
      resetparams();
    }


    //if (digitalRead(resetButtonPin) == LOW) {

    if (digitalRead(optoPinrst) == LOW) {

      resetparams();
    }

    else if (mode == 2) {
      //Maintence mode
      mcycleStartTime = millis();
      mcycleTime = (millis() - mcycleStartTime) / 60000;
      EEPROM.write(mcycleadd, mcycleTime);

    } else if (mode == 3) {
      //setting mode
      scycleStartTime = millis();
      scycleTime = (millis() - scycleStartTime) / 60000;
      EEPROM.write(scycleadd, scycleTime);
    } else {
      mode = 1;
    }
  }
}