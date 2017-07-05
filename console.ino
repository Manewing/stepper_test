
struct str_token {
  char* str;
  uint8_t token_count;
  uint8_t token_pos[5];
};

void help(struct str_token* tk);
void set_step_count(struct str_token* tk);
void set_step_speed(struct str_token* tk);
void show(struct str_token* tk);
void nerd(struct str_token* tk);

struct command {
  const char* str;
  const char* doc;
  void (*callback)(struct str_token* tk);
};

struct command commands[] = {
  { "help", "print this screen", &help},
  { "ssc", "sets step count: with'-t' in turns else in single steps", &set_step_count },
  { "sss", "sets the step speed in rotations per minute", &set_step_speed },
  { "show", "shows a specific variable by name", &show },
  { "nerd", "lets you enter nerd code", &nerd}
};

const int command_count = sizeof(commands) / sizeof(commands[0]);


const char* nea_str = "not enough arguments";
const char* icf_str = "integer conversion failed";

void invalid_syntax(const char* str) {
  Serial.print("invalid syntax: ");
  Serial.println(str);
}

void help(struct str_token* tk) {
  Serial.println("options:");

  int l;
  for (l = 0; l < command_count; l++) {
    Serial.print(commands[l].str);
    Serial.print(" - ");
    Serial.println(commands[l].doc);
  }

  Serial.println("exit - exits the console");
  
}

extern uint32_t step_count;
void set_step_count(struct str_token* tk) {

  if (tk->token_count == 1) {
    invalid_syntax(nea_str);
    return;
  }

  int tk_idx = 1;
  if (tk->token_count == 3) {
    if (strcmp(tk->str + tk->token_pos[1], "-t") == 0) {
      tk_idx = 2;
    } else {
      invalid_syntax(tk->str + tk->token_pos[1]);
      return;
    }
  }
    
  char* end_ptr;
  int step_count_new = strtol(tk->str + tk->token_pos[tk_idx], &end_ptr, 10);

  if (end_ptr == NULL) {
    invalid_syntax(icf_str);
    return;
  }

  if (tk->token_count == 3) {
    Serial.print("stetting step turns to: ");
    Serial.println(step_count_new);
    step_count_new *= 800;
  }

  Serial.print("stetting step count to: ");
  Serial.println(step_count_new);

  step_count = step_count_new;
  
}

extern uint32_t step_speed;
void set_stepper_speed(uint32_t spd);

void set_step_speed(struct str_token* tk) {
  
  if (tk->token_count != 2) {
    invalid_syntax(nea_str);
    return;
  }

  char* end_ptr;
  int step_speed_new = strtol(tk->str + tk->token_pos[1], &end_ptr, 10);
  
  if (end_ptr == NULL) {
    invalid_syntax(icf_str);
    return;
  }

  Serial.print("stetting step speed to: ");
  Serial.println(step_speed_new);

  set_stepper_speed(step_speed_new);

}

void read_step_cmds();
void list_step_cmds();
void execute_step_cmds();

void nerd(struct str_token* tk) {

  if (tk->token_count == 1) {
    read_step_cmds();
  } else if (tk->token_count == 2) {
    
    if (strcmp(tk->str+tk->token_pos[1], "list") == 0)
      list_step_cmds();
    else if (strcmp(tk->str+tk->token_pos[1], "execute") == 0)
      execute_step_cmds();
      
  } else {
    invalid_syntax("too many arguments");
    return;
  }

  Serial.println("... done");
  return;
}

struct show_var {
  const char* var;
  const char* ptrn;
  uint32_t * ui_data;  
};

struct show_var show_vars[] = {
  { "step_speed", "step speed: %ld", &step_speed },
  { "step_count", "step count: %ld", &step_count }
};

const int show_var_count = sizeof(show_vars) / sizeof(show_vars[0]);

void show(struct str_token* tk) {
  if (tk->token_count == 1) {

    int l;
    Serial.println("available:");
    for (l = 0; l < show_var_count; l++) {
      Serial.print(" -> ");
      Serial.println(show_vars[l].var);
    }
    
    return;
  }

  int l;
  for (l = 0; l < show_var_count; l++) {
    if (strcmp(show_vars[l].var, tk->str+ tk->token_pos[1]) == 0) {
      char buff[64];
      sprintf(buff, show_vars[l].ptrn, *show_vars[l].ui_data);
      Serial.println(buff);
      break;
    }
  }

  if (l >= show_var_count) {
    invalid_syntax(tk->str + tk->token_pos[1]);
  }

  
  
}

int tokenize(struct str_token* tk, char* str) {
  const int len = strlen(str);

  tk->str = str;
  tk->token_count = 1;
  tk->token_pos[0] = 0;

  int l;
  for (l = 1; l < len; l++) {
    if (str[l] == ' ' || str[l] == '\n') {
      str[l] = 0;
      tk->token_pos[tk->token_count++] = l+1;

      // TODO
      if (tk->token_count >= 5)
        return 0;
    }
  }

  return tk->token_count;
}

void stop_console();

void console() {
  Serial.println("Console:");
  
  while (true) {
    
    if (Serial.available() > 0) {

      char line_buffer[64] = { 0 };
      Serial.readBytes(line_buffer, 64);
      Serial.print("> ");
      Serial.println(line_buffer);

      struct str_token tk;
      tokenize(&tk, line_buffer);

      int l;
      for (l = 0; l < command_count; l++) {
        if (strcmp(commands[l].str, line_buffer) == 0) {
          commands[l].callback(&tk);
          break;
        }
      }

      if (strcmp(line_buffer, "exit") == 0) {
        stop_console();
        return;
      }
        

      if (l >= command_count) {
        invalid_syntax(line_buffer);
      }
      
    }
    
  }
}
