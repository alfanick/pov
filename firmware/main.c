#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

#define CLOCK (1<<PB1)
#define DATA (1<<PB2)
#define BLANK (1<<PB5)
#define LATCH (1<<PB6)

#define BUFFER_SIZE 9000

int i;
int main(void)
{
    //UCSR3C =
    
    UCSR3B = (1<<RXEN3) | (1<<TXEN3); //0x18;      //reciever enable , transmitter enable
    UBRR3H = 0;
    UBRR3L = 8;
    
    // External memory interface enable
    MCUCR |= (1<<SRE);
    XMCRA = 0;
    //PC7..PC5 released pins
    XMCRB = 0;//|= (1<<XMM1)|(1<<XMM0);
    
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
  
    uint8_t *mem = malloc(9000);
  
    
    /*while (1) {
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '.';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\n';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\r';
    }*/
    
    /*
    uint8_t index, data = 0;
    // Fill memory incrementing values
    for(index = 0; index < BUFFER_SIZE; index++)
    {
        mem[index] = data++;
    }
    // Display memory block
    for(index = 0; index < BUFFER_SIZE; index++)
    {
     //PRINTF("%02X ",mem[index]);
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = mem[index];
        //if((index&0x0F) == 0x0F)
        //{
        //    PRINTF("\n");
        //}
    }*/
    
    while (1) {
        // dla kazdej diody 288 * 5 / 32
        //for (i = 0; i < 180; i++) {
            while (!( UCSR3A & (1<<RXC3)) );
            // zielono-rozowy
            SPDR = UDR3;
            // czekaj az sie wysle bufor
            while(!(SPSR & (1<<SPIF)));
        
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '.';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\n';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\r';
        //}

        // impuls latcha - wyswietlenie nowych
        PORTB |= LATCH; //| BLANK;
        PORTB &= ~LATCH;//~(LATCH | BLANK);
    }
    return 0;
}
