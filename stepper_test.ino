#include <Stepper.h>

const uint8_t PIN_DIR = 0x6;
const uint8_t PIN_PUL = 0x7;

#define STEPPER_STEPS   200L
#define STEPPER_TURN    STEPPER_STEPS*4
#define STEPPER_SPS(x)  ((x) * 240L)
#define STEPPER_SPM(x)  ((x) *   4L)

Stepper stepper(STEPPER_STEPS, 8, 9);


// speed in rotations per minute
uint32_t step_speed = 100;

void set_stepper_speed(uint32_t spd) {
  step_speed = spd;
  stepper.setSpeed(STEPPER_SPM(spd));
}

// step count per period
uint32_t step_count  = STEPPER_TURN;

void switch_dir() {
  Serial.println("button 1 pending served");
  step_count = -step_count;
}

/*
void next_step_count() {
  step_count <<= 1L;
  if (step_count > STEPPER_TURN << 4L)
    step_count = STEPPER_TURN >> 3L;
}*/

int console_active = 0;

void start_console() {
  Serial.println("starting console...");
  console_active = 1;  
}

void stop_console() {
  Serial.println("stopping console...");
  console_active = 0;
}

void console();

void sleep(int times) {
  for (; times >= 0; times--)
    asm volatile("nop");
}

void interrupt_btn1();
void interrupt_btn2();

void setup() {
  Serial.begin(9600);
  Serial.println("-.- Stepper Control -.-");
  

  set_stepper_speed(100);

  
  setup_btn1(&switch_dir);
  setup_btn2(&start_console);
}

void loop() {

  stepper.step(step_count);

  delay(3000);

  if (console_active) {
    console();
  }

}
