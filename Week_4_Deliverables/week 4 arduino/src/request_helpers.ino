uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

/*----------------------------------
 * do_http_request Function:
 * Arguments:
 *    const char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_request(const char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}       

//OWN HELPERS
void login(){
  Serial.println("start login");
  sprintf(body, "username=");
  strcat(body, username);
  strcat(body, "&password=");
  strcat(body,loginPassword);
  Serial.println(body);
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/yechengz/packmat2/login.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n"); //new line from header to body
  strcat(request_buffer, body); //body
  strcat(request_buffer, "\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response_buffer); //viewable in Serial Terminal
  const int capacity = 300;
  StaticJsonDocument<capacity> doc;
  deserializeJson(doc,response_buffer);
  const char * mess = doc["message"];
  sprintf(message, "%s", mess);
  char check[] = "incorrect password or username";
  Serial.println(message);
  if (strcmp(message, check) != 0) {
    const char * id = doc["user_id"];
    sprintf(user_id, "%s", id);
    Serial.println(user_id);
  }
}

void create_account(){
  Serial.println("start create account");
  sprintf(body, "username=");
  strcat(body, username);
  strcat(body, "&password=");
  strcat(body,loginPassword);
  Serial.println(body);
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/yechengz/packmat2/create_account.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n"); //new line from header to body
  strcat(request_buffer, body); //body
  strcat(request_buffer, "\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response_buffer); //viewable in Serial Terminal
}

void postUpdate(int alarm_status, int is_active){
  Serial.println("start post update");
  sprintf(body, "passcode=");
  char currPasscode[2];
  for(int i = 0;i < KEY_LENGTH; i++){
    sprintf(currPasscode,"%d",correctPasscode[i]);
    strcat(body,currPasscode);
  }
  if(alarm_status == 1){
    if (is_active == 1) {
      strcat(body, "&alarm_status=1&is_active=1");
    } else {
      strcat(body, "&alarm_status=1&is_active=0");
    }
  }
  else if(alarm_status == 0){
    if (is_active == 1) {
      strcat(body, "&alarm_status=0&is_active=1");
    } else {
      strcat(body, "&alarm_status=0&is_active=0");
    }
  }
  strcat(body, "&user_id=");
  strcat(body, user_id);
  Serial.println(body);
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n"); //new line from header to body
  strcat(request_buffer, body); //body
  strcat(request_buffer, "\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response_buffer); //viewable in Serial Terminal
}

int checkWebsiteAlarmStatus(){
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py?user_id=%s HTTP/1.1\r\n", user_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  //Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  //Serial.println(response_buffer); //viewable in Serial Terminal
  const int capacity = 300;
  StaticJsonDocument<capacity> doc;
  deserializeJson(doc,response_buffer);
  int alarm_status = doc["alarm_status"];
  return alarm_status;
}