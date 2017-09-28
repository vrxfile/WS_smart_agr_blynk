#define BLYNK_PRINT Serial

// Библиотеки
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Blynk auth token
char auth[] = "af690efbe2a3460290e42cd5ef9873ae";
IPAddress blynk_ip(139, 59, 206, 133);

// Датчик DS18B20
#define DS18B20_1 3
OneWire oneWire1(DS18B20_1);
DallasTemperature ds_sensor1(&oneWire1);

// Датчик DHT11
#define DHTPIN 2
DHT dht1(DHTPIN, DHT11);

// Период для таймера обновления данных
#define UPDATE_TIMER 1000

// Таймер
BlynkTimer timer_update;

// Выход управления помпой через реле
#define PUMP 4

// Вход датчика света
#define LIGHT_SENSOR A0

// Вход датчика влажности почвы
#define SOIL_SENSOR A1

void setup() {
  // Инициализация UART
  Serial.begin(115200);

  // Подключение к серверу Blynk
  Blynk.begin(auth, blynk_ip, 8442);

  // Инициализация датчика DHT11
  dht1.begin();

  // Инициализация датчика DS18B20
  ds_sensor1.begin();

  // Инициализация выхода реле
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);

  // Инициализация таймера
  timer_update.setInterval(UPDATE_TIMER, readSendData);
}

void loop() {
  Blynk.run();
  timer_update.run();
}

// Считывание датчиков и отправка данных на сервер Blynk
void readSendData() {
  float air_hum = dht1.readHumidity();                // Считывание влажности воздуха
  float air_temp = dht1.readTemperature();            // Считывание температуры воздуха
  Serial.print("Air temperature = ");
  Serial.println(air_temp);
  Serial.print("Air humidity = ");
  Serial.println(air_hum);
  Blynk.virtualWrite(V0, air_temp); delay(25);        // Отправка данных на сервер Blynk
  Blynk.virtualWrite(V1, air_hum); delay(25);         // Отправка данных на сервер Blynk

  ds_sensor1.requestTemperatures();                   // Считывание температуры почвы
  float soil_temp = ds_sensor1.getTempCByIndex(0);
  Serial.print("Soil temperature = ");
  Serial.println(soil_temp);
  Blynk.virtualWrite(V2, soil_temp); delay(25);       // Отправка данных на сервер Blynk

  float light = analogRead(LIGHT_SENSOR) / 1023.0 * 100.0;    // Считывание датчика света
  float soil_hum = analogRead(SOIL_SENSOR) / 1023.0 * 100.0;  // Считывание влажности почвы
  Serial.print("Soil moisture = ");
  Serial.println(soil_hum);
  Serial.print("Light = ");
  Serial.println(light);
  Blynk.virtualWrite(V4, light); delay(25);          // Отправка данных на сервер Blynk
  Blynk.virtualWrite(V3, soil_hum); delay(25);       // Отправка данных на сервер Blynk
}

// Управление помпой с Blynk
BLYNK_WRITE(V5)
{
  int pump_ctl = param.asInt();   // Получение управляющего сигнала с сервера
  Serial.print("Pump power: ");
  Serial.println(pump_ctl);
  digitalWrite(PUMP, pump_ctl);   // Управление реле (помпой)
}

