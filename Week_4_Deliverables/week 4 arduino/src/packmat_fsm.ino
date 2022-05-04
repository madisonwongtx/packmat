//1: entering numbers/unlock mode, 2: pressure increase, 3: pressure decrease/yes/correct, 4: no/incorrect
void packmat(int input1, int input2, int input3, int input4){
  key = keypad.getKey();
  switch(state){
    case PM1:
      if(update){ //so only prints once
        Serial.println("In PM1 State"); 
        setPasscode(tft);
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("First number entered");
        Serial.println("Switching to Program Mode 2");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        correctPasscode[numCoded + 1] = '\0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        state = PM2;
        firstNum(tft, key);
      }
      break;

    case PM2:
      if(update){ //so only prints once
        Serial.println("In PM2 State"); 
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Second number entered");
        Serial.println("Switching to Program Mode 3");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        correctPasscode[numCoded + 1] = '\0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        state = PM3;
        secondNum(tft, key);
      }
      break;

    case PM3:
      if(update){ //so only prints once
        Serial.println("In PM3 State"); 
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Third number entered");
        Serial.println("Switching to Program Mode 4");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        correctPasscode[numCoded + 1] = '\0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        state = PM4;
        thirdNum(tft, key);
      }
      break;

    case PM4:
      if(update){ //so only prints once
        Serial.println("In PM4 State"); 
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Fourth number entered");
        Serial.println("Switching to Locked");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        correctPasscode[numCoded + 1] = '\0';
        Serial.println(correctPasscode[numCoded]);
        postUpdate(0, 1);
        numCoded = 0;
        update = 1;
        state = REST;
        fourthNum(tft, key);
        display = millis();
        while(millis()-display < 3000); //so can see fully entered code
      }
      break;
    case USERNAME:
      if(update){ //so only prints once
        Serial.println("In USERNAME State"); 
        logInUser(tft);
        update = 0;
      }
      userKeyCheck();
      if(!input3 && millis()-timer >= 1000){
        user++;
        numPresses = 0;
        timer = millis();
      }
      if(!input4 && millis()-timer2 >= 1000){
        update = 1;
        user++;
        *user = '\0';
        state = PASSWORD;
        timer2 = millis();
      }
      break;
    case PASSWORD:
      if(update){ //so only prints once
        Serial.println("In PASSWORD State"); 
        logInPass(tft);
        update = 0;
      }
      passwordKeyCheck();
      if(!input3 && millis()-timer >= 1000){
        pwd++;
        numPresses =0;
        timer = millis();
      }
      if(!input4 && millis()-timer2 >= 1000){
        update = 1;
        pwd++;
        *pwd = '\0';
        state = LOGIN;
        timer2 = millis();
      }
      break;
    case LOGIN:
      if(update){
        create_account();
        login();
        postUpdate(0, 0);
        state = PM1;
      }
      break;
    case REST:
      ledcWriteTone(AUDIO_PWM,0);
      numCoded = 0;
      if(update){ //so only prints once
        Serial.println("In Rest State"); 
        unarmed(tft);
        update = 0;
        postUpdate(0, 0);
      }
      if(curr_weight - old_weight>THRESHOLD){//pressure increase
        Serial.println("Pressure increase detected.");
        Serial.println("Switching to package confirmation 1 state");
        update = 1;
        state = PC1;
      }
      break;
    
    case PC1:
      if(update){ //so only prints once
        Serial.println("In PC1 State"); 
        packageConfirmation(tft);
        update = 0;
      }
      if(!input3){//yes
        Serial.println("Package Confirmed");
        Serial.println("Switching to Program Mode 1");
        update = 1;
        state = LOCKED;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Rest");
        update = 1;
        state = REST;
      }
      break;

    case LOCKED:
      if(update){ //so only prints once
        Serial.println("In LOCKED State"); 
        locked(tft);
        postUpdate(0, 1);
        numCoded = 0;
        update = 0;
      }
      if((curr_weight - old_weight)>THRESHOLD){//pressure increase
        Serial.println("Pressure Increase Detected");
        Serial.println("Swithcing to Package Confirmation 2");
        update = 1;
        state = PC2;
      }else if(old_weight - curr_weight>THRESHOLD){ //pressure decrease
        Serial.println("Pressure Decrease Detected");
        Serial.println("Switching to Alarm");
        update = 1;
        state = ALARM;
      }else if(!input4){//unlock mode
        Serial.println("Entering Unlock mode");
        Serial.println("Switching to unlock mode 1");
        update = 1;
        state = UM1;
      }
      break;
    
    case PC2:
      if(update){ //so only prints once
        Serial.println("In PC2 State"); 
        packageConfirmation(tft);
        update = 0;
      }
      if(!input3){//yes
        Serial.println("Package Confirmed");
        Serial.println("Switching to LOCKED");
        update = 1;
        state = LOCKED;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Alarm");
        alarmStart = millis();
        alarm_on = 1;
        postUpdate(1, 1);
        playTone();
        update = 1;
        state = ALARM;
        alarm(tft);
        display = millis();
        while(millis()-display < 3000); //so can see fully entered code
      }
      break;

    case ALARM:
      playTone();
      numCoded = 0;
      if(update){ //so only prints once
        Serial.println("In ALARM State"); 
        unlockAlarm(tft);
        postUpdate(1, 1);
        alarm_on=1;
        update = 0;
      }
      else{
        state = AS1;
        update = 1;
      }
      break;
    
    case AS1:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 1 State"); 
        unlockAlarm(tft);
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("First number entered");
        Serial.println("Switching to Alarm Check 1");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = AC1;
        firstNum(tft, key);
      }
      
      break;

    case AC1:

      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 1 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(passcode[0] == correctPasscode[0]){ //correct
        Serial.println("Correct first number");
        Serial.println("Switching to Alarm Stop 2");
        update = 1;
        state = AS2;
      }else if(passcode[0] != correctPasscode[0]){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to Alarm");
        update = 1;
        state = ALARM;
      }
      break;

    case AS2:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 2 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Second number entered");
        Serial.println("Switching to Alarm Check 2");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = AC2;
        secondNum(tft, key);
      }
      break;

    case AC2:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 2 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(passcode[1] == correctPasscode[1]){ //correct
        Serial.println("Correct second number");
        Serial.println("Switching to AS3");
        update = 1;
        state = AS3;
      }else if(passcode[1] != correctPasscode[1]){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to Alarm");
        update = 1;
        state = ALARM;
      }
      break;

    case AS3:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 3 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Third number entered");
        Serial.println("Switching to Alarm Check 3");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = AC3;
        thirdNum(tft, key);
      }
      break;

    case AC3:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 3 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(passcode[2] == correctPasscode[2]){ //correct
        Serial.println("Correct third number");
        Serial.println("Switching to AS4");
        update = 1;
        state = AS4;
      }else if(passcode[2] == correctPasscode[2]){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to Alarm");
        update = 1;
        state = ALARM;
      }
      break;

    case AS4:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 4 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Fourth number entered");
        Serial.println("Switching to Alarm Check 4");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded = 0;
        update = 1;
        state = AC4;
        fourthNum(tft, key);
      }
      break;

    case AC4:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 4 State"); 
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          update = 1;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(passcode[3] == correctPasscode[3]){ //correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        update = 1;
        alarm_on = 0;
        state = REST;
      }else if(passcode[3] != correctPasscode[3]){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to Alarm");
        update = 1;
        state = ALARM;
      }
      break;
    
    case UM1:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 1 State"); 
        unlock(tft);
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("First number entered");
        Serial.println("Switching to Unlock Check 1");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = UC1;
        firstNum(tft,key);
      }
      break;
    
    case UC1:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 1 State"); 
        update = 0;
      }
      if(passcode[0] == correctPasscode[0]){//correct
        Serial.println("Correct first number");
        Serial.println("Switching to Unlock Mode 2");
        update = 1;
        state = UM2;
      }else if(passcode[0] != correctPasscode[0]){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to LOCKED");
        update = 1;
        state = LOCKED;
      }
      break;

    case UM2:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 2 State"); 
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Second number entered");
        Serial.println("Switching to Unlock Check 2");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = UC2;
        secondNum(tft, key);
      }
      break;
    
    case UC2:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 2 State"); 
        update = 0;
      }
      if(passcode[1] == correctPasscode[1]){//correct
        Serial.println("Correct second number");
        Serial.println("Switching to Unlock Mode 3");
        update = 1;
        state = UM3;
      }else if(passcode[1] != correctPasscode[1]){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to LOCKED");
        update = 1;
        state = LOCKED;
      }
      break;

    case UM3:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 3 State"); 
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Third number entered");
        Serial.println("Switching to Unlock Check 3");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded += 1;
        update = 1;
        state = UC3;
        thirdNum(tft, key);
      }
      break;
    
    case UC3:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 3 State"); 
        update = 0;
      }
      if(passcode[2] == correctPasscode[2]){//correct
        Serial.println("Correct third number");
        Serial.println("Switching to Unlock Mode 4");
        update = 1;
        state = UM4;
      }else if(passcode[2] != correctPasscode[2]){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to LOCKED");
        update = 1;
        state = LOCKED;
      }
      break;

    case UM4:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 4 State"); 
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Fourth number entered");
        Serial.println("Switching to Unlock Check 4");
        passcode[numCoded] = key-'0';
        passcode[numCoded + 1] = '\0';
        numCoded = 0;
        update = 1;
        state = UC4;
        fourthNum(tft, key);
      }
      break;
    
    case UC4:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 4 State"); 
        update = 0;
      }
      if(passcode[3] == correctPasscode[3]){//correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        update = 1;
        state = REST;
      }else if(passcode[3] != correctPasscode[3]){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to LOCKED");
        update = 1;
        state = LOCKED;
      }
      break;
  }
}