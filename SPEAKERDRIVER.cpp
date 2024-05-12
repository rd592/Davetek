#include "SPEAKERDRIVER.h"
#include <cstdio>


Speaker :: Speaker(PwmOut &speaker) : _speaker(speaker){
}


//this function is used to set a new frequency for the Pwm output
void Speaker :: SetFreq(char frequency){

    float note = FindPitch(frequency);
    float period = 1/note;
    _frequency = note;
    _speaker.period(period);
}

//used to play an array of notes. Currently must be inputted one at a time 
void Speaker :: Play(vector<char> freqList){
    _speaker.resume();
    for(int i =0; i<freqList.size();i++){
        printf("PLAYING    ");
        SetFreq(freqList.at(i));
        wait_us(500000);
    }
    _speaker.suspend();

}
//must be a value between 1 and 0. controls the width of a pulse
void Speaker :: SetRatio(float ratio){
    _ratio = ratio;
    _speaker.write(ratio);
}

//finds the pitch in pitchArray from a given character input
//possible inputs are A,B,C,D,E,F,G
float Speaker :: FindPitch(char note){
    for(int i = 0; i < 8; i++){
        //finds note in pitch array once its location is found in note array
        if(noteArray[i] == note){
            
            return pitchArray[i][0];
        }
    }
    return 1; //null value
}

//returns the PWM pin
PwmOut Speaker :: GetPWM(){
    return _speaker;
}

//used to play a single tone
void Speaker :: Tone(int pitch){
    _frequency = pitch;
    float period = (float)1/(float)pitch;
    _speaker.period(period);
}

void Speaker :: Init(){
    _speaker.resume();
    float period = (float)1/(float)_frequency;
    _speaker.period(period);
    _speaker.write(_ratio);
    _speaker.suspend();
}
