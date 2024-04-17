#include "LCD1602.h"
#include "mbed.h"
#include "SCENEITEMS.h"

class Scene {

    public:
    int _sceneID;
    bool active;


        Button _buttonArray[10];//tracks the buttons in a scene
        Button _textArray[10];

        int textNum = 0;
        int buttonNum = 0;//number of buttons

        Scene();
        Scene(int sceneID);


        void NewText(LCD1602 lcd,  const char *text, int column, int row);
        void NewButton(LCD1602 lcd, int nextScene, const char* buttonText, int column, int row);

        void DisplayScene(LCD1602 lcd);
        void ChangeScene(Button button);

        Button GetButton(int row);
        
};