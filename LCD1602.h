#include "mbed.h"
#pragma once

class LCD1602 {
    DigitalOut _rs, _enable, _backlight, _contrastOff; //contrast is on when input is 0
    //BusOut _dataPins;
    DigitalOut _D4, _D5, _D6, _D7;

    int _ccolumn; //cursors column position
    int _crow; //cursors row position

    int _column;
    int _row;



    int address(int column, int row);
    void character(int column, int row, int c);
    void writeByte(int value);
    void writeCommand(int command);
    void writeData(int data);

    void handleNewline();
    void incrementColumn();
    void incrementRow();
    void checkRowOverflow();
    
public:

    LCD1602(PinName rs, PinName e, PinName bl, PinName cr, PinName d4, PinName d5, PinName d6, PinName d7);

    void clear();
    void init();
    void write(const char* string);
    void setCursor(int column, int row);
    int getCursorCol(); //returns current column of cursor
    int getCursorRow(); //returns current row of cursor
    void writeChar(int data);
    void loadCustomChar(int index, const int* pattern);
    void writeCustomChar(int index);
    void toggleBackLight(bool value);
};
