#include "lcd.h"
#include <LiquidCrystal_I2C.h>

// Use internal linkage so the global name doesn't collide with other translation units
static LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pre-built empty line for quick clearing
static char emptyLine[17] = "                "; // 16 spaces + null terminator

void LcdModule::initialize(){
    lcd.init();
    // lcdInternal.clear();
    changeBackLightState(true);
}

void LcdModule::setTextFirstLine(char *text) {
    lcd.setCursor(0, 0);
    lcd.print(text);
}

void LcdModule::setTextSecondLine(char *text){
    lcd.setCursor(0, 1);
    lcd.print(text);
}
void LcdModule::setTextSecondLine(int number){
    lcd.setCursor(0, 1);
    lcd.print(number);
}

void LcdModule::setTextLineWrap(char *text){

}

void LcdModule::clearLineOne(){
    setTextFirstLine(emptyLine);
}

void LcdModule::clearLineTwo(){
    setTextSecondLine(emptyLine);
}

void LcdModule::clearDisplay(){
    lcd.clear();
}

void LcdModule::changeBackLightState(bool state){
    lcd.setBacklight(state);
    backLightState = state;
}