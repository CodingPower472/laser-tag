#include <unwind-cxx.h>
#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <utility.h>

#include <IRLibAll.h>
#include <jled.h>
#include <vector>

const int MAX_HEALTH = 25;
int health = MAX_HEALTH;

const int LASER_TAG_RAW_SAMPLES = 18;
const int PISTOL_RAW_SAMPLES = 2;
const int DELAY_BEFORE_DEATH_STROBE = 2500;
const int DEATH_STROBE_BLINK = 600;
const int BLINK_HIT_TIME = 1000;
const int PISTOL_DMG_MULTIPLIER = 2;

const int IR_RECIEVER = 2;
const int RESET_BUTTON = 12;

const int LED_PINS[] = { 3, 5, 6, 9, 11 };
const int NUM_HEALTH_BARS = sizeof(LED_PINS) / sizeof(int);
const int LED_BLINKS[NUM_HEALTH_BARS] = { 4000, 4000, 4000, 3500, 3000 };

std::vector<JLed> HEALTH_BAR_LEDS;

IRrecvPCI receiver(IR_RECIEVER);
IRdecode decoder;

void reset() {
  for (int i = 0; i < NUM_HEALTH_BARS; i++) {
    HEALTH_BAR_LEDS[i].On();
  }
  health = MAX_HEALTH;
  receiver.enableIRIn();
}

void showDeathStrobe() {
  for (int i = 0; i < NUM_HEALTH_BARS; i++) {
    //HEALTH_BAR_LEDS[i].Blink(DEATH_STROBE_BLINK, DEATH_STROBE_BLINK).Forever();
    HEALTH_BAR_LEDS[i].Breathe(3000).DelayAfter(1000).Forever();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < NUM_HEALTH_BARS; i++) {
    int ledPin = LED_PINS[i];
    pinMode(ledPin, OUTPUT);
    HEALTH_BAR_LEDS.push_back(JLed(ledPin).On());
  }
  pinMode(RESET_BUTTON, INPUT);
  receiver.enableIRIn();
  Serial.println("IR receiver ready");
}

void updateLEDs() {
  double healthProportion = ((double) health) / ((double) MAX_HEALTH);
  Serial.print("Health proportion: ");
  Serial.println(healthProportion);
  Serial.println("Number of leds: ");
  Serial.println(NUM_HEALTH_BARS);
  if (health <= 0) {
    receiver.disableIRIn();
    for (int i = 0; i < NUM_HEALTH_BARS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }
    delay(DELAY_BEFORE_DEATH_STROBE);
    showDeathStrobe();
  } else {
    double stageRange = 1.0 / (2 * NUM_HEALTH_BARS);
    Serial.println("Each stage has length: ");
    Serial.println(stageRange);
    int stage = (int) floor(healthProportion / stageRange) - 1;
    Serial.print("Stage: ");
    Serial.println(stage);
    int numLed = stage / 2 + 1;
    Serial.print("Num led: ");
    Serial.println(numLed);
    if (numLed >= NUM_HEALTH_BARS) {
      return;
    }
    numLed = NUM_HEALTH_BARS - 1 - numLed; // invert state, because highest LED is index 0
    boolean shouldBlink = (stage % 2 == 1);
    if (stage < 0) {
      shouldBlink = true;
      numLed = NUM_HEALTH_BARS - 1;
    }
    Serial.print("Should blink: ");
    Serial.println(shouldBlink);
    if (shouldBlink) {
      HEALTH_BAR_LEDS[numLed].Breathe(4000).DelayAfter(500).Forever();
    } else {
      HEALTH_BAR_LEDS[numLed].Off();
    }
    for (int i = numLed - 1; i >= 0; i--) {
      HEALTH_BAR_LEDS[i].Off();
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < NUM_HEALTH_BARS; i++) {
    HEALTH_BAR_LEDS[i].Update();
  }
  // until i get the IR sensor, read string from serial to activate
  if (Serial.readString() != "") {
    Serial.println("Hit!");
    health--;
    updateLEDs();
    for (int i = 0; i < NUM_HEALTH_BARS; i++) {
      HEALTH_BAR_LEDS[i].Blink(BLINK_HIT_TIME, BLINK_HIT_TIME);
    }
  }
  if (digitalRead(RESET_BUTTON) == HIGH) {
    Serial.println("Resetting");
    reset();
  }
  if (receiver.getResults()) {
    Serial.print("Decode length: ");
    Serial.println(recvGlobal.decodeLength);
    if (recvGlobal.decodeLength == LASER_TAG_RAW_SAMPLES) {
      health--;
      updateLEDs();
    } else if (recvGlobal.decodeLength == PISTOL_RAW_SAMPLES) {
      health -= PISTOL_DMG_MULTIPLIER;
    }
    receiver.enableIRIn();
  }
}
