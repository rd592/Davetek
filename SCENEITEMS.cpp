#include "SCENEITEMS.h"

//constructor for buttons
Button :: Button(LCD1602 lcd, int nextScene, const char* buttonText, int columnLoc, int rowLoc){
        _columnLoc = columnLoc;
        _rowLoc = rowLoc;
        _nextScene = nextScene;
        _buttonText = buttonText;
}

//constructor for text
Button :: Button(LCD1602 lcd, const char* buttonText, int columnLoc, int rowLoc){
        _columnLoc = columnLoc;
        _rowLoc = rowLoc;
        _buttonText = buttonText;

}

//constructor for custom characters
Button :: Button(LCD1602 lcd, const int *customChar, int columnLoc, int rowLoc){
        _columnLoc = columnLoc;
        _rowLoc = rowLoc;
        _customChar = customChar;
        
        //these statements are used to find where to put the custom character

        //lcd.loadCustomChar(location, customChar);

}

Button :: Button(){

}
const char *Button :: GetText(){
    return _buttonText;
}

int Button :: GetNextScene(){
    return  _nextScene;
}
