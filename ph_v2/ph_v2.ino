//Import Library
#include <WiFi.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

//Variabel untuk Wifi
const char* ssid = "";
const char* password = "";

//IP Address MQTT Broker 
const char* mqtt_server = "";

//Variabel untuk MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//Variabel untuk sistem
const int trigPin = 15;
const int trigPin1 = 5;
const int trigPin2= 19;
const int echoPin = 4;
const int echoPin1 = 18;
const int echoPin2 = 21;
const int potPin = 34;
const int relayasam = 26;
const int relaybasa = 27;
float ph;
float Value=0;
float sisaair;
float sisaAsam;
float sisaBasa;
int tinggiKolam;
int tinggiAsam;
int tinggiBasa;
int luasKolam;
int volume;
float xbasa[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float xasam[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Yasam[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
int Ybasa[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
float larasam;
float larbasa;
int delayasam;
int delaybasa;
float sumasam;
float sumbasa;
float avga = 0;
float avgb = 0;
long duration;
long duration1;
long duration2;
float distanceCm;
float distanceInch;
float distanceCm1;
float distanceCm2;
char phValue[10];
char sisaAsamValue[10];
char sisaBasaValue[10];
char avgAsamValue[10];
char avgBasaValue[10];

#define SOUND_SPEED 0.034


void setup(){
  Serial.begin(115200); //Mengatur baud rate

  //Mengatur input dan output pin
  pinMode(potPin,INPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(relayasam, OUTPUT);
  pinMode(relaybasa, OUTPUT);

  //Mematikan relay pada tahap awal
  digitalWrite(relayasam, LOW);
  digitalWrite(relaybasa, LOW);

  setup_wifi(); //Menghubungkan ke Wifi
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi(){
  delay(10);
  //Mulai hubungkan ke jaringan WiFi
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
}

bool isConvertibleToInt(String str) {
    int intValue = str.toInt();
    
    // Check if the conversion was successful and if there's no remainder
    return intValue != 0 || str == "0";
}

void callback(char* topic, byte* message, unsigned int length) {
  //Untuk monitor di serial
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //Untuk control dari dashboard
  if(isConvertibleToInt(messageTemp)){
    if (String(topic) == "esp32/tinggiKolam") {
      Serial.print("Mengubah tinggi kolam menjadi ");
      Serial.println(messageTemp);
      tinggiKolam = messageTemp.toInt() * 100;
    }
    else if (String(topic) == "esp32/luasKolam") {
      Serial.print("Mengubah luas kolam menjadi ");
      Serial.println(messageTemp);
      luasKolam = messageTemp.toInt() * 100;
    }
    else if (String(topic) == "esp32/tinggiAsam") {
      Serial.print("Mengubah tinggi wadah larutan asam menjadi ");
      Serial.println(messageTemp);
      tinggiAsam = messageTemp.toInt();
    }
    else if (String(topic) == "esp32/tinggiBasa") {
      Serial.print("Mengubah tinggi wadah larutan basa menjadi ");
      Serial.println(messageTemp);
      tinggiBasa = messageTemp.toInt();
    }
  }
}

void reconnect() {
  // Loop hingga terhubung
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", "pi", "raspberry")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/tinggiKolam");
      client.subscribe("esp32/luasKolam");
      client.subscribe("esp32/tinggiAsam");
      client.subscribe("esp32/tinggiBasa");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Tunggu 5 detik sebelum mencoba lagi
      delay(5000);
    }
  }
}

void loop(){
  //Mematikan relay pada tahap awal
  digitalWrite (relayasam, LOW);
  digitalWrite (relaybasa, LOW);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    Value = analogRead(potPin);
    Serial.print(Value);
    Serial.print(" | ");
    float voltage=Value*(3.3/4095.0);
    ph= 14 - (3.3*voltage-2.4);
    Serial.println(ph);
    delay(500);

    
    dtostrf(ph, 2, 2, phValue);
    dtostrf(sisaAsam, 2, 2, sisaAsamValue);
    dtostrf(sisaBasa, 2, 2, sisaBasaValue);
    dtostrf(avga, 2, 2, avgAsamValue);
    dtostrf(avgb, 2, 2, avgBasaValue);

    client.publish("esp32/phKolam", phValue); //Update pH Kolam
    client.publish("esp32/sisaAsam", sisaAsamValue); //Update Sisa Asam
    client.publish("esp32/sisaBasa", sisaBasaValue); //Update Sisa Basa
    client.publish("esp32/avgAsam", avgAsamValue); //Update Avg Asam
    client.publish("esp32/avgBasa", avgBasaValue); //Update Avg Basa

    // Sensor Ultrasonik Kolam
    // Mengatur ulang trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Mengaktifkan trigPin selama 10ms
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Membaca echoPin (ms)
    duration = pulseIn(echoPin, HIGH);
    
    // Menghitung jarak
    distanceCm = duration * SOUND_SPEED/2;
    
    // Mencetak jarak pada Serial Monitor
    Serial.print("Distance Kolam (cm): ");
    Serial.println(distanceCm); // jarak sensor ke permukaan kolam
    sisaair = tinggiKolam - distanceCm + 50; //+50 akomodasi posisi sensor
    delay(500);

    // Sensor Ultrasonik Asam
    // Mengatur ulang trigPin1
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    // Mengaktifkan trigPin1 selama 10ms
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    
    // Membaca echoPin1 (ms)
    duration1 = pulseIn(echoPin1, HIGH);
    
    // Menghitung jarak
    distanceCm1 = duration1 * SOUND_SPEED/2;
    
    // Mencetak jarak pada Serial Monitor
    Serial.print("Distance Larutan Asam (cm): ");
    Serial.println(distanceCm1);  
    sisaAsam =  (tinggiAsam - distanceCm1);
    delay(500);
    
    //Sensor Ultrasonik Basa
    // Mengatur ulang trigPin2
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    // Mengaktifkan trigPin2 selama 10ms
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    
    // Membaca echoPin2 (ms)
    duration2 = pulseIn(echoPin2, HIGH);
    
    // Menghitung jarak
    distanceCm2 = duration2 * SOUND_SPEED/2;
    
    // Mencetak jarak pada Serial Monitor
    Serial.print("Distance Larutan Basa (cm): ");
    Serial.println(distanceCm2);  
    sisaBasa = (tinggiBasa - distanceCm2);
    delay(500);

    volume = luasKolam * tinggiKolam;

    if (ph > 7.5 && sisaAsam > 0){
      sumasam = 0;
      Serial.println("Kondisi terlalu basa");
      larasam = -((pow(10,-ph)-pow(10,-7.4))*1000*volume/1.37);
      Serial.print("Penambahan asam: ");
      Serial.println(larasam);
      Serial.print("Delay asam: ");
      delayasam = floor(larasam*36);
      Serial.println(delayasam);
      for (int i = 0; i <= 29; i++) {
        Yasam[i] = Yasam[i] + 1;    
      }
      for (int i = 0; i <= 28; i++) {
        xasam[i] = xasam[i+1];
        sumasam = sumasam + xasam[i];
      }
      xasam[29] = larasam;
      sumasam = sumasam + larasam;
      larasam = 0;
      
      digitalWrite(relayasam, HIGH);
      delay(1000);
      digitalWrite(relayasam, LOW);
      delay(100);
    }

    else if (ph < 6.5 && sisaBasa > 0){
      sumbasa = 0;
      Serial.println("Kondisi terlalu asam"); 
      larbasa = (pow(10,-ph)-pow(10,-6.6))*1000*volume/1.37;  
      Serial.print("Penambahan basa: ");
      Serial.println(larbasa);
      Serial.print("Delay basa: ");
      delaybasa = floor(larbasa*36);
      Serial.println(delaybasa);
      for (int i = 0; i <= 29; i++) {
        Ybasa[i] = Ybasa[i] + 1;
        }
      for (int i = 0; i <= 28; i++) {
        xbasa[i] = xbasa[i+1];
        sumbasa = sumbasa + xbasa[i];
        }
      xbasa[29] = larbasa;
      sumbasa = sumbasa + larbasa;
      larbasa = 0;
      digitalWrite(relaybasa, HIGH);
      delay(1000);
      digitalWrite(relaybasa, LOW);
      delay(100);
      }

    avga = sumasam/30;
    avgb = sumbasa/30;
  }
}