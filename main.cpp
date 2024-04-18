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

//example code 

    /*

    lcd.init();
    buttonThread.start(buttonThreadFn);

@ -92,5 +97,36 @@ int main(){
            thread_sleep_for(400);
        }
    }
    */
//

    lcd.init();
    //buttonThread.start(buttonThreadFn);
    NewScene(0);
    NewButton(0, 1, "", 0, 0);

    Scene & baseScene = sceneManager.sceneArray[0];


    while(true){
        srand((unsigned)time(NULL));
        int random = rand() %4;
        baseScene.ClearScene(lcd);
        printf("%d", random);
        switch(random){      
            case 0: 
            NewText(0, "X", 0, 0);
            case 1:
            NewText(0, "X", 15, 0);
            case 3:
            NewText(0, "X", 0, 1);
            case 4:
            NewText(0, "X", 15, 1);
            
        }
        thread_sleep_for(1000);

    }

}
