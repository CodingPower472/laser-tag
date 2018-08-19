#include <IRremote.h>

IRsend irsend;

int trigger = 3;
int reload = 0;
int maxAmmo = 10;
int ammo = 0;

void setup() {
  pinMode(trigger, INPUT);
  pinMode(reload, INPUT);
}


void loop() {
  int trigger_state = digitalRead(trigger);
  if (trigger_state == 1) {\
    if (ammo >= 1) {
      ammo = ammo - 1;
      
      unsigned int irSignal[] = {1000000};
      irsend.sendRaw(irSignal, sizeof(irSignal), 38);
      delay(500);
    }
  }
  int reload_state = digitalRead(reload);
  if (reload_state == 1) {
    ammo = maxAmmo;
  }
}
