int S = A0; // Analog Input
int d1=D1;
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(d1, OUTPUT);
 pinMode(S, INPUT);
}

void loop()
{
  int sensor=analogRead(S);
  
  if(sensor>10){
    digitalWrite(d1,LOW);
}
  else if(sensor<=10){
    digitalWrite(d1,HIGH);
  }
 Serial.println(sensor);
 delay(1000);
}
