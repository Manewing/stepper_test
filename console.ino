
struct str_token {
  char* str;
  uint8_t token_count;
  uint8_t token_pos[5];
};



void help(struct str_token* tk);
void code(struct str_token* tk);
void list(struct str_token* tk);
void exec(struct str_token* tk);
void aval(struct str_token* tk);

void invalid_syntax(const char* msg);
int tokenize(struct str_token* tk, char* str);


// default strings
const char* nea_str = "not enough arguments";
const char* icf_str = "integer conversion failed";



struct command {
  const char* str;
  const char* doc;
  void (*callback)(struct str_token* tk);
};

struct command commands[] = {
  { "help", "print this screen", &help},
  { "code", "lets you enter NERD code", &code },
  { "list", "show current NERD code", &list },
  { "exec", "executes current NERD code", &exec },
  { "aval", "list available commands", &aval }
};
const int command_count = sizeof(commands) / sizeof(commands[0]);



void help(struct str_token* tk) {
  Serial.println("options:");

  int l;
  for (l = 0; l < command_count; l++) {
    Serial.print("  ");
    Serial.print(commands[l].str);
    Serial.print("  -  ");
    Serial.println(commands[l].doc);
  }
}

void console() {
  Serial.println("Stepper Control:");
  Serial.write(0x3e);
  Serial.write(0x20);
  
  while (true) {

    if (Serial.available() <= 0)
      continue;
      
    struct str_token* tk = readline();

    int l;
    for (l = 0; l < command_count; l++) {
      if (strcmp(commands[l].str, tk->str) == 0) {
        commands[l].callback(tk);
        break;
      }
    } 

    if (l >= command_count) {
      // tokenized string is still valid
      invalid_syntax(tk->str);
    }

    Serial.print("> ");
  }
}

void invalid_syntax(const char* msg) {
  Serial.print("invalid syntax: ");
  Serial.println(msg);
}
