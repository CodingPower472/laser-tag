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
const int BUZZ_HIT_TIME = 1000;
unsigned long whenBuzzAssigned = 0;
bool isBuzzAssigned = false;

const int PISTOL_DMG_MULTIPLIER = 2;

const int LED_PINS[] = { 11, 9, 6, 5, /*3*/ };
const int BLINK_LED_PIN = 3;
const int BUZZER_PIN = 13;
const int IR_RECIEVER = 2;
const int RESET_BUTTON = 12;
const int NUM_HEALTH_BARS = sizeof(LED_PINS) / sizeof(int);
const int LED_BLINKS[NUM_HEALTH_BARS] = { 4000, 4000, 4000, 3500, /*3000*/ };

std::vector<JLed> HEALTH_BAR_LEDS;
JLed BLINK_LED = JLed(BLINK_LED_PIN).Off();

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
    BLINK_LED.Update();
  }
  if (digitalRead(RESET_BUTTON) == HIGH) {
    Serial.println("Resetting");
    reset();
  }
  if (isBuzzAssigned && millis() - whenBuzzAssigned >= BUZZ_HIT_TIME) {
    isBuzzAssigned = false;
    digitalWrite(BUZZER_PIN, LOW);
  }
  if (receiver.getResults()) {
    Serial.print("Decode length: ");
    Serial.println(recvGlobal.decodeLength);
    if (recvGlobal.decodeLength == LASER_TAG_RAW_SAMPLES || recvGlobal.decodeLength == PISTOL_RAW_SAMPLES) {
      health--;
      updateLEDs();
      BLINK_LED.Blink(BLINK_HIT_TIME, BLINK_HIT_TIME);
      isBuzzAssigned = true;
      whenBuzzAssigned = millis();
      digitalWrite(BUZZER_PIN, HIGH);
    }
    if (recvGlobal.decodeLength == PISTOL_RAW_SAMPLES) {
      health -= PISTOL_DMG_MULTIPLIER + 1;
      updateLEDs();
    }
    receiver.enableIRIn();
  }
}
