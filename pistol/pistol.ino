
#include <IRremote.h>

IRsend irSender;

const int MAX_FIRING_SPEED = 120; // in shots per minute
const int MAX_AMMO = 10;
const unsigned int IR_SIGNAL[] = {1000000};
const int TRIGGER_PIN = 3;
const int RELOAD_PIN = 0;

int ammo = 0;

void setup() {
  pinMode(TRIGGER_PIN, INPUT);
  pinMode(RELOAD_PIN, INPUT);
}


void loop() {
  int triggerState = digitalRead(TRIGGER_PIN);
  if (triggerState == HIGH) {
    if (ammo >= 1) {
      ammo--;
      
      irSender.sendRaw(IR_SIGNAL, sizeof(IR_SIGNAL), 38);
      delay(60 / MAX_FIRING_SPEED * 1000);
    }
  }
  int reloadState = digitalRead(RELOAD_PIN);
  if (reloadState == HIGH) {
    ammo = MAX_AMMO;
  }
}
