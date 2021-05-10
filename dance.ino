#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h> 

// 아래의 6개설정은 사용자 환경에 맞게 수정하세요.
const char* ssid = "RainDrop"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* password = "asdfasdfaa";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char *thingId = "3.17.203.242";          // 사물 이름 (thing ID) mac address로 자동 생성
const char *host = "a10x5fsnw5ocjg-ats.iot.us-east-2.amazonaws.com"; // AWS IoT Core 주소
const char* outTopic = "/khj/inTopic"; // 이름이 중복되지 않게 설정 기록
const char* inTopic = "/khj/outTopic"; // 이름이 중복되지 않게 설정 기록
const char* clientName = "";  // setup 함수에서 자동생성
String sChipID;
char cChipID[20];


// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUYnAJYvsTMWraZ1CdeQt23V7jD74wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDQwODA3NDU0
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANSrPBR6KYzBXlpOiB63
lEZnfLGZb0QS6KE7VKjONZvhbsuqacnmtd8W1bScE2fzAMkNaES8akxfXLOyk9fa
i7c8zyUb/t4zd9vyd9ldhFVvfCXzSx+kCm2wL0bepa95GvB/roxXANgs2uYMHEkE
OE3kJasfn55c+93jz98KGk/D4qEN7LRPggSwzgPuJMBOjUVWQHflfa8S0E5pCkzC
8TgReW0zXUq5i3jlyzZ4pC5b0CC4qGd4qQp+2GPtaUmZXtuV0pDAxSK1apQIpsLF
twrwAVmR5xTfHL1sOyzgcQ3kyXnzVAnlRBNR1kaMJX2hGdT19EqUpVnJmOonwy98
uUsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUF6TsnrwldLl8Ma2CMYFUQQ8DuoswHQYD
VR0OBBYEFKZiV5MHprflHfVa1XmjXDN+077RMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAy4fhbD3Tg6Q/CZLljapXsjeJ0
AlHrrUWWLEmaaqZJg21Y/ado7NZPAT9ygO2c+kIY3pmwLvJLa/D5HEpoLXcEfSe+
Hjh0+oF2Xpt8hVBqzXQgNN6zucImEP+fdvkhhyHyzLj8JzLvXcmLmNPWQhbZF4Fl
83gixWG3G1g+LBj8XOF7rup6M5jiB4vELGcp8nkhADvpGSLGQVIvIGu5E0c8ykOh
3Ds64mIhPSXoNg5C7F24Za7j7MUVQ9GNm9ewjFUn3suQA4+0OASPasQY6QM63m0+
LhsFbyFdBiWoxudb4nK9HJm8fOhcPL9IT5u29VAzR0q/vPP5nEkia2vBRaz8
-----END CERTIFICATE-----

)EOF";
// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA1Ks8FHopjMFeWk6IHreURmd8sZlvRBLooTtUqM41m+Fuy6pp
yea13xbVtJwTZ/MAyQ1oRLxqTF9cs7KT19qLtzzPJRv+3jN32/J32V2EVW98JfNL
H6QKbbAvRt6lr3ka8H+ujFcA2Cza5gwcSQQ4TeQlqx+fnlz73ePP3woaT8PioQ3s
tE+CBLDOA+4kwE6NRVZAd+V9rxLQTmkKTMLxOBF5bTNdSrmLeOXLNnikLlvQILio
Z3ipCn7YY+1pSZle25XSkMDFIrVqlAimwsW3CvABWZHnFN8cvWw7LOBxDeTJefNU
CeVEE1HWRowlfaEZ1PX0SpSlWcmY6ifDL3y5SwIDAQABAoIBACBoc8BfGWPUt88l
NoMP0K2l6Y+a5STHiOHBC9apjevSP7MTpglZeg+kCC78tcxacJ0yyNfoqn427gaI
SRVUvYPP5XUvPfiYeURwP6lFIfxu/8k8uQ76TYiHl7zCAxepadyKaIwEWiGXPEhg
QhMybG4aPU9Fg2p7mvomVDAV9wp+dTQhlPje+Bj9zipRBkcjo0OC3FmoI2YbVjSi
Xrn9tULMPkcmP9cVPRtzqWYeVHGeRdncAHGLxtcQDPfMdwqgCsSMMFqNSPSB8dBW
zQaXexrHRQgo583CJc7e/1CzRDaUA6x4CEWvMqRneea6ZRUMEFtvNpdyWFts1wU6
5mtBXDECgYEA7RlAF5gI3LBUTDsl6xcvHU3B0JIjOogBj8ZDwW9Tj1968DESjYg0
sgEAcZrsf+HPNOXNttm1anHXjeWkcnlT5qiAr1UOSpEPYe4UrPV7yEKVmPjoQnvd
qF+iKiU7BlFqkvnlEYhtzprkuAbHZGpnMlwEI+vNggZREK/KE04fNPMCgYEA5Z9q
9ktDarUG8F0DyCbu7cWKV1PXaIubqfzxIx+b87zqsDaNXnO4V4p8aTlT1iuD4Qa7
aWAhNNWJEYo4E2sEgS6FXOOhXkz6vmGZTSbDAbgQ/64aQEcIFD2s/lnWWmlran9y
sqIV94tKEc/LbHlLWaFnrOB1X2qc19figJU24EkCgYAdHayxOQpdj4nKkrjlVcOD
ZbkXU9rfpTQL490ONA2e3arLNDuDBebytQzI5q2ERsabSF0qxzFwEOrXkgvPwvZ6
rNagsWE/5vBspkADvWGmBgc48dgT0gI2WwgfBNNqwzTDN8zZildxt7O+pZZ2zlo9
5O7dOmfSP9RUqEG4fS934QKBgQCSnquW3tIKddMbcdkzWaDg1G0Ue+WkzNg1v9xd
mBZlmTCmGf9E21FPHI96RO/CZoDMFBdhHs+1Q9BFEePiikgTEJh5iqQEadUF8Z4I
xIJqO5UnHY2Hk5OWERfARa74hi3fj8e2RocOTLOcEfoLppu1fLmNirmHlrTVofTw
M9cZeQKBgD6jyIUH7inPtd1pJOKxW7hIu6MIAKyIEMobu4TKh4KboUIOkAuNQ0U5
6/6TxDX8Lz1GSYvSMx31MI32qUyLWNqkfm8qSaekNeMVQ3pbSQxdNJBX0sH/EU1v
xEID+6SZroX6ZPN/fOsdlERj9m/whOqE9fDQc7suVii60cbXjOv+
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

long lastMsg = 0;
char msg[50];
Servo servoLeft1;
Servo servoLeft2;
Servo servoRight1;
Servo servoRight2;

//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

int Duty,mode=0; //imode 0=초기치 1,2,3,4=서보모터 회전 5=동작속도 9=DoArray
int data[100],volume;  //volume : data[]의 개수
int iTime=500;  // 로봇 동작속도

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
    int i,j,k;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  int premode=mode;
  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  mode = root["mode"];
  Duty = root["duty"];
  if(mode==0)
    InitSet();
  else if(mode==1)
    servoLeft1.write(Duty);
  else if(mode==2)
    servoLeft2.write(Duty);
  else if(mode==3)
    servoRight1.write(Duty);
  else if(mode==4)
    servoRight2.write(Duty);
  else if(mode==5) {
    iTime = root["valocity"];
    mode=premode;
  }
  else if(mode==9) { 
      volume = root["volume"]; 
      Serial.println(volume);
      for(i=0; i<volume;i++)
        data[i]= root["data"][i];     
    Serial.println("=======");
    for(i=0;i<volume;i++)
      Serial.println(data[i]);
  }
  else
    Serial.print("No choice");
  Serial.print(mode);
}

X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  delay(1000);

  //이름 자동으로 생성
  sChipID=String(ESP.getChipId(),HEX);
  sChipID.toCharArray(cChipID,sChipID.length()+1);
  clientName=&cChipID[0];
  Serial.println(clientName);

  client.setServer(host, 8883);
  client.setCallback(callback);

  servoLeft1.attach(5);  // attaches the servo on GIO5 to the servo object 
  servoLeft2.attach(4);  
  servoRight1.attach(0); 
  servoRight2.attach(2); 
}

void setup_wifi() {
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

  wifiClient.setTrustAnchors(&ca);
  wifiClient.setClientRSACert(&cert, &key);
  Serial.println("Certifications and key are set");

  setClock();
  //client.setServer(host, 8883);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Reconnected");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wifiClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void DoArray() {
  int i,j,k;
  k=volume/4;
  for(i=0 ; i<volume ; i++) {
    j=i%4;
    Serial.println(j);
    Serial.println(data[i]);
    if(j==0)
       servoLeft1.write(data[i]);
    else if(j==1)
       servoLeft2.write(data[i]);
    else if(j==2)
       servoRight1.write(data[i]);
    else if(j==3)
       servoRight2.write(data[i]);

    delay(iTime);
  }
}

void InitSet() {
  servoLeft1.write(90);
  delay(iTime);
  servoLeft2.write(90);
  delay(iTime);
  servoRight1.write(90);
  delay(iTime);  
  servoRight2.write(90);
  delay(iTime);   
}

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(mode==9)
    DoArray();
}
