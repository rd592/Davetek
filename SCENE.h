#include "LCD1602.h"
#include "mbed.h"
#include "SCENEITEMS.h"
#include <vector>

using namespace std;

class Scene {

    public:
    int _sceneID;
    bool active;
    
        vector<Button> _buttonArray;
        vector<Button> _textArray;
        vector<Button> _charArray;

        int textNum = 0;
        int buttonNum = 0;//number of buttons
        int charNum = 0;//number of custom chars

        Scene();
        Scene(int sceneID);


        void NewText(LCD1602 lcd,  const char *text, int column, int row); //used to print any basic character
        void NewChar(LCD1602 lcd, const int *customChar, int column, int row); //specifically for custom characters

        void NewButton(LCD1602 lcd, int nextScene, const char* buttonText, int column, int row);

        void ClearScene(LCD1602 lcd);
        void ClearChar(LCD1602 lcd);

        void DisplayScene(LCD1602 lcd);
        void ChangeScene(Button button);

        Button GetText(int column, int row);
        
};
