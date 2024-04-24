#include "SCENE.h"


    //create a new scene. SceneID must be unique
    Scene::Scene(int sceneID ){
        _sceneID = sceneID;
    }

    Scene :: Scene(){
    }

//row is the the row of the lcd to check for a button in
    Button Scene :: GetButton(int row){

        for(int i = 0; i < buttonNum; i++){
            if(_buttonArray[i]._rowLoc == row){
                return _buttonArray.at(i);
            }

        }
        return _buttonArray.at(0);
    }

    //used to make a button in the menu. make sure text is shorter than 16 letters
    void Scene::NewButton(LCD1602 lcd, int nextScene, const char* buttonText, int column, int row){

        //there are no checks in place for how long the text can be. Be careful with text length.
        Button newButton1 = Button(lcd, nextScene, buttonText, column, row);//instantiate new button
        _buttonArray.push_back(newButton1);
        buttonNum ++;
    
         
    }

    //used to write text at a given point on the screen
    void Scene::NewText(LCD1602 lcd, const char *text, int column, int row){
        Button newText1 = Button(lcd, text, column, row);

        _textArray.push_back(newText1);
        textNum++;
    }

    void Scene::NewChar(LCD1602 lcd, const int *customChar, int column, int row){
        Button newText = Button(lcd, customChar, column, row);

        _charArray.push_back(newText);
        charNum++;
    }

    void Scene :: ClearScene(LCD1602 lcd){
        lcd.clear();
        _textArray.clear();
        _charArray.clear();
        _buttonArray.clear();

        buttonNum = 0;
        textNum = 0;
        charNum = 0;
        DisplayScene(lcd);
    }

    void Scene :: DisplayScene(LCD1602 lcd){

        lcd.clear();

        if(buttonNum >0){ //prints each button element
            for(int i = 0; i < buttonNum; i++){
            
            
            Button curButton = _buttonArray.at(i);
            int curCol = curButton._columnLoc;
            int curRow = curButton._rowLoc;

            if(curRow <=1 && curRow >=0){
                lcd.setCursor(curCol, curRow);
                lcd.write(curButton._buttonText);
            }

        }

        }

        if(textNum > 0){ //prints each text element
            for(int i = 0; i < textNum; i++){
            
            Button curButton = _textArray.at(i);
            int curCol = curButton._columnLoc;
            int curRow = curButton._rowLoc;

            if(curRow <=1 && curRow >=0){ //only displays what can currently be shown on the screen
                lcd.setCursor(curCol, curRow);
                lcd.write(curButton._buttonText);
            }
            }
        
        }

        if(charNum > 0){ //prints each custom text element
            for(int i = 0; i < charNum; i++){
            
            Button curButton = _charArray.at(i);
            int curCol = curButton._columnLoc;
            int curRow = curButton._rowLoc;

            //currently custom characters are displayed immediately when written. Might need to change this

            if(curRow <=1 && curRow >=0){ //only displays what can currently be shown on the screen
                //lcd.setCursor(curCol, curRow);

                lcd.setCursor(curCol, curRow);
                //lcd.loadCustomChar(0, curButton._customChar);
                lcd.writeCustomChar(0);
            }    
        }  
    }
}
