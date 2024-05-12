#include "SCENEMANAGER.h"


SceneManager :: SceneManager(LCD1602 lcd): _lcd(lcd){
    
}

void SceneManager :: DisplaySceneByID(int ID){
    Scene& curScene = sceneArray[ID];
    curScene.DisplayScene(_lcd);
}

 void SceneManager :: NewScene(int ID){
    Scene newScene = Scene(ID);
    sceneArray[sceneNum] = newScene;
    sceneNum++;
}

void SceneManager :: NewButton(int sceneID, int nextScene, const char *text, int column, int row){
    Scene & curScene = sceneArray[sceneID];//find current scene
    curScene.NewButton(_lcd, nextScene, text, column, row);
}

void SceneManager :: NewText(int sceneID, const char *text, int column, int row){
    Scene & curScene = sceneArray[sceneID];
    curScene.NewText(_lcd, text, column, row);
}

void SceneManager :: NewChar(int sceneID, const int *customChar, int column, int row){
    Scene & curScene = sceneArray[sceneID];
    curScene.NewChar(_lcd, customChar, column, row);
}

void SceneManager :: ClearAll(){
    for(int i = 0; i <sceneNum; i++){
        sceneArray[i].ClearScene(_lcd);
        sceneArray[i].ClearChar(_lcd);

    }
}
void SceneManager :: ClearChars(){
    for(int i = 0; i <sceneNum; i++){
        sceneArray[i].ClearChar(_lcd);

    }
}
