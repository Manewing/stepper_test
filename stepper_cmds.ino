extern Stepper stepper;

struct step_cmd_args {
  int32_t* arg[2];
  const char* str;
  void (*callback)(struct step_cmd_args*); 
};

struct step_cmd {
  const char* str;
  const char* doc;
  int arg_count;
  void (*callback)(struct step_cmd_args*); 
};

void lock_break();
void unlock_break();
void __step(struct step_cmd_args* args) {
  unlock_break();
  stepper.step((uint32_t)*args->arg[0]);
  lock_break();
}

void __speed(struct step_cmd_args* args) {
  stepper.setSpeed((uint32_t)*args->arg[0] * 4);
}

void __delay(struct step_cmd_args* args) {
  delay(*args->arg[0]);
}

void __load(struct step_cmd_args* args) {
  *args->arg[0] = *args->arg[1];
}

void __add(struct step_cmd_args* args) {
  *args->arg[0] = *args->arg[0] + *args->arg[1];
}

void __sub(struct step_cmd_args* args) {
  *args->arg[0] = *args->arg[0] - *args->arg[1];
}

void __div(struct step_cmd_args* args) {
  *args->arg[0] = *args->arg[0] / *args->arg[1];
}

void __mul(struct step_cmd_args* args) {
  *args->arg[0] = *args->arg[0] * *args->arg[1];
}

void __show(struct step_cmd_args* args) {
  Serial.print("#: ");
  Serial.println(*args->arg[0]);
}


extern uint8_t pc;
extern uint8_t did_jump;
extern uint8_t arg_lbl_idx;
extern uint8_t arg_labels[8];
extern struct step_cmd_args arg_list[64];
extern int arg_list_count;

void __done(struct step_cmd_args* args);

void __until(struct step_cmd_args* args) {
  if (*args->arg[0] == *args->arg[1]) {
    while (pc < arg_list_count && arg_list[pc].callback != &__done) { pc++; }
    pc++;
    did_jump = 1;
    return;
  }

  arg_labels[arg_lbl_idx] = pc;
  arg_lbl_idx++;
}

void __done(struct step_cmd_args* args) {
  pc = arg_labels[--arg_lbl_idx];
  did_jump = 1;
}

struct step_cmd step_cmds[] = {
  { "step", "steps stepper", 1, &__step },
  { "ldr", "arg1=arg2", 2, &__load },
  { "add", "arg1+=arg2", 2, &__add },
  { "sub", "arg1-=arg2", 2, &__sub },
  { "mul", "arg1*=arg2", 2, &__mul },
  { "div", "arg1/=arg2", 2, &__div },
  { "until", "until arg1!=arg2", 2, &__until },
  { "done", "end of until", 0, __done },
  { "show", "shows register or value", 1, &__show },
  { "delay", "delay in millis", 1, &__delay },
  { "speed", "sets speed", 1, &__speed },
};
const int step_cmd_count = sizeof(step_cmds) / sizeof(step_cmds[0]);

void aval(struct str_token* tk) {
  Serial.println("commands: op *arg1 *arg2");

  int l;
  for (l = 0; l < step_cmd_count; l++) {
    Serial.print("  ");
    Serial.print(step_cmds[l].str);
    Serial.print(" - ");
    Serial.println(step_cmds[l].doc);
  }
}
