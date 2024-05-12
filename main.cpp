#include "mbed.h"
#include "SCENEMANAGER.h"
#include "LCD1602.h"
#include <cstdio>
#include <string>
#include "Timers.h"
#include "randLIB.h"
#include "SPEAKERDRIVER.h"



const int filledSquare[8] = { // Custom characters are stored as bit patterns (5x8 matrix)
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};
const int bottomLine[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111
};

LCD1602 lcd(A1, A0, D1, A3, D4, D5, D6, D7);
SceneManager sceneManager(lcd);
//DigitalOut backlight(A2);//controls backlight


DigitalIn B1(D9, PullUp); //left most button
DigitalIn B2(D10, PullUp);// all other pins have a constant output of 1 for some reason
DigitalIn B3(D11, PullUp);
DigitalIn B4(D12, PullUp);//right most button

PwmOut curSpeaker(D3);

Speaker speaker(curSpeaker); //speaker object


Thread buttonThread;
Thread speakerThread;

//dave says variables
short longestSequence = 0;//stores the longest sequence reached by the user ever in dave says
short difficultySet = 1; //controls how long between each sequence flash in daveSays. diffiuclty setting

short easyTimePre = 500, easyTimePost = 800;
short hardTimePre = 200, hardTimePost = 500;
short cusTimePre = 0, cusTimePost = 200; //used only when player is pressing buttons back in sequence.
//

vector<char> sequenceList = {
    'x','x','x','x',
    'x','x','x','x',
    'x','x','x','x',
    'x','x','x','x',

    'x','x','x','x',
    'x','x','x','x',
    'x','x','x','x',
    'x','x','x','x'     

    };//Sequence-x note grid list

//tracks if a song has been written for play back sequence-x
bool songWritten = 0;


Scene quickDrawScene, daveScene, menuScene, sequenceScene;
void MainMenuLoop(Scene &menuScene);

//these functions just allow scenes to be made quicker, dont have to reference sceneManager every time
void NewScene(int ID){
    sceneManager.NewScene(ID);
}
void NewButton(int sceneID, int nextScene, const char *text, int column, int row){
    sceneManager.NewButton(sceneID, nextScene, text, column, row);
}
void NewText(int sceneID, const char *text, int column, int row){
    sceneManager.NewText(sceneID, text, column, row);
}
void NewCustomChar(int sceneID, const int *customChar, int column, int row){
    sceneManager.NewChar(sceneID, customChar, column, row);

    //lcd.setCursor(column, row);
    //lcd.writeCustomChar(0);
}

void DaveSaysLoop(Scene &baseScene);
//this function is used to exit the loop when the player is wrong in Dave Says
void WrongFunc(Scene &baseScene, short numSequence, short &longestSequence){
    
    int sceneID = baseScene._sceneID;
    sceneManager.ClearAll();
    NewText(sceneID, "WRONG!!!!!!!!!", 0, 0);
    sceneManager.DisplaySceneByID(sceneID);
    
    thread_sleep_for(2000);

    int tempNum = numSequence - 1;
    char buffer[20];
    char buffer2[30];

    //displays the length of the sequence the player reached
    int n = sprintf(buffer, "YOU GOT %d RIGHT", tempNum);
    sceneManager.ClearAll();
    NewText(sceneID, buffer, 0, 0);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(2000);

    //displays the longest sequence reached
    if(longestSequence < tempNum){
        longestSequence = tempNum;
    }
    int p = sprintf(buffer2, "LONGEST SEQUENCE %d ", longestSequence);
    sceneManager.ClearAll();
    NewText(sceneID, buffer2, 0, 0);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(2000);

    DaveSaysLoop(baseScene); //reset loop.
}
//used to control which area of the screen is displayed in dave Says
//pre sleep and post sleep control how long the text is displayed.
void DSPrintOutSection(Scene &baseScene, int caseNum, int difficulty){
    //each case refers to a different section of the screen.
    //prints a filled square and a number at a given section
    int sceneID = daveScene._sceneID;
    sceneManager.ClearAll();
    switch (caseNum){
        case 0:
            NewText(sceneID, "1", 0, 0);
            NewCustomChar(sceneID, filledSquare, 1, 0);
            NewCustomChar(sceneID, filledSquare, 2, 0);
            break;
        case 1:
            NewCustomChar(sceneID, filledSquare, 15, 0);
            NewCustomChar(sceneID, filledSquare, 14, 0);
            NewText(sceneID, "2", 13, 0);
            break;
        case 2:
            NewText(sceneID, "3", 0, 1);
            NewCustomChar(sceneID, filledSquare, 1, 1);
            NewCustomChar(sceneID, filledSquare, 2, 1);
            break;
        case 3:
            NewCustomChar(sceneID, filledSquare, 15, 1);
            NewCustomChar(sceneID, filledSquare, 14, 1);
            NewText(sceneID, "4", 13, 1);
            break;

        
    }
    //if difficulty = 1, change time between each flash
    int tempPre = easyTimePre, tempPost = easyTimePost;
    if(difficulty == 1){
        tempPre = hardTimePre;
        tempPost = hardTimePost;
    }
    //only active when player is pressing buttons back. Shortens time
    else if(difficulty == 2){
        tempPre = cusTimePre;
        tempPost = cusTimePost;
    }

    thread_sleep_for(tempPre);
    sceneManager.DisplaySceneByID(sceneID); //custom characters currently arent compatible with scenes. Print them seperately.
    thread_sleep_for(tempPost);
    sceneManager.ClearAll();
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

void speakerThreadFn(){
    int freq = 261;
    speaker.SetFreq('A');
    speaker.SetRatio(0.4f);

    while(true){
        thread_sleep_for(80);
    }
}



//this is the code for the main loop of dave says
void DaveSaysLoop(Scene &baseScene){
    

    int sceneID = baseScene._sceneID;

    lcd.loadCustomChar(0, filledSquare);
    //random number generators dont seem to be working properly, a starting point in this array will be chosen randomly instead
    int numArray[] = {0, 2, 1, 2, 0, 3, 1, 1 ,3 ,2, 1, 0, 0, 1, 2, 0, 3, 3, 
                    2, 1, 0, 1, 3, 2, 3, 0, 1, 0, 2, 2, 0, 0, 1, 2, 3, 1, 0, 
                    1, 0, 2, 3, 0, 3, 3, 2, 1, 0, 0, 1, 3, 1, 1, 3};

    int numArrayLength = 53;

    int numSequenceArray[100]; // max number of sequenced numbers
    int numSequence = 0;//number of numbers in the sequence

    int counter = 0; //counter is used to determine the starting point in numArray
    menuScene.ClearScene(lcd);
    sceneManager.ClearAll();
    NewText(sceneID, "B4 = MAIN MENU", 0, 0);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(2000);
    sceneManager.ClearAll();

    printf("ADHADSDFADAD");
    NewText(sceneID, "EASY MODE  (B1)", 0, 0);
    NewText(sceneID, "HARD MODE  (B2)", 0, 1);
    sceneManager.DisplaySceneByID(sceneID); //display the scene


    //this is the main while loop which contains the whole game
    while(true){
        if(counter >= numArrayLength){
            counter = 0;
        }
        counter ++;

        //if button input
        if(!B1 || !B2){
            //button 1 is easy mode, button 2 is hard mode
            if(!B1){
                difficultySet = 0;
            }
            else{
                difficultySet = 1;
            }
            

            int random =0;
            sceneManager.ClearAll();//clears all text in the scene

            while(true){
                thread_sleep_for(1000); //time before sequence starts

                if(counter >= numArrayLength){ //counter cant go out of arrays bounds
                    counter = 0;
                }

                random = numArray[counter];//chooses first random number from numArray
                counter++;

                numSequenceArray[numSequence] = random; //adds number to the array
                numSequence++;

                //if difficulty setting is hard, 2 numbers are added to the sequence instead of 1
                if(difficultySet == 1){
                    random = numArray[counter];//chooses first random number from numArray
                    counter++;
                    numSequenceArray[numSequence] = random;
                    numSequence++;
                }
                
                for(int i = 0; i < numSequence; i++){
                    sceneManager.ClearAll();
                    thread_sleep_for(10);

                    //500ms and 800ms are long enough for this. Can easily be changed for difficulty settings
                   //this function chooses where on the screen to print out based on the inputs 
                    DSPrintOutSection(daveScene, numSequenceArray[i], difficultySet);
                }

                thread_sleep_for(100);    
                sceneManager.ClearAll();
                NewText(sceneID, "SEQUENCE?", 0, 0);
                sceneManager.DisplaySceneByID(sceneID); //display the scene

                int i = 0;
                bool buttonStillPressed = 1; //this bool is used to make sure the loop detects multiple disinct button presses
                bool b1=0, b2=0, b3=0, b4=0;
                while(i != numSequence){

                    for(i = 0; i < numSequence; i++){
                        //code for detecting button inputs
                        while(true){
                            
                            
                            //prevents loop being triggered until all buttons are unpressed
                            if(B1 && B2 && B3 && B4){
                                buttonStillPressed = 0;
                                thread_sleep_for(10);
                            }

                            if(buttonStillPressed == 0){
                                if(!B1){
                                    b1 = true;
                                }
                                else if(!B2){
                                    b2 = true;
                                }
                                else if(!B3){
                                    b3 = true;
                                }
                                else if(!B4){
                                    b4 = true;
                                }
                            }

                            //if button 1 pressed
                            if(b1){
                                b1=0, b2=0, b3=0, b4=0;
                                if(numSequenceArray[i] == 0){                                         
                                    buttonStillPressed = 1;
                                    sceneManager.ClearAll();
                                    DSPrintOutSection(daveScene, 0, 2);
                                    break;                           
                                }
                                else{
                                sceneManager.ClearAll();
                                WrongFunc(baseScene, numSequence, longestSequence);
                                }
                                
                            }
                            //if button 2 pressed
                            else if(b2){
                                b1=0, b2=0, b3=0, b4=0;
                                if(numSequenceArray[i] == 1){ 
                                    buttonStillPressed = 1;
                                    sceneManager.ClearAll();
                                    DSPrintOutSection(daveScene,1, 2);
                                    break;                           
                                }
                                else{
                                sceneManager.ClearAll();
                                WrongFunc(baseScene, numSequence, longestSequence);
                                }
                            }
                            //if button 3 pressed
                            else if(b3){
                                b1=0, b2=0, b3=0, b4=0;
                                if(numSequenceArray[i] == 2){ 
                                    buttonStillPressed = 1;
                                    sceneManager.ClearAll();//clears all text in the scene
                                    DSPrintOutSection(daveScene, 2, 2);
                                    break;                           
                                }
                                else{
                                sceneManager.ClearAll();
                                WrongFunc(baseScene, numSequence, longestSequence);
                                }
                            }
                            //if button 4 pressed
                            else if(b4){
                                b1=0, b2=0, b3=0, b4=0;
                                if(numSequenceArray[i] == 3){ 
                                    buttonStillPressed = 1;
                                    sceneManager.ClearAll();//clears all text in the scene
                                    DSPrintOutSection(daveScene, 3, 2);
                                    break;                           
                                }
                                else{
                                sceneManager.ClearAll();
                                WrongFunc(baseScene, numSequence, longestSequence);
                                }
                            }

                            thread_sleep_for(30);
                        }
                    }
                    sceneManager.ClearAll();
                    thread_sleep_for(200);
                    NewText(sceneID, "CORRECT!!", 0, 0);
                    sceneManager.DisplaySceneByID(sceneID);                   
                }        
            }
        }
        //return to main menu
        else if(!B4){
            MainMenuLoop(menuScene);
        }
        thread_sleep_for(10);
    }
}

void quickDrawSinglePlayerGame(Scene &quickDrawScene){
    int sceneID = quickDrawScene._sceneID;
    sceneManager.ClearAll();

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int reaction[5]; //Holds the individual reaction times of the player
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability

    char buffer[30];//used to print reaction time
    int p;

    //Welcome message
    NewText(sceneID, "WELCOME TO", 0, 0); 
    NewText(sceneID, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(1000);

    sceneManager.ClearAll();
    NewText(sceneID, "B1 = START GAME", 0, 0);
    NewText(sceneID, "B4 = MAIN MENU", 0, 1);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(2000);

    while(true){
        if(!B1){
            //Game begin
            while(gameOver == false){
                //Use a for loop to determine the amount of rounds
                for(int i = 0; i < lengthOfGame; i++){
                    lcd.toggleBackLight(0);//0 is on 1 is off

                    //Display what round it is and give a prompt to the user that it is starting
                    sceneManager.ClearAll();
                    sceneManager.ClearAll();
                    p = sprintf(buffer, "ROUND: %d ", i+1);
                    NewText(sceneID, buffer, 0, 0);   
                    sceneManager.DisplaySceneByID(sceneID);
                    thread_sleep_for(1000);
                    sceneManager.ClearAll();
                    lcd.toggleBackLight(0);//0 is on 1 is off

                    //Generate a random amount of time in milliseconds (not microseconds)
                    randLIB_seed_random();
                    react = randLIB_get_8bit() * 15;
                    timer.start();

                    while(timer.elapsed() < react && roundOver == false){
                        if(!B1 || !B2 || !B3 || !B4){
                            timer.stop();
                            lcd.toggleBackLight(0);//0 is on 1 is off
                            NewText(sceneID, "TOO EARLY", 0, 0);
                            sceneManager.DisplaySceneByID(sceneID);
                            thread_sleep_for(1000);
                            sceneManager.ClearAll();

                            reaction[i] = 5000; //penalty for being too early
                            roundOver = true;   
                        }
                    }

                    if(roundOver == false){
                        lcd.toggleBackLight(1);//0 is on 1 is off
                        NewText(sceneID, "REACT" , 0, 0); //Use backlight here
                        sceneManager.DisplaySceneByID(sceneID);
                    }

                    while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                        if(!B1 || !B2 || !B3 || !B4){
                            timer.stop();
                            lcd.toggleBackLight(0);//0 is on 1 is off
                            sceneManager.ClearAll();
                            p = sprintf(buffer, "REACTION TIME:   %dms ", timer.elapsed() - react);
                            NewText(sceneID, buffer, 0, 0);   
                            sceneManager.DisplaySceneByID(sceneID);
                            thread_sleep_for(1000);
                            sceneManager.ClearAll();

                            reaction[i] = timer.elapsed() - react; //No penalty, just the reaction time of the player
                            roundOver = true;
                        }
                    }
                    if(timer.elapsed() >= 5000){
                        timer.stop();
                        lcd.toggleBackLight(0);//0 is on 1 is off
                        NewText(sceneID, "TOO LATE", 0, 0); //fix
                        sceneManager.DisplaySceneByID(sceneID);
                        thread_sleep_for(1000);
                        sceneManager.ClearAll();

                        reaction[i] = 5000; // penalty for being too late
                    }
                    roundOver = false;
                }
                //Calculate average reaction time
                for(int i = 0; i < lengthOfGame; i++){
                    average = average + reaction[i];
                }
                average = average/lengthOfGame;

                sceneManager.ClearAll();
                p = sprintf(buffer, "AVERAGE TIME:   %dms ", average);
                NewText(sceneID, buffer, 0, 0);

                //NewText(1, "YOUR REACTION", 0, 0);
                //NewText(1, "TIME", 0, 1); //add in the average here with the fix
                sceneManager.DisplaySceneByID(sceneID);
                thread_sleep_for(2000);
                quickDrawSinglePlayerGame(quickDrawScene);
            }
            
        }
        else if(!B4){
        MainMenuLoop(menuScene);
        }                   
    } 
    
}

void quickDrawMultiplayerGameHost(Scene &quickDrawScene){
    //The game needs to start simulatenously for both players
    //Send a ping to the joined player here
    //the ping needs to tell the guest that the quickDrawMultiplayerGameGuest needs to start running
    sceneManager.ClearAll();

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability
    int reaction[lengthOfGame]; //Holds the individual reaction times of the player
    int guestAverage = 0; 

    //Welcome message
    NewText(1, "MULTIPLAYER", 0, 0); 
    NewText(1, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(1);
    thread_sleep_for(1000);

    //Game begin
    while(gameOver == false){
        //Use a for loop to determine the amount of rounds
        for(int i = 0; i < lengthOfGame; i++){

            //Display what round it is and give a prompt to the user that it is starting
            sceneManager.ClearAll();
            NewText(1, "ROUND ", 0, 0); //fix
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(500);
            sceneManager.ClearAll();

            //Generate a random amount of time in milliseconds (not microseconds)
            randLIB_seed_random();
            react = randLIB_get_8bit() * 15;
            timer.start();

            while(timer.elapsed() < react && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    NewText(1, "TOO EARLY", 0, 0);
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    sceneManager.ClearAll();

                    reaction[i] = 5000; //penalty for being too early
                    roundOver = true;   
                }
            }

            if(roundOver == false){
                NewText(1, "REACT" , 0, 0); //Use backlight here
                sceneManager.DisplaySceneByID(1);
            }

            while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    sceneManager.ClearAll();
                    NewText(1, "REACTION TIME", 0, 0); //fix
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    sceneManager.ClearAll();

                    reaction[i] = timer.elapsed(); //No penalty, just the reaction time of the player
                    
                }
            }
            if(timer.elapsed() >= 1000){
                timer.stop();
                NewText(1, "TOO LATE", 0, 0); //fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(1000);
                sceneManager.ClearAll();

                reaction[i] = 5000; // penalty for being too late
            }
            roundOver = false;
        }
        //Calculate average reaction time
        for(int i = 0; i < lengthOfGame; i++){
            average = average + reaction[i];
        }
        average = average/lengthOfGame;

        NewText(1, "YOUR REACTION", 0, 0);
        NewText(1, "TIME", 0, 1); //add in the average here with the fix
        sceneManager.DisplaySceneByID(1);
        thread_sleep_for(2000);
   
        
        NewText(1, "PLAY AGAIN", 0, 0);
        NewText(1, "B1 IS Y B4 IS N", 0, 1);
        sceneManager.DisplaySceneByID(1);
        playAgain = false;
        while(playAgain == false){
            if(B1){
                playAgain = true;
            }
            if(B4){
                gameOver = true;
                playAgain = true;
            }
        }
    } 
}

void quickDrawMultiplayerGameGuest(Scene &quickDrawScene){
    sceneManager.ClearAll();

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    bool progress = false;
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int hostAverage = 0;
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability
    int reaction[lengthOfGame]; //Holds the individual reaction times of the player

    //Welcome message
    NewText(1, "MULTIPLAYER", 0, 0); 
    NewText(1, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(1);

    //Game begin
    while(gameOver == false){
        //Get a ping for progress here
        if(progress == true){
            progress = false;
            //Use a for loop to determine the amount of rounds
            for(int i = 0; i < lengthOfGame; i++){

                //Display what round it is and give a prompt to the user that it is starting
                sceneManager.ClearAll();
                NewText(1, "ROUND ", 0, 0); //fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(500);
                sceneManager.ClearAll();

                //Get this random number from the host not generated by the guest
                randLIB_seed_random();
                react = randLIB_get_8bit() * 15;
                timer.start();

                while(timer.elapsed() < react && roundOver == false){
                    if(B1 || B2 || B3 || B4){
                        timer.stop();
                        NewText(1, "TOO EARLY", 0, 0);
                        sceneManager.DisplaySceneByID(1);
                        thread_sleep_for(1000);
                        sceneManager.ClearAll();

                        reaction[i] = 5000; //penalty for being too early
                        roundOver = true;   
                    }
                }

                if(roundOver == false){
                    NewText(1, "REACT" , 0, 0); //Use backlight here
                    sceneManager.DisplaySceneByID(1);
                }

                while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                    if(B1 || B2 || B3 || B4){
                        timer.stop();
                        sceneManager.ClearAll();
                        NewText(1, "REACTION TIME", 0, 0); //fix
                        sceneManager.DisplaySceneByID(1);
                        thread_sleep_for(1000);
                        sceneManager.ClearAll();

                        reaction[i] = timer.elapsed(); //No penalty, just the reaction time of the player
                        
                    }
                }
                if(timer.elapsed() >= 1000){
                    timer.stop();
                    NewText(1, "TOO LATE", 0, 0); //fix
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    sceneManager.ClearAll();

                    reaction[i] = 5000; // penalty for being too late
                }
                roundOver = false;

                while(progress == false){
                    //get progress from host here
                    progress = true;
                }
            }
            //Calculate average reaction time
            for(int i = 0; i < lengthOfGame; i++){
                average = average + reaction[i];
            }
            average = average/lengthOfGame;

            NewText(1, "YOUR REACTION", 0, 0);
            NewText(1, "TIME", 0, 1); //add in the average here with the fix
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(2000);

            NewText(1, "HOST REACTION", 0, 0);
            NewText(1, "TIME", 0, 1); //add in the host average here with the fix, will need to get this from payload
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(2000);

            //Compare reaction time and assign a winner and loser
            //Need to get reaction time from host
            hostAverage = 500; //Get the function here to get the thing from the host
            if(average > hostAverage){
                sceneManager.ClearAll();
                NewText(1, "YOU LOSE", 0, 0);
                NewText(1, "", 0, 1); //add in the average here with the fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(2000);
            }
            else{
                sceneManager.ClearAll();
                NewText(1, "YOU WIN", 0, 0);
                NewText(1, "", 0, 1); //add in the average here with the fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(2000);
            }
            sceneManager.ClearAll();
        }
    }
}


vector<float> testList = {329,293,261,293,329,329,329,0.1,293,293,293,0.1,329,392,392};
vector<char>testList2 = {'E','D','C','D','E','E','E','X','D','D','D','X','E','G','G','X'};


void NoteGrid(Scene &sequenceScene, LCD1602 lcd);//allows note grid to be called in menu loop
//contains the options to play song or write song
void MenuLoop(Scene &sequenceScene, LCD1602 lcd){

    int sceneID = sequenceScene._sceneID;
    menuScene.ClearScene(lcd);
    sceneManager.ClearAll();
    NewText(sceneID, "WRITE SONG? (B1)", 0, 0);
    NewText(sceneID, "PLAY SONG?  (B2)", 0, 1);

    sceneManager.DisplaySceneByID(sceneID);
    while(true){
        //button 1 input = play song
        if(!B2 && songWritten){
            //logic for playing song
            speaker.Play(sequenceList);
            thread_sleep_for(500);
        }
        else if(!B1){
            NoteGrid(sequenceScene, lcd);
        }
        else if(!B4){
            MainMenuLoop(menuScene);
        }
    }


}

//B1 = scroll left
//B2 = scroll right
//B3 = change note up
//B4 = play song
void NoteGrid(Scene &sequenceScene, LCD1602 lcd){

    int sceneID = sequenceScene._sceneID;

    sceneManager.ClearAll();
    lcd.loadCustomChar(0, bottomLine);//loads in the custom character for cursor
    int currentCol = 0;
    int currentRow = 0;

    sceneManager.ClearAll();
    NewCustomChar(sceneID, bottomLine, currentCol, currentRow);
    sceneManager.DisplaySceneByID(sceneID);

    char full[33] = {
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  
            };

    NewText(sceneID, full, 0, 0);
    sceneManager.DisplaySceneByID(sceneID);
    thread_sleep_for(1000);

    while(true){
        //shift to left
        if(!B1){
            if(currentCol>0){
                currentCol--;
            }
            //move from second row to first row
            else if(currentCol==0 && currentRow ==1){
                currentCol = 15;
                currentRow = 0;
            }
            sceneManager.ClearChars();
            NewCustomChar(sceneID, bottomLine, currentCol, currentRow);
            sceneManager.DisplaySceneByID(sceneID);
            thread_sleep_for(200);
        }
        //shift to the right
        else if(!B2){
            if(currentCol<15){
                currentCol++;
            }
            //move to second row
            else if(currentCol==15 && currentRow==0){
                currentCol = 0;
                currentRow=1;
            }
            sceneManager.ClearChars();
            NewCustomChar(sceneID, bottomLine, currentCol, currentRow);
            sceneManager.DisplaySceneByID(sceneID);
            thread_sleep_for(200);
        }

        else if(!B3){
            //logic for changing note up
            char curNote = sequenceList.at((currentRow*16) + currentCol);
            char temp;
            for(int i = 0; i< 8; i++){
                if(speaker.noteArray[i] == curNote){
                    if(i == 7){
                        temp = speaker.noteArray[0]; //reset to 1st position if greater than 6
                        sequenceList.at((currentRow*16) + currentCol) = temp;
                    }
                    else{
                        temp = speaker.noteArray[i+1]; //gets the note in the array above curNote
                        sequenceList.at((currentRow*16) + currentCol) = temp;
                    }             
                }   
            }
            songWritten = true; //now a note has been placed, song can be played back

            char sequenceChar[33] = {
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  

            };
            int counter = 0;

            for(int j = 0; j < 32; j++){
                char temp = sequenceList[j];
                sequenceChar[j] = temp;      
                counter ++;    
            }
            //print out the characters on screen
            sceneManager.ClearAll();
            
            NewText(sceneID, sequenceChar, 0, 0);
            //printf("CNNEFOEINFEFNEFN");

            sceneManager.DisplaySceneByID(sceneID);
            thread_sleep_for(200);
            NewCustomChar(sceneID, filledSquare, currentCol, currentRow);
            sceneManager.DisplaySceneByID(sceneID);
        }
        else if(!B4){
            thread_sleep_for(1000);
            MenuLoop(sequenceScene, lcd);
        }
        thread_sleep_for(20);
    }

}

void SequenceLoop(Scene &sequenceScene){
    sceneManager.ClearAll();
    int sceneID = sequenceScene._sceneID;

    while(true){
        
        NewText(sceneID, "B1 = MOVE LEFT", 0, 0);
        NewText(sceneID, "B2 = MOVE RIGHT", 0, 1);
        sceneManager.DisplaySceneByID(sceneID);
        thread_sleep_for(3000);
        sceneManager.ClearAll();

        NewText(sceneID, "B3 = CHANGE NOTE", 0, 0);
        NewText(sceneID, "B4 = GO BACK", 0, 1);
        sceneManager.DisplaySceneByID(sceneID);
        thread_sleep_for(3000);
        while(true){
            MenuLoop(sequenceScene, lcd);
        }
    }
}


void MainMenuLoop(Scene &menuScene){
    sceneManager.ClearAll();

    int sceneID = menuScene._sceneID;
    int count = 500;
    bool screen = 0;
    while(true){
        

        while(true){

            if(count >500 && screen == 0){
                screen = 1;
                count = 0;
                sceneManager.ClearAll();
                NewText(sceneID, "B1 = QUICK DRAW", 0, 0);
                NewText(sceneID, "B2 = DAVE SAYS", 0, 1);
                sceneManager.DisplaySceneByID(sceneID);

            }
            else if(count > 500 && screen == 1){
                screen = 0;
                count = 0;
                sceneManager.ClearAll();
                NewText(sceneID, "B3 = SEQUENCE-X", 0, 0);
                sceneManager.DisplaySceneByID(sceneID);

            }

            if(!B1){
                thread_sleep_for(100);
                sceneManager.ClearAll();
                quickDrawSinglePlayerGame(quickDrawScene);
            }
            else if(!B2){
                thread_sleep_for(100);
                sceneManager.ClearAll();
                NewText(1, "WELCOME TO", 0, 0);
                NewText(1, "DAVE SAYS", 0, 1);
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(2000);
                DaveSaysLoop(daveScene);
            }
            else if(!B3){
                thread_sleep_for(100);
                SequenceLoop(sequenceScene);
            }
    
        count++;
        thread_sleep_for(10);
        }

        
    }
}

int main(){

    //speakerThread.start(speakerThreadFn);
    
    lcd.init(); //initialise LCD 
    thread_sleep_for(10);
    lcd.init(); //initialise LCD 
    speaker.Init();
    //speaker.Tone(440);

    NewScene(0);//main menu
    NewScene(1);//DaveSays
    NewScene(2);//QuickDraw
    NewScene(3);//Sequence-X

    //sceneManager.DisplaySceneByID(3);

    Scene & menuScene = sceneManager.sceneArray[0];
    Scene & daveScene = sceneManager.sceneArray[1]; //reference to dave says scene
    Scene & quickDrawScene = sceneManager.sceneArray[2];
    Scene & sequenceScene = sceneManager.sceneArray[3];

    menuScene.ClearScene(lcd);
    daveScene.ClearScene(lcd);
    NewText(0, "DAVETEK LTD", 0, 0);
    sceneManager.DisplaySceneByID(0);
    thread_sleep_for(2000);
    MainMenuLoop(menuScene);

}
