
// registers
struct __attribute__((packed)) {
  int32_t x;
  int32_t y;
  int32_t c;
} regs;

uint8_t pc;
uint8_t did_jump;

int reg_idx(int32_t* ptr) {
  int diff = (uint8_t*)ptr - (uint8_t*)&regs;
  return diff / sizeof(int32_t);
}

bool is_reg(int32_t* ptr) {
  int diff = (uint8_t*)ptr - (uint8_t*)&regs;
  if (diff >= 0 && diff <= (sizeof(regs) - sizeof(int32_t)))
    return true;
  return false;
}

char var_names[] = { 'x', 'y', 'c' };

// program instructions
struct step_cmd_args arg_list[64];

// program data
int32_t arg_values[64];

// program labels
uint8_t arg_labels[8];
uint8_t arg_lbl_idx;

int arg_list_count;

int tokenize(struct str_token* tk, char* str);
void invalid_syntax(const char* str);
  
int write_args(struct str_token* tk, int idx) {
  arg_list[idx].arg[0] = NULL;
  arg_list[idx].arg[1] = NULL;
  
  if (!tk->token_count)
    return 0;

  if (tk->token_count > 3)
    return -1;
    
  int l;
  for (l = 1; l < tk->token_count; l++) {
    if ((tk->str+tk->token_pos[l])[0] == 'x') {
      arg_list[idx].arg[l-1] = &regs.x;
    } else if ((tk->str+tk->token_pos[l])[0] == 'y') {
      arg_list[idx].arg[l-1] = &regs.y;
    } else if ((tk->str+tk->token_pos[l])[0] == 'c') {
      arg_list[idx].arg[l-1] = &regs.c;
    } else {
      
      char* end_ptr;
      arg_values[idx] = strtol(tk->str + tk->token_pos[l], &end_ptr, 10);
      arg_list[idx].arg[l-1] = &arg_values[idx];
      
      if (end_ptr == NULL) {
        invalid_syntax(icf_str);
        return -1;
      }
    }
  }
  
  return l - 1;
}

void reset_prg() {
  pc = 0;
  did_jump = 0;
  arg_list_count = 0;
  arg_lbl_idx = 0;
}

void code(struct str_token* tk) {

  // reset program
  reset_prg();

  Serial.print("... ");
  while (true) {
    
    if (Serial.available() <= 0)
      continue;
      
    struct str_token* tk = readline();
    
    int l;
    for (l = 0; l < step_cmd_count; l++) {
      if (strcmp(tk->str, step_cmds[l].str) == 0) {
        arg_list[arg_list_count].callback = step_cmds[l].callback;
        arg_list[arg_list_count].str = step_cmds[l].str;
        
        int ret = write_args(tk, arg_list_count);
        if (ret != step_cmds[l].arg_count) {
          
          // reset program
          reset_prg();
          
          invalid_syntax("invalid argument count");
          return;
        }
        
        arg_list_count++;
        break;
      }
    }

    if (strcmp(tk->str, "end") == 0)
      return;

    if (l >= step_cmd_count) {

      invalid_syntax(tk->str);
      
      // reset program
      reset_prg();
      
      return;
    }

    Serial.print("... ");
  }
}

void list(struct str_token* tk) {
  int l, m;
  Serial.print("instructions: ");
  Serial.println(arg_list_count);
  
  for (l = 0; l < arg_list_count; l++) {
    Serial.print("... ");
    Serial.print(arg_list[l].str);


    for (m = 0; m < 2; m++) {
      if (!arg_list[l].arg[m])
        continue;

      Serial.print(" ");
      
      if (is_reg(arg_list[l].arg[m])) {
        Serial.print(var_names[reg_idx(arg_list[l].arg[m])]);
      } else {
        Serial.print(*arg_list[l].arg[m]);
      }
      
    }
    
    Serial.print('\n');
  }
}

void exec(struct str_token* tk) {
  pc = 0;
  did_jump = 0;
  
  while (pc < arg_list_count) {
    struct step_cmd_args* args = &arg_list[pc];

    args->callback(args);
    
    if (did_jump)
      did_jump = 0;
    else
      pc++;
  }
}



