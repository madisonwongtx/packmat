void resetScreen(TFT_eSPI tft){
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
}

void welcome(TFT_eSPI tft){
  resetScreen(tft);
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
  tft.setCursor(40,10);
  tft.setTextSize(2);
  tft.println("Set");
  tft.setCursor(20, 60);
  tft.println("Passcode");
  tft.setTextSize(1);
  tft.setCursor(40, 100);
  tft.println("Please Enter");
  tft.setCursor(50, 120);
  tft.println("4 Digits");
}

void firstNum(TFT_eSPI tft, char key){
  resetScreen(tft);
  tft.setCursor(10, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void secondNum(TFT_eSPI tft, char key){
  resetScreen(tft);
  tft.setCursor(40, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void thirdNum(TFT_eSPI tft, char key){
  resetScreen(tft);
  tft.setCursor(70, 50);
  tft.setTextSize(4);
  tft.println(key);
}

void fourthNum(TFT_eSPI tft, char key){
  resetScreen(tft);
  tft.setCursor(100, 50);
  tft.setTextSize(4);
  tft.println(key);
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.println("Entered Code:");
}

void unarmed(tft){
  resetScreen(tft);
  tft.setCursor(20,10);
  tft.setTextSize(2);
  tft.println("Please");
  tft.setCursor(50, 40);
  tft.println("place");
  tft.setCursor(20, 70);
  tft.println("packages");
  tft.setCursor(20, 100);
  tft.println("on mat :)");
  
}


