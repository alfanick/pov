#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CLOCK (1<<PB1)
#define DATA (1<<PB2)
#define BLANK (1<<PB5)
#define LATCH (1<<PB6)

int i;
int main(void)
{
    // outputy + SS
    DDRB |= (CLOCK | DATA | BLANK | LATCH | (1<<PB0));
    // stan niski
    PORTB &= ~(CLOCK | DATA | BLANK | LATCH);
    
    // SPI, bity odwrotnie
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<DORD);
    // max predkosc
    SPSR = (1<<SPI2X);
    
    // blank na wysoki, diody gasna
    PORTB |= BLANK;
    // czekamy dwie sekundy
    _delay_ms(2000);
    // blank na niski
    PORTB &= ~BLANK;
    
    while (1) {
        // dla kazdej diody 288 * 5 / 32
        for (i = 0; i < 180; i++) {
            // zielono-rozowy
            SPDR = 0b00001111;
            // czekaj az sie wysle bufor
            while(!(SPSR & (1<<SPIF)));
        }
        
        // impuls latcha - wyswietlenie nowych
        PORTB |= LATCH; //| BLANK;
        PORTB &= ~LATCH;//~(LATCH | BLANK);
    }
    return 0;
}