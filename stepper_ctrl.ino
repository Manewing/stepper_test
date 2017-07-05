
extern Stepper stepper;


struct step_cmd_args {
  uint32_t* var;
  uint32_t* value;
  const char* str;
  void (*callback)(struct step_cmd_args*); 
};

void __step_stepper(struct step_cmd_args* args) {
  if (args->var)
    stepper.step(*args->var);
  else if(args->value)
    stepper.step(*args->value);
}

void __speed(struct step_cmd_args* args) {
  if (args->var)
    stepper.setSpeed(*args->var * 4);
  else if(args->value)
    stepper.setSpeed(*args->value * 4);
}

void __delay(struct step_cmd_args* args) {
  if (args->var)
    delay(*args->var);
  else if(args->value)
    delay(*args->value);
}

void __load(struct step_cmd_args* args) {
  if (args->var == NULL || args->value == NULL) {
    Serial.println("nullptr");
    return; //TODO
  }

  *args->var = *args->value;
}

void __test(struct step_cmd_args* args) {
  Serial.println("test:");
  if (args->var)
    Serial.println(*args->var);
  if (args->value)
    Serial.println(*args->value);
}

struct step_cmd {
  const char* str;
  int arg_count;
  void (*callback)(struct step_cmd_args*); 
};


uint32_t x = 1;
uint32_t y = 2;

uint32_t c = 3;

struct step_cmd_args arg_list[64];
uint32_t arg_values[64];
int arg_list_idx;
int arg_list_count;


struct step_cmd step_cmds[] = {
  { "step", 1, &__step_stepper },
  { "load", 2, &__load },
  { "test", 1, &__test },
  { "delay", 1, &__delay },
  { "speed", 1, &__speed }
};

const int step_cmd_count = sizeof(step_cmds) / sizeof(step_cmds[0]);


int tokenize(struct str_token* tk, char* str);
void invalid_syntax(const char* str);
  
int write_args(struct str_token* tk, int idx) {
  arg_list[idx].var = NULL;
  arg_list[idx].value = NULL;
  
  if (!tk->token_count)
    return 0;
    
  if (tk->token_count >= 2) {

    int l;
    for (l = 1; l < tk->token_count; l++) {
      if ((tk->str+tk->token_pos[l])[0] == 'x') {
        arg_list[idx].var = &x;
      } else if ((tk->str+tk->token_pos[l])[0] == 'y') {
        arg_list[idx].var = &y;
      } else if ((tk->str+tk->token_pos[l])[0] == 'c') {
        arg_list[idx].var = &c;
      } else {
        
        char* end_ptr;
        arg_values[idx] = strtol(tk->str + tk->token_pos[l], &end_ptr, 10);
        arg_list[idx].value = &arg_values[idx];
        
        if (end_ptr == NULL) {
          invalid_syntax(icf_str);
          return -1;
        }
      }
    }
    
    return l - 1;
  }

  return -1;
}

void read_step_cmds() {

  Serial.println("pls code:");

  arg_list_idx = 0;
  arg_list_count = 0;
  
  while (true) {

    if (Serial.available() <= 0)
      continue;
    
    char line_buffer[64] = { 0 };
    Serial.readBytes(line_buffer, 64);
    Serial.println(line_buffer);
    struct str_token tk;
    tokenize(&tk, line_buffer);
    
    int l;
    for (l = 0; l < step_cmd_count; l++) {
      if (strcmp(tk.str, step_cmds[l].str) == 0) {
        arg_list[arg_list_count].callback = step_cmds[l].callback;
        arg_list[arg_list_count].str = step_cmds[l].str;
        
        int ret = write_args(&tk, arg_list_count);
        if (ret != step_cmds[l].arg_count) {
          arg_list_idx = 0;
          arg_list_count = 0;
          invalid_syntax("todo");
          return;
        }
        
        arg_list_count++;
        break;
      }
    }

    if (strcmp(tk.str, "end") == 0)
      return;

    if (l >= step_cmd_count) {
      invalid_syntax(tk.str);
      arg_list_idx = 0;
      arg_list_count = 0;
      return;
    }
      
  }
}

void list_step_cmds() {
  int l;
  Serial.print("arguments: ");
  Serial.println(arg_list_count);
  
  for (l = 0; l < arg_list_count; l++) {
    Serial.print("  ");
    Serial.println(arg_list[l].str); 
  }
}

void execute_step_cmds() {
  int idx = 0;
  
  while (idx < arg_list_count) {
    struct step_cmd_args* args = &arg_list[idx];

    args->callback(args);
    
    idx++;
  }
}



