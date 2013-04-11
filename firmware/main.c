#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdlib.h>


#define CLOCK (1<<PB1)
#define DATA (1<<PB2)
#define BLANK (1<<PB5)
#define LATCH (1<<PB6)

#define NUM_TLC5947 5

#define ANGLE 40

volatile uint8_t buffer[120];

// aktualna animacja
volatile uint8_t current_animation = 0;


// wystawienie ledow na spi
inline void send_led(uint8_t a) {
    // przypisanie do spi
    SPDR = a;
    // czekaj az sie wysle bufor
    while(!(SPSR & (1<<SPIF)));
}

// zatrzasniecie latcha
inline void commit() {
    PORTB |= LATCH | BLANK;
    PORTB &= ~(LATCH | BLANK);
}

// przeliczenie na 12 bitów
inline uint16_t translate(uint8_t a) {
    return (a * 4095) / 255;
}

// konwersja 8bitów na paczki po 12bitów i z powrotem na 8bitów
void send_translate() {
    
    static uint8_t mode = 1, to_send = 0;
    uint16_t tmp=0;
    uint8_t i=0;
    
    for (i = 0; i < 120; i++) {
        tmp = translate(buffer[i]);
        // wysylka prawych 8 bitow
        if (mode) {
            send_led((uint8_t) (tmp >> 4));
            
            to_send = ((tmp << 12) >> 8);
        } else {
            to_send |= tmp >> 8;
            
            send_led(to_send);
            
            send_led((tmp << 8) >> 8);
        }
        mode = !mode;
    }
}


// przerwanie od guziczka
ISR(INT6_vect) {
    current_animation++;
    current_animation %= 3;
}


int main(void)
{
    // wlaczenie przerwan
    sei();
    
    // outputy + SS
    DDRB |= (CLOCK | DATA | BLANK | LATCH | (1<<PB0));
    // stan niski
    PORTB &= ~(CLOCK | DATA | BLANK | LATCH);
    
    // SPI
    SPCR = (1<<SPE) | (1<<MSTR);
    // max predkosc
    SPSR = (1<<SPI2X);
    
    // start (wyzerowanie pwmów)
    PORTB |= BLANK;
    PORTB &= ~BLANK;
    
    // wlaczenie przerwan
    EIMSK |= (1<<INT6);
    EICRB |= (1<<ISC60);

    uint8_t i, j=0;

    for (i = 0; i < 180; i++) {
        send_led(0);
    }
    commit();
    
    while (1) {
        for (i = 0; i < 120; i++) {
            buffer[i] = (i==j || (120-i)==j) ? 255 : 0;
        }
        
    
        send_translate();
        commit();
        
        j++;
        j%=120;
    }
    
    
    
    return 0;
}