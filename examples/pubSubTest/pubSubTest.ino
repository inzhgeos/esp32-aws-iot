#include <AWS_IOT.h>
#include <WiFi.h>
//#include <HardwareSerial.h>

// подключает файл с WiFi и AWS настройками.
#include "config.h"

// подключение fromArduinoSerial1 для приемки данных с Arduino
//HardwareSerial fromArduinoSerial1(1);

// создание объекта класса AWS_IOT - класс соединения с сервером
AWS_IOT hornbill;

int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}



void setup() {
  // установление Serial соединения с компьютером
  Serial.begin(115200);
  //fromArduinoSerial1.begin(115200, SERIAL_8N1, 4, 2);   //Baud rate, parity mode, RX, TX

  // установление WiFi соединения с компьютером
  while (status != WL_CONNECTED)
  {
    
    if (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(WIFI_SSID_2);
      // соединение с WPA/WPA2 сетью. Нужно изменить если подключаешься к открытой или WEP сети:
      status = WiFi.begin(WIFI_SSID_2, WIFI_PASSWORD_2);
      // ждем 5 секунд установки соединения:
      delay(5000);
      if (status == WL_CONNECTED) { Serial.print("Connected to wifi "); Serial.println(WIFI_SSID_2);}
    }
    if (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(WIFI_SSID_3);
      // соединение с WPA/WPA2 сетью. Нужно изменить если подключаешься к открытой или WEP сети:
      status = WiFi.begin(WIFI_SSID_3, WIFI_PASSWORD_3);
      // ждем 5 секунд установки соединения:
      delay(5000);
      if (status == WL_CONNECTED) { Serial.print("Connected to wifi "); Serial.println(WIFI_SSID_3);}
    }
    if (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(WIFI_SSID_1);
      // соединение с WPA/WPA2 сетью. Нужно изменить если подключаешься к открытой или WEP сети:
      status = WiFi.begin(WIFI_SSID_1, WIFI_PASSWORD_1);
      // ждем 5 секунд установки соединения:
      delay(5000);
      if (status == WL_CONNECTED) { Serial.print("Connected to wifi "); Serial.println(WIFI_SSID_1);}
    }
  }


  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID,
                       aws_root_ca_pem, certificate_pem_crt, private_pem_key) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);

    if (0 == hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler))
    {
      Serial.println("Subscribe Successfull");
    }
    else
    {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while (1);
    }
  }
  else
  {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }

  delay(2000);

}

void loop() {

  if (msgReceived == 1)
  {
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.println(rcvdPayload);
  }

  /*
    //Данные пришедшие в fromArduinoSerial1 выводим на компьютер через Serial и отправляем в AWS IOT
    if (Serial1.available()) {
    String temp = String(Serial1.read());
    //sprintf(payload,temp);
    Serial.println(temp);
    temp.toCharArray(payload, 512);
    if (hornbill.publish(TOPIC_NAME,payload) == 0)
    {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Publish failed");
    }
    }*/
  if (tick >= 5)  // публикует данные в топик каждые 5 секунд
  {
    tick = 0;
    // сборка строки "payload" с данными для отправки
    sprintf(payload, "Hello from hornbill ESP32 : %d", msgCount++);
    if (hornbill.publish(TOPIC_NAME, payload) == 0)
    {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Publish failed");
    }
  }
  vTaskDelay(1000 / portTICK_RATE_MS);
  tick++;
}
