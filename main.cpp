/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */


#include <Arduino.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D2         // Порт для датчика
#define TEMPERATURE_PRECISION 9 // Какая-то точность (не понятно)

//идем по wire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//Массив для датчиков
DeviceAddress thermo1, thermo2, thermo3, thermo4, thermo5, thermo6;

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define WebServerImpl ESP8266WebServer
#elif ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WebServer.h>
#define WebServerImpl WebServer
#else
#error Unsupported architecture, use ARDUINO_ARCH_ESP8266 or ARDUINO_ARCH_ESP32
#endif

// Это ip адрес по которому будет доступен веб-сервер
IPAddress local_ip(192, 168,   4, 22);
// Это ip адрес шлюза, этот шлюз будут использовать wifi клиенты подключившиеся к точке доступа
IPAddress gateway (192, 168,   4,  9);
// Это маска подсети
IPAddress subnet  (255, 255, 255,  0);

// Экземпляр веб-сервера
WebServerImpl server(80);

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;
int sensor5 = 0;
int sensor6 = 0;

int in1 = D5;              // Указываем, что вывод реле In1, подключен к реле цифровому выводу 5
int in2 = D6;              // Указываем, что вывод реле In2, подключен к реле цифровому выводу 6

// Это обработчик для запроса корневого url
void handle_root() {
  // Отвечаем текстом что выполнен запрос корневого url
  server.send(200, "text/plain", "This is response for root url.");
}
void thermo(){
  Serial.begin(115200);
  while (!Serial) {
    ;                                               // Ждём'c
  }
  sensors.begin();


  Serial.print("Поиск девайсов...");
  Serial.print("Найдено ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" устройства.");

  if (!sensors.getAddress(thermo1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(thermo2, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(thermo3, 2)) Serial.println("Unable to find address for Device 2");
  if (!sensors.getAddress(thermo4, 3)) Serial.println("Unable to find address for Device 3");
  if (!sensors.getAddress(thermo5, 4)) Serial.println("Unable to find address for Device 4");
  if (!sensors.getAddress(thermo6, 5)) Serial.println("Unable to find address for Device 5");

 

  sensors.setResolution(thermo1, TEMPERATURE_PRECISION);
  sensors.setResolution(thermo2, TEMPERATURE_PRECISION);
  sensors.setResolution(thermo3, TEMPERATURE_PRECISION);
  sensors.setResolution(thermo4, TEMPERATURE_PRECISION);
  sensors.setResolution(thermo5, TEMPERATURE_PRECISION);
  sensors.setResolution(thermo6, TEMPERATURE_PRECISION);




}
void getThermo(){
  sensors.requestTemperatures();
  sensor1 = sensors.getTempC(thermo1);
  sensor2 = sensors.getTempC(thermo2);
  sensor3 = sensors.getTempC(thermo3);
  sensor4 = sensors.getTempC(thermo4);
  sensor5 = sensors.getTempC(thermo5);
  sensor6 = sensors.getTempC(thermo6);
}
void relay1(){

  digitalWrite(in1, LOW);  // Включаем реле
  digitalWrite(in2, HIGH); // Выключаем реле


}
void relay2(){
  digitalWrite(in2, LOW);  // Включаем реле
  digitalWrite(in1, HIGH); // Выключаем реле
}

void relayOff(){
  digitalWrite(in2, LOW); // Выключаем реле
  digitalWrite(in1, LOW); // Выключаем реле
}

// Это обработчик для url /my-status
void handle_my_status() {
  // Строим документ
  StaticJsonDocument<256> doc;
  // Добавляем строковый элемент с именем "some_text" и значением sensor1
  doc["thermo1"] = sensor1;
    // Добавляем строковый элемент с именем "some_text" и значением sensor2
  doc["thermo2"] = sensor2;
    // Добавляем строковый элемент с именем "some_text" и значением sensor3
  doc["thermo3"] = sensor3;
    // Добавляем строковый элемент с именем "some_text" и значением sensor4
  doc["thermo4"] = sensor4;
    // Добавляем строковый элемент с именем "some_text" и значением sensor5
  doc["thermo5"] = sensor5;
      // Добавляем строковый элемент с именем "some_text" и значением sensor6
  doc["thermo6"] = sensor6;
  // Добавляем числовой элемент с именем "my_millis" и значением полученных millis
  doc["my_millis"] = millis();
  String output;
  // Серилизуем в строку
  serializeJson(doc, output);
  // Отправляем ответ клиенту
  server.send(200, "application/json", output);
}

// Это обработчик для всех остальных запросов
void handle_not_found() {
  // Отвечаем запросившему всё что мы о нем думаем
  // и отправляем обратно то, что он прислал в запросе.
  String message = "File Not Found\n\n";
  // Добавляем uri
  message += "URI: ";
  message += server.uri();
  // Добавляем запрошенный метод
  message += "\nMethod: ";
  message += server.method() == HTTP_GET ? "GET" : "POST";
  // Если в строке запроса есть ещё аргументы, добавим их в сообщение
  if (server.args() > 0) {
    message += "\nArguments: ";
    // Указываем сколько было аргументов
    message += server.args();
    message += "\n";
    // Добавляем аргументы 'ключ: значение'
    for (uint8_t i = 0; i < server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  }
  // Отправляем ответ
  server.send(404, "text/plain", message);
}

void setup() {
  pinMode(in1, OUTPUT);    // Установим вывод 5 как выход
  pinMode(in2, OUTPUT);    // Установим вывод 6 как выход
  thermo();
  
  // Инициализируем wifi
  // Отключаем сохранение параметров во флеш
  WiFi.persistent(false);
  // Выключаем wifi совсем
  WiFi.mode(WIFI_OFF);
  // Зачем-то спим немножко
  delay(100);
  // Устанавливаем параметры ip для интерфейса AP
  WiFi.softAPConfig(local_ip, gateway, subnet);
  // Выбираем режим работы wifi
  WiFi.mode(WIFI_AP);
  // Задаем идентификатор SSID
  WiFi.softAP("MY-ESP");
 
  // Настраиваем веб-сервер
  // Устанавливаем обработчик корневого url
  server.on("/", HTTP_GET, handle_root);
  // Устанавливаем обработчик для url /my-status
  server.on("/my-status", HTTP_GET, handle_my_status);


  //Включить первое реле
  server.on("/relay1", HTTP_GET, relay1);
  //Включить второе реле
  server.on("/relay2", HTTP_GET, relay2);
  //Выключить все реле
  server.on("/relayOff", HTTP_GET, relayOff);


  // Устанавливаем обработчик для всего остального не обработанного
  server.onNotFound(handle_not_found);
  // Запускаем сервер
  server.begin();
}

void loop() {
  // Даем серверу обрабатывать запросы

  getThermo();  
  server.handleClient();
}




 

