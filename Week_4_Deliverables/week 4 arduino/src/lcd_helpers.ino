void resetScreen(TFT_eSPI tft){
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
}

void welcome(TFT_eSPI tft){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(20,10);
  tft.setTextSize(2);
  tft.println("Welcome");
  tft.setCursor(50, 60);
  tft.println("to");
  tft.setCursor(20, 120);
  tft.println("PackMat!");
  
}

void setPasscode(TFT_eSPI tft){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Set");
  tft.setCursor(20, 60);
  tft.println("Passcode");
  tft.setTextSize(1);
  tft.setCursor(30, 100);
  tft.println("Please Enter");
  tft.setCursor(40, 120);
  tft.println("4 Digits");
}

void firstNum(TFT_eSPI tft, char key){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(10, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void secondNum(TFT_eSPI tft, char key){
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(40, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void thirdNum(TFT_eSPI tft, char key){
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(70, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void fourthNum(TFT_eSPI tft, char key){
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(100, 50);
  tft.setTextSize(4);
  tft.println(key);
  tft.setTextSize(1);
  tft.setCursor(30, 20);
  tft.println("Entered Code:");
}

void unarmed(TFT_eSPI tft){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(35,10);
  tft.setTextSize(2);
  tft.println("Please");
  tft.setCursor(40, 40);
  tft.println("place");
  tft.setCursor(20, 70);
  tft.println("packages");
  tft.setCursor(20, 100);
  tft.println("on mat :)");
}

void packageConfirmation(TFT_eSPI tft){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(10,10);
  tft.setTextSize(2);
  tft.println("Confirm");
  tft.setCursor(10, 35);
  tft.println("Package");
  tft.setCursor(25, 100);
  tft.println("Yes");
  tft.setCursor(80, 100);
  tft.println("No");
}

void locked(TFT_eSPI tft){
  resetScreen(tft);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextSize(3);
  tft.setCursor(10, 60);
  tft.println("LOCKED");
  tft.setCursor(20, 120);
  tft.setTextSize(1);
  tft.println("Press Button 1");
  tft.setCursor(20, 135);
  tft.println("to unlock");
}

void alarm(TFT_eSPI tft){
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setCursor(20,10);
  tft.setTextSize(2);
  tft.println("Security");
  tft.setCursor(30, 45);
  tft.println("Measures");
  tft.setCursor(20, 80);
  tft.println("Activated!");
  tft.setCursor(20, 120);
  tft.setTextSize(1);
  tft.println("Press Button 1");
  tft.setCursor(20, 135);
  tft.println("to deactivate");

}

void unlockAlarm(TFT_eSPI tft){
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Enter");
  tft.setCursor(20, 60);
  tft.println("Passcode");
  tft.setTextSize(1);
  tft.setCursor(30, 100);
  tft.println("Please Enter");
  tft.setCursor(40, 120);
  tft.println("4 Digits");
}

void unlock(TFT_eSPI tft){
  tft.fillScreen(TFT_GREEN);
  tft.setTextColor(TFT_WHITE, TFT_GREEN);
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Enter");
  tft.setCursor(20, 60);
  tft.println("Passcode");
  tft.setTextSize(1);
  tft.setCursor(30, 100);
  tft.println("Please Enter");
  tft.setCursor(40, 120);
  tft.println("4 Digits");
}

void logInUser(TFT_eSPI tft){
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Login");
  tft.setTextSize(1);
  tft.setCursor(15, 40);
  tft.println("Enter Username:");
  tft.println("press button 2 to");
  tft.println("to confirm after eachletter");
  tft.println("press button 1 to");
  tft.println("submit username");
  tft.println("2ABC");
  tft.println("3DEF");
  tft.println("4GHI");
  tft.println("5JKL");
  tft.println("6MNO");
  tft.println("7PQRS");
  tft.println("8TUV");
  tft.println("9XYZ");
}

void logInPass(TFT_eSPI tft){
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Login");
  tft.setTextSize(1);
  tft.setCursor(15, 40);
  tft.println("Enter Password:");
  tft.println("press button 2 to");
  tft.println("to confirm after eachletter");
  tft.println("press button 1 to");
  tft.println("submit username");
  tft.println("2ABC");
  tft.println("3DEF");
  tft.println("4GHI");
  tft.println("5JKL");
  tft.println("6MNO");
  tft.println("7PQRS");
  tft.println("8TUV");
  tft.println("9XYZ");
}


