#include "mbed.h"
#include <vector>

class Speaker{

int _frequency=440;
float _ratio=0.5;

PwmOut &_speaker;

public:

//2d array allows for multiple octaves in future. 
//Limited to C major as sharp and flat symbols will be a challenge to implement
float pitchArray[8][1] = {
    {131}, //C
    {147}, //D
    {165}, //E
    {175}, //F
    {196}, //G
    {220}, //A
    {247}, //B
    {0.1}, //x is null note

};
char noteArray[8] = {
    'C','D','E','F','G','A','B','x'
};

Speaker(PwmOut &speaker);

void Init();
void Play(vector<char> freqList);//used to play an array of notes. Currently must be inputted one at a time 
void SetFreq(char frequency);//this function is used to set a new frequency for the Pwm output
void SetRatio(float ratio);//must be a value between 1 and 0. controls the width of a pulse
float FindPitch(char note);//finds the pitch in pitchArray from a given character input
void Tone(int pitch);//used to play a single tone

PwmOut GetPWM();//returns the PWM pin

};
