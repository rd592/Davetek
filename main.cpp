#include "mbed.h"
#include "SCENEMANAGER.h"
#include "LCD1602.h"

LCD1602 lcd(A1, A0, A2, A3, D4, D5, D6, D7);
SceneManager sceneManager(lcd);

/*
Scene scene0 = Scene(0);
Scene scene1 = Scene(1);
Scene scene2 = Scene(2);
Scene scene3 = Scene(3);
*/

DigitalIn button(BUTTON1);
DigitalIn button2(D8);

Thread buttonThread;



/* Im tired so Im going to bed but what needs to be done:

-find a way to display a scene based on the sceneID stored in a button
update main to work with that
make code a bit neater


*/


//these functions just allow scenes to be made quicker, dont have to reference sceneManager
void NewScene(int ID){
    sceneManager.NewScene(ID);
}
void NewButton(int sceneID, int nextScene, const char *text, int column, int row){
    sceneManager.NewButton(sceneID, nextScene, text, column, row);
}

void NewText(int sceneID, const char *text, int column, int row){
    sceneManager.NewText(sceneID, text, column, row);
}


//cursor flashing
void buttonThreadFn(){

    while(true){
        lcd.setCursor(14, 0);
        lcd.write("<");
        thread_sleep_for(800);
        lcd.setCursor(14, 0);
        lcd.write(" ");
        thread_sleep_for(800);
    }
}

int main(){
    lcd.init();
    buttonThread.start(buttonThreadFn);

    NewScene(0);  //default scene
    NewScene(1);
    NewScene(2);
    NewScene(3);
    
    NewButton(0, 1, "Scene 1?", 0, 0);
    NewButton(0, 2, "Scene 2?", 0, 1);

    NewText(1, "Scene1", 0,0);
    NewText(2, "Scene2", 0,0);

    sceneManager.DisplaySceneByID(0);

    Scene & defaultScene = sceneManager.sceneArray[0];

    while(true){

        if(!button){
            for(int i = 0; i < defaultScene.buttonNum; i++){
            Button & button = defaultScene._buttonArray[i];
            button._rowLoc--;
            }
            sceneManager.DisplaySceneByID(0);
            thread_sleep_for(400);
        }

        if(button2){
            Button curButton = defaultScene.GetButton(0);
            printf("hello %d", curButton.GetNextScene());
            sceneManager.DisplaySceneByID(curButton.GetNextScene());
            thread_sleep_for(400);
        }
    }
}

