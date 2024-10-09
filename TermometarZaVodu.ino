/*
Code for thermometer
v0.3 using:
- 16x2 LCD + IC2
- DS18B20 thermometer
- On/OFF button
- On/Off led diode


pins:
12 -> DS180B20 Data
11 -> On/OFF button
10 -> On/Off led diode
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>


#define THERMO_DATA 12

#define ON_OFF_BTN 11

#define ON_OFF_LED 10

#define BUZZER 8


//Degree Character
#define CELSIUS "°"  //Ne radi iz nekog razloga

byte deg[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

//IC2 address
#define ic2Addr 0x27

//Setup thermometer
OneWire oneWire(THERMO_DATA);
DallasTemperature sensors(&oneWire);

//Setup LCD
LiquidCrystal_I2C lcd(ic2Addr, 16, 2);

//Setup on-off
bool isOn = false;
unsigned long debounceDuration = 50;
unsigned long lastTimeButtonChanged = 0;
byte lastButtonState = LOW;

//Setup buzzer
bool buzzing = false;
unsigned long buzzingTimeStart = 0;
bool ended = true;


unsigned long currentMillis = 0;

void setup() {
  //Disable L diode
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //Set button to input mode and diode to output mode
  pinMode(ON_OFF_BTN, INPUT);
  pinMode(ON_OFF_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  //Setup thermometer
  sensors.begin();

  //Setup LCD
  lcd.init();
  lcd.clear();         
  lcd.createChar(0, deg);
  lcd.createChar(1, heart);

  //Setup console
  Serial.begin(9600);
}

void loop() {
  if (millis() - lastTimeButtonChanged >= debounceDuration) {
    byte buttonState = digitalRead(ON_OFF_BTN);
    if (buttonState != lastButtonState) {
      lastTimeButtonChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        isOn = !isOn;
        byte ledState = isOn ? HIGH : LOW;
        if (isOn) {
          lcd.display();
          lcd.backlight();
        }
        else {
          lcd.clear();
          lcd.noDisplay();
          lcd.noBacklight();
          
          digitalWrite(BUZZER, LOW);
          buzzing = false;
          ended = false;
          buzzingTimeStart = 0;
        }
        digitalWrite(ON_OFF_LED, ledState);
      }
    }
  }

  if (isOn) {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    
    //lcd.print(sensors.getTempCByIndex(0));
    /*lcd.print(temperature);
    lcd.write(0);
    lcd.print("C");
    lcd.setCursor(15, 0);
    lcd.write(1);
    lcd.setCursor(0, 0);*/

    if (temperature >= 85.0 && !buzzing) {
      buzzing = true;
      ended = false;
      buzzingTimeStart = millis();
      digitalWrite(BUZZER, HIGH);
    }

    if (buzzing && !ended && millis() - buzzingTimeStart > 5000) {
      digitalWrite(BUZZER, LOW);
      ended = true;
    }

    //Serial.print(sensors.getTempCByIndex(0));
    //Serial.print(CELSIUS);
    //Serial.println("C");
    

    //delay(debounceDuration);
    //better delay
    currentMillis = millis();
    while (millis() < currentMillis + debounceDuration) {}
  }
}

/*
future plans:
- bell
- button to disable bell
*/
