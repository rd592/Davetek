#include "LCD1602.h"
#include "mbed.h"


LCD1602::LCD1602(PinName rs, PinName e, PinName bl, PinName cr, PinName d4, PinName d5,
                 PinName d6, PinName d7) : _rs(rs),
        _enable(e), _backlight(bl), _contrastOff(cr), _D4(D4), _D5(D5), _D6(D6), _D7(D7) {
}



void LCD1602::init() {
    _enable  = 0;
    wait_us(40);
    _enable = 1;
    _rs = 0;            // command mode

    // Wait for display to start
    wait_us(15000);

    // send "Display Settings" 3 times on startup & prepare to transition to 4-bit mode
    for (int i=0; i<3; i++) {
        writeByte(0x3);
        wait_us(1640);
    }

    // Set LCD to 4-bit mode
    writeByte(0x2);
    wait_us(40);

    writeCommand(0x28); // Function set 001 BW N F - -

    // Turn on display without cursor and without blinking
    writeCommand(0x0C);

    writeCommand(0x6);  // Set Cursor Direction and Display Shift
    //setCursor(0,0);
    clear();
}

void LCD1602::character(int column, int row, int ch) {
    int addr = address(column, row);
    writeCommand(addr);
    writeData(ch);
}

void LCD1602::write(const char* str) {
    while (*str != '\0') {
        writeChar(static_cast<int>(*str));
        str++;
        //str++;
    }
}

void LCD1602::writeChar(int value) {
    if (value == '\n') {
        handleNewline();
    } else {
        character(_column, _row, value);
        incrementColumn();
        checkRowOverflow();
    }
}

void LCD1602::loadCustomChar(int index, const int* pattern) {
    writeCommand(0x40 + (index * 8)); // Set address for the character
    for (int i = 0; i < 8; i++) {
        writeData(pattern[i]); // Write the pattern to CGRAM
    }
}

void LCD1602::writeCustomChar(int index) {
    character(_column, _row, index);
    incrementColumn();
    checkRowOverflow();
}

void LCD1602::handleNewline() {
    _column = 0;
    _row++;
    checkRowOverflow();
}

void LCD1602::incrementColumn() {
    _column++;
    if (_column >= 16) {
        _column = 0;
        incrementRow();
    }
}

void LCD1602::incrementRow() {
    _row++;
    checkRowOverflow();
}

void LCD1602::checkRowOverflow() {
    if (_row >= 2) {
        _row = 0;
    }
}

void LCD1602::writeByte(int value) {
    //_dataPins = value >> 4; // Send the higher 4 bits of the value to the data pins
    _D7 = (value >>7)& 0x01;
    _D6 = (value >>6)& 0x01;
    _D5 = (value >>5)& 0x01;
    _D4 = (value >>4)& 0x01;

    wait_us(40);            // Wait for the display
    _enable = 0;            
    wait_us(40);
    _enable = 1;
    //_dataPins = value >> 0; // Send the lower 4 bits of the value to the data pins
    _D7 = (value >>3)& 0x01;
    _D6 = (value >>2)& 0x01;
    _D5 = (value >>1)& 0x01;
    _D4 = (value)& 0x01;

    wait_us(40);
    _enable = 0;
    wait_us(40);
    _enable = 1;
}

void LCD1602::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
}

void LCD1602::writeData(int data) {
    _rs = 1;
    writeByte(data);
}

int LCD1602::address(int column, int row) {
    return 0x80 + (row * 0x40) + column;
}

void LCD1602::setCursor(int column, int row) {
    _column = column;
    _row = row;

    _ccolumn = column;//update cursor position
    _crow = row;
}

//column first 4 bytes, row last 4 bytes
int LCD1602 :: getCursorCol(){

    return _ccolumn;
}
int LCD1602 :: getCursorRow(){
    return _crow;
}

void LCD1602::clear() {
    writeCommand(0x01); // Clear the display
    wait_us(1640);      // Wait for the display (1.64 ms)
    setCursor(0, 0);    // Reset the cursor position
}

void LCD1602::toggleBackLight(bool value){
    _backlight = value;
}
