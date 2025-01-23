// Simon Project

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define rounds 5 // number of rounds the game will play

void init() {
    DDRD = DDRD | 0x01; // set pd0 to output
    DDRD = DDRD &~ 0x02; // set pd1 to input
    DDRD = DDRD | 0x04; // set pd2 to output
    DDRD = DDRD &~ 0x08; // set pd3 to input
    DDRD = DDRD | 0x10; // set pd4 to output
    DDRD = DDRD &~ 0x20; // set pd5 to input
    DDRD = DDRD | 0x40; // set pd6 to output
    DDRD = DDRD &~ 0x80; // set pd7 to input
    
    DDRB = DDRB | 0x02; // set pb1 to output
        
    // T1 to CTC
    TCCR1A = TCCR1A &~ 0x01; // wgm0 = 0
    TCCR1A = TCCR1A &~ 0x02; // wgm1 = 0
    TCCR1B = TCCR1B | 0x08; // wgm2 = 1
    TCCR1B = TCCR1B &~ 0x10; // wgm3 = 0
    
    // set T1 prescaler to 256
    TCCR1B = TCCR1B | 0x04; // cs02 = 1
    TCCR1B = TCCR1B &~ 0x02; // cs01 = 0
    TCCR1B = TCCR1B &~ 0x01; // cs00 = 0
    
    // sets T1 to clear on match and set to bottom
    TCCR1A = TCCR1A &~ 0x80; // COM1A1 = 0
    TCCR1A = TCCR1A | 0x40; // COM1A0 = 1
        
    PORTD = 0;
}

int button() {
    int blue, green, yellow, red;
    int value = -1;
    
    blue = PIND & (0x02);
    if(blue != 0) {
        value = 0;
    }
    green = PIND & (0x08);
    if(green != 0) {
        value = 1;
    }
    yellow = PIND & (0x20);
    if(yellow != 0) {
        value = 2;
    }
    red = PIND & (0x80);
    if(red != 0) {
        value = 3;
    }
    return value;
}

void delay() {
    for (unsigned long i = 0; i < 64000; i++) {}
}
void timerReset() {
    TCNT1H = 0 ;
    TCNT1L = 0 ;
}


void playBlue() {
    PORTD = PORTD | 0x01;
    timerReset();
    OCR1A = 250;
    delay();
}
void playGreen() {
    PORTD = PORTD | 0x04;
    timerReset();
    OCR1A = 200;
    delay();
}
void playYellow() {
    PORTD = PORTD | 0x10;
    timerReset();
    OCR1A = 150;
    delay();
}
void playRed() {
    PORTD = PORTD | 0x40;
    timerReset();
    OCR1A = 100;
    delay();
}
void playNone() {
    PORTD = 0;
    timerReset();
    OCR1A = 0;
    delay();
}

void play(int x) {
    if (x == 0) {
        playBlue();
    }
    else if (x == 1) {
        playGreen();
    }
    else if (x == 2) {
        playYellow();
    }
    else if (x == 3) {
        playRed();
    }
    else {
        playNone();
    }
}

void gameLoss() {
    // game loss sequence
    for (int i = 0; i < 3; i++) {
        playRed();
        delay();
        playNone();
    }
}

void gameWin() {
    for (int i = 0; i < 3; i++) {
        playGreen();
        delay();
        playNone();
    }
}

int main(void) {
    init();
    
    while(1) {
    int val = -1;
    int loss = 0, count = 0;
    
    // wait for press
    while (button() == -1) {
        count++;
        delay();
        }
    while (button() != -1) {} // wait for release
        
    // initialize the seq array with the random values
    int seq[rounds];
    
    for (int i = 0; i < rounds; i++) {
        int x = (count + rand()) % 4;
        seq[i] = x;
    }
    
    // after a button is pressed, initialize the game loop
    for (int j = 1; j <= rounds && loss == 0; j++) {
        // loop to play the sequence of lights
        for (int k = 0; k < j; k++) {
            play(seq[k]);
            delay();
            playNone();
        }
        
        for (int l = 0; l < j && loss == 0; l++) {
            while(button() == -1) {} // wait for press
            val = button();
            play(val);
            while(button() != -1) {} // wait for release
            playNone();
            // test if button was right
            if (seq[l] != val) {
                gameLoss();
                loss = 1;
            }
            else if (l == (rounds - 1) && seq[rounds - 1] == val) {
                gameWin();
            }
            else {
                delay();
                continue;
            }
        }
    }
    }
}
