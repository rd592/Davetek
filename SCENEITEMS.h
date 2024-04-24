
#include "mbed.h"
#include "LCD1602.h"



//used for text and buttons. different constructors for each
class Button{

    int _sceneID;
    int _nextScene; //each new scene will be given a unique int value. This is the scene the button will change to


    public:
    const char *_buttonText; //text displayed by button.
    const int *_customChar; //stores custom char if there is one
    int _columnLoc; //button location
    int _rowLoc;

    Button();
    Button(LCD1602 lcd, const char* buttonText, int columnLoc, int rowLoc);//text
    Button(LCD1602 lcd, const int *customChar, int columnLoc, int rowLoc);//custom characters. Due to laziness this will load the text instantly.
    Button(LCD1602 lcd, int nextScene, const char* buttonText, int columnLoc, int rowLoc);//button
    const char *GetText();

    int GetNextScene();

};
