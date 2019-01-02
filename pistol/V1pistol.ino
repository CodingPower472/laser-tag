int IRledPin = 1;
int TriggerPin = 0;

void setup() {
  pinMode(IRledPin, OUTPUT);
  pinMode(TriggerPin, INPUT);
}

void loop() {
  int val = digitalRead(TriggerPin);
  if (val == HIGH) {
    SendIRCode();
    delay(100);
  }
}

void IR(long microsecs) {

  cli();

  while (microsecs > 0) {
    digitalWrite(IRledPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(IRledPin, LOW);
    delayMicroseconds(10);
    microsecs -= 26;
  }

  sei();
}

void SendIRCode() {
  IR(2874);
  delayMicroseconds(6086);
  IR(2898);
  delayMicroseconds(2098);
  IR(882);
  delayMicroseconds(2102);
  IR(898);
  delayMicroseconds(2090);
  IR(902);
  delayMicroseconds(2090);
  IR(902);
  delayMicroseconds(2090);
  IR(898);
  delayMicroseconds(2094);
  IR(898);
  delayMicroseconds(2098);
  IR(878);
  delayMicroseconds(1000);
}
