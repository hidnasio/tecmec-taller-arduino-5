#define LED BUILTIN_LED

void setup() {
  pinMode(LED, OUTPUT);  
}

void loop() {
  if(digitalRead(LED) == 0) {
    digitalWrite(LED, 1);
  } else {
    digitalWrite(LED, 0);
  }

  delay(1000);
}
