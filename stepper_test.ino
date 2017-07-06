#include <Stepper.h>

const uint8_t PIN_BRK = 0x2;
const uint8_t PIN_DIR = 0x6;
const uint8_t PIN_PUL = 0x7;

#define STEPPER_STEPS   200L
#define STEPPER_TURN    STEPPER_STEPS*4
#define STEPPER_SPS(x)  ((x) * 240L)
#define STEPPER_SPM(x)  ((x) *   4L)

Stepper stepper(STEPPER_STEPS, 8, 9);

void lock_break() {
  digitalWrite(PIN_BRK, HIGH);
}

void unlock_break() {
  digitalWrite(PIN_BRK, LOW);
}

void console();

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BRK, OUTPUT);
  lock_break();
}

void loop() {
  console();
}
