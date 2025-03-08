#include <OneWire.h>
#include <PWM.h>

OneWire ds(5);
int thermistorPin = A0;
int temperature = 87;
long lastUpdateTime = 0;
const int TEMP_UPDATE_TIME = 1000;
const int buttonPin = 7;
int A_C = 0;
#define FANrelay 9
byte FANspeed = 0;
byte lastFANspeed = FANspeed;
long fanOnTime = 0;
const long FAN_RUN_TIME = 10000; // 10 секунд

void setup() {
  Serial.begin(9600); // Инициализация монитора порта с коммуникацией на скорости 9600 бит/с
  InitTimersSafe();
  SetPinFrequencySafe(FANrelay, 10);
  pwmWrite(FANrelay, 223);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
}

void loop() {
  A_C = digitalRead(buttonPin);  
 //detectTemperature();
  
  // Отправка информации о температуре и ШИМ в монитор порта
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("C\t");
  Serial.print("FANspeed: ");
  Serial.print(FANspeed);
  Serial.println("%");

  if (temperature > 80 && temperature <= 90) {
    FANspeed = map(temperature, 86, 100, 10, 90);
     fanOnTime = millis(); // Обновляем время включения вентилятора
  } else if (temperature > 100) {
    FANspeed = 100;
    fanOnTime = millis(); // Обновляем время включения вентилятора
  }
 
   
  if (temperature <= 85 && A_C != LOW) {
   
    FANspeed = 3;
    
  }
  
  if (A_C == LOW && FANspeed < 50) {
    FANspeed = 50;
  }
  
  if (lastFANspeed != FANspeed) {
    byte pwm = map(FANspeed, 0, 100, 0, 90);
    pwm = constrain(pwm, 0, 90);
    pwmWrite(FANrelay, 230 - (pwm * 230 / 100));
    lastFANspeed = FANspeed;
  }
}

void detectTemperature() {
  if (millis() - lastUpdateTime > TEMP_UPDATE_TIME) {
    lastUpdateTime = millis();
    int raw = analogRead(thermistorPin); // Считываем аналоговое значение с термистора
    float resistance = 4700.0 / (1023.0 / raw - 1); // Преобразуем в сопротивление, используя делитель напряжения с 4.7 кОм резистором
    temperature = int(1.0 / (log(resistance / 10000.0) / 3435.0 + 1 / 298.15) - 273.15); // Преобразуем в температуру
   // temperature = temperature + 45;
    }
}
/*
void detectTemperature() {
  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
  if (millis() - lastUpdateTime > TEMP_UPDATE_TIME) {
    lastUpdateTime = millis();
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    data[0] = ds.read();
    data[1] = ds.read();
    temperature = (data[1] << 8) + data[0];
    temperature = temperature >> 4;
  }
}
*/
