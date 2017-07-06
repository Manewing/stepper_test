
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

struct str_token* readline() {
  static char line_buffer[64];
  static struct str_token tk;
  
  int bytes = Serial.readBytes(line_buffer, 64);
  line_buffer[bytes] = 0;

  // tokenize
  tokenize(&tk, line_buffer);

  return &tk;
}

