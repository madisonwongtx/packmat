void userKeyCheck(){
  if (key && key != 'A' && key != 'B' && key != 'C' && key != 'D' &&key != '*' && key != '#') {
    if(numPresses == 0){
      numPresses += 1;
      prev_press = key - '0';
      sprintf(user, "%d",key - '0');
    }
    else if(prev_press == key - '0'){
      numPresses += 1;
      if(prev_press == 2){
        if(numPresses == 2){
          *user = 'A';
        }
        else if (numPresses == 3){
          *user = 'B';
        }
        else if(numPresses == 4){
          *user = 'C';
        }
      }
      else if(prev_press == 3){
        if(numPresses == 2){
          *user = 'D';
        }
        else if (numPresses == 3){
          *user = 'E';
        }
        else if(numPresses == 4){
          *user = 'F';
        }
      }
      else if(prev_press == 4){
        if(numPresses == 2){
          *user = 'G';
        }
        else if (numPresses == 3){
          *user = 'H';
        }
        else if(numPresses == 4){
          *user = 'I';
        }
      }
      else if(prev_press == 5){
        if(numPresses == 2){
          *user = 'J';
        }
        else if (numPresses == 3){
          *user = 'K';
        }
        else if(numPresses == 4){
          *user = 'L';
        }
      }
      else if(prev_press == 6){
        if(numPresses == 2){
          *user = 'M';
        }
        else if (numPresses == 3){
          *user = 'N';
        }
        else if(numPresses == 4){
          *user = 'O';
        }
      }
      else if(prev_press == 7){
        if(numPresses == 2){
          *user = 'P';
        }
        else if (numPresses == 3){
          *user = 'Q';
        }
        else if(numPresses == 4){
          *user = 'R';
        }
        else if(numPresses == 5){
          *user = 'S';
        }
      }
      else if(prev_press == 8){
        if(numPresses == 2){
          *user = 'T';
        }
        else if (numPresses == 3){
          *user = 'U';
        }
        else if(numPresses == 4){
          *user = 'V';
        }
      }
      else if(prev_press == 9){
        if(numPresses == 2){
          *user = 'W';
        }
        else if (numPresses == 3){
          *user = 'X';
        }
        else if(numPresses == 4){
          *user = 'Y';
        }
        else if(numPresses == 5){
          *user = 'Z';
        }
      }
    }
    Serial.println(username);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0);
    tft.println(username);
  }
}

void passwordKeyCheck(){
  if (key && key != 'A' && key != 'B' && key != 'C' && key != 'D' &&key != '*' && key != '#') {
    if(numPresses == 0){
      numPresses += 1;
      prev_press = key - '0';
      sprintf(pwd, "%d",key - '0');
    }
    else if(prev_press == key - '0'){
      numPresses += 1;
      if(prev_press == 2){
        if(numPresses == 2){
          *pwd = 'A';
        }
        else if (numPresses == 3){
          *pwd = 'B';
        }
        else if(numPresses == 4){
          *pwd = 'C';
        }
      }
      else if(prev_press == 3){
        if(numPresses == 2){
          *pwd = 'D';
        }
        else if (numPresses == 3){
          *pwd = 'E';
        }
        else if(numPresses == 4){
          *pwd = 'F';
        }
      }
      else if(prev_press == 4){
        if(numPresses == 2){
          *pwd = 'G';
        }
        else if (numPresses == 3){
          *pwd = 'H';
        }
        else if(numPresses == 4){
          *pwd = 'I';
        }
      }
      else if(prev_press == 5){
        if(numPresses == 2){
          *pwd = 'J';
        }
        else if (numPresses == 3){
          *pwd = 'K';
        }
        else if(numPresses == 4){
          *pwd = 'L';
        }
      }
      else if(prev_press == 6){
        if(numPresses == 2){
          *pwd = 'M';
        }
        else if (numPresses == 3){
          *pwd = 'N';
        }
        else if(numPresses == 4){
          *pwd = 'O';
        }
      }
      else if(prev_press == 7){
        if(numPresses == 2){
          *pwd = 'P';
        }
        else if (numPresses == 3){
          *pwd = 'Q';
        }
        else if(numPresses == 4){
          *pwd = 'R';
        }
        else if(numPresses == 5){
          *pwd = 'S';
        }
      }
      else if(prev_press == 8){
        if(numPresses == 2){
          *pwd = 'T';
        }
        else if (numPresses == 3){
          *pwd = 'U';
        }
        else if(numPresses == 4){
          *pwd = 'V';
        }
      }
      else if(prev_press == 9){
        if(numPresses == 2){
          *pwd = 'W';
        }
        else if (numPresses == 3){
          *pwd = 'X';
        }
        else if(numPresses == 4){
          *pwd = 'Y';
        }
        else if(numPresses == 5){
          *pwd = 'Z';
        }
      }
    }
    Serial.println(loginPassword);
    tft.setCursor(0,0);
    tft.fillScreen(TFT_BLACK);
    tft.println(loginPassword);
  }
}

bool comparePasscodes(){
  bool correct = true;
    for(int i = 0;i < KEY_LENGTH; i++){
      Serial.println(passcode[i]);
      if(passcode[i] != correctPasscode[i]){
        correct = false;
      }
    }
  return correct;
}