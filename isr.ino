

const uint8_t PIN_BUTTON_1 = 0x2;
const uint8_t PIN_BUTTON_2 = 0x3;

const int DEBOUNCE = 200;

void (*btn1_callback)();
void (*btn2_callback)();


void setup_btn1(void (*__btn_callback)()) {
  btn1_callback = __btn_callback;
  pinMode(PIN_BUTTON_1, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_1), interrupt_btn1, RISING);
}

void interrupt_btn1() {
  
  // debounce
  static uint32_t last_millis;
  uint32_t current_millis = millis();
  
  if (current_millis - last_millis > DEBOUNCE)
    btn1_callback();

  last_millis = current_millis;
  
}




void setup_btn2(void (*__btn_callback)()) {
  btn2_callback = __btn_callback;
  pinMode(PIN_BUTTON_2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_2), interrupt_btn2, RISING);
}

void interrupt_btn2() {

  // debounce
  static uint32_t last_millis;
  uint32_t current_millis = millis();
  
  if (current_millis - last_millis > DEBOUNCE)
    btn2_callback();

  last_millis = current_millis;
  
}

