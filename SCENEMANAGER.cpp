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