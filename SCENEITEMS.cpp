#include "SCENEITEMS.h"


Button :: Button(LCD1602 lcd, int nextScene, const char* buttonText, int columnLoc, int rowLoc){
        _columnLoc = columnLoc;
        _rowLoc = rowLoc;
        _nextScene = nextScene;
        _buttonText = buttonText;
}

Button :: Button(LCD1602 lcd, const char* buttonText, int columnLoc, int rowLoc){
        _columnLoc = columnLoc;
        _rowLoc = rowLoc;
        _buttonText = buttonText;

}

Button :: Button(){

}
const char *Button :: GetText(){
    return _buttonText;
}

int Button :: GetNextScene(){
    return  _nextScene;
}