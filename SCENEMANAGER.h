#include "SCENE.h"
#pragma once


//this class is whats used to create new scenes and any objects within the scenes.
class SceneManager{


    LCD1602 _lcd;

public:
//number of scenes is capped at 50 right now. making the array too large overlaps with other memory addresses. 
//I dont know how to fix this right now.
    Scene sceneArray[20];
    int sceneNum = 0;

    SceneManager(LCD1602 lcd);

    //these functions can be followed down to sceneitems through scene. 
    void NewScene(int ID);
    void NewButton(int sceneID, int nextScene, const char *text, int column, int row);
    void NewText(int sceneID, const char *text, int column, int row);
    void NewChar(int sceneID, const int *customChar, int column, int row); //custom char

    void DisplaySceneByID(int ID);
    //Scene GetScene();


};
