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

volatile uint8_t need_to_run, set_white;
volatile uint8_t j;
volatile uint8_t buffer[120];
uint8_t buffer_position = 0;
uint8_t i;

inline void send_led(uint8_t a) {
    SPDR = a;
    // czekaj az sie wysle bufor
    while(!(SPSR & (1<<SPIF)));
}

inline void commit() {
    PORTB |= LATCH | BLANK;
    PORTB &= ~(LATCH | BLANK);
}

inline uint16_t translate(uint8_t a) {
    return (a * 4095) / 255;
}

void send_translate() {

    static uint8_t mode = 1, to_send = 0;
    uint16_t tmp=0, i=0;
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

// Wcisniecie guziczka
ISR(PCINT1_vect) {
  /*  if (!(PINJ & (1<<PJ5))) {
        set_white = 1;
        for (i = 0; i < 120; i++) {
            //buffer[i] = (i%3 == j) ? 255 : 0;
            buffer[i] = 255;
        }
        send_translate();
        commit();
        _delay_ms(500);
        for (i = 0; i < 120; i++) {
            //buffer[i] = (i%3 == j) ? 255 : 0;
            buffer[i] = 0;
        }
        send_translate();
        commit();
        _delay_ms(500);
    }
    
    if ( !(PINJ & (1<<PJ6))) {
        set_white = 1;
        for (i = 0; i < 120; i++) {
            //buffer[i] = (i%3 == j) ? 255 : 0;
            buffer[i] = 255;
        }
        send_translate();
        commit();
    
   
    }*/
    
    /*} else {
        set_white = 0;
        for (i = 0; i < 120; i++) {
            //buffer[i] = (i%3 == j) ? 255 : 0;
            buffer[i] = 0;
        }
        send_translate();
        commit();
    }*/
}

// Odbior danych z uarta
ISR(USART3_RX_vect)
{
    char ReceivedByte;
    ReceivedByte = UDR3;
    UDR3 = ReceivedByte;
}

ISR(TIMER0_OVF_vect)
{
    /*
    for (i = 0; i < 120; i++) {
        //buffer[i] = (i%3 == j) ? 255 : 0;
        buffer[i] = (i%3 == 0) ? 255 : 0;
    }
    send_translate();
    commit();
    _delay_us(10);
    
    for (i = 0; i < 120; i++) {
        //buffer[i] = (i%3 == j) ? 255 : 0;
        buffer[i] = 0;
    }
    send_translate();
    commit();
    */
    //TCNT0 = 0xFF;
    //ms++;
}

unsigned long int millis(void)
{
    //return ms;
}

int main(void)
{
    sei();

    UCSR3B = (1<<RXEN3) | (1<<TXEN3);// | (1<<RXCIE3);// | (1<<TXCIE3); //0x18;      //reciever enable , transmitter enable
    UBRR3H = 0;
    UBRR3L = 8;

    // outputy + SS
    DDRB |= (CLOCK | DATA | BLANK | LATCH | (1<<PB0));
    // stan niski
    PORTB &= ~(CLOCK | DATA | BLANK | LATCH);

    // SPI
    SPCR = (1<<SPE) | (1<<MSTR); //| (1<<DORD);
    // max predkoscq
    SPSR = (1<<SPI2X);

    // blank na wysoki, diody gasna
    PORTB |= BLANK;
    // czekamy dwie sekundy
    //_delay_ms(2000);
    // blank na niski
    PORTB&= ~BLANK;

    DDRJ &= ~(1<<PJ5 | 1<<PJ6);
    
    PCICR |= (1<<PCIE1);
    PCMSK1 |= (1<<PCINT14 | 1<<PCINT15);

    TCCR0A = (1<<WGM01);
    TCCR0B = (1<<CS02);
    OCR0A = 1000;
    //TCNT0 = 0xFF; //16 clock cycles
    TIMSK0 = 1<<OCIE0A; //Set TOIE bit
    
    for (i = 0; i < 180; i++)
        send_led(0b11111111);
    commit();
    

    while (1) {
          /*  for (i = 0; i < 120; i++) {
                //buffer[i] = (i%3 == j) ? 255 : 0;
                ATOMIC_BLOCK(ATOMIC_FORCEON) {
                    buffer[i] = (set_white == 0) ? 255 : 0;
                }
            }
            send_translate();
            commit();
        
            j++;
            j%=3;
*/    }
    
    //buffer = //malloc(180 * sizeof(uint16_t));
    /*
    uint8_t to_send = 0;
    uint8_t mode = 1;
    uint8_t rec;
    uint8_t bp = 0;
    uint8_t k = 0;
    //while (1) {

    for (i = 0; i < 40; i++) {
        buffer[i*3] = 4095;//(i%3 == 0) ? 4095 : 0;//translate(255);
        buffer[i*3 + 1] = 0;
        buffer[i*3 + 2] = 0;
    }

    to_send = 0b00000000;
    mode = 1;
    for (i = 0; i < 120; i++) {
        // wysylka prawych 8 bitow
        if (mode) {
            send_led((uint8_t) (buffer[i] >> 4));

            to_send = ((buffer[i] << 12) >> 8);
        } else {
            to_send |= buffer[i] >> 8;

            send_led(to_send);

            send_led((buffer[i] << 8) >> 8);
        }

        mode = !mode;
    }

    commit();
     */
       // send_translate();

    //    _delay_ms(100);
    //}

    /*
    while (1) {

        while (!( UCSR3A & (1<<UDRE3)));
        //while(!(UCSR3A & (1<<RXC3)));
        buffer[bp] = translate(UDR3);//translate(rec);
        bp++;

        if (bp == 120) {

        }

        bp %= 120;

    }
    */
    /*
    j = 0;
    while (1) {
        //for (i = 0; i < 180; i++)
        //    send_led(0b11111111);
        //commit();

        //_delay_ms(500);

        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            if (int14) {
                for (i = 0; i < 180; i++)
                    send_led(0b11111111);
                commit();

                _delay_ms(500);

                for (i = 0; i < 180; i++)
                    send_led(0);
                commit();

                _delay_ms(500);

                int14 = 0;
            }
        }

        //for (i = 0; i < 180; i++)
        //    send_led(0);
        //commit();

        _delay_ms(500);

    }*/

    /*while(1) {
        j++;
        j %= 40;

        for (i = 0; i < 40; i++) {
            buffer[i*3+2] = buffer[i*3+1] = buffer[i*3] = (i==j) ? 4095 : 0;
            //buffer[i*3+1] =
        }

        for (i = 0; i < 120; i++) {
            // wysylka prawych 8 bitow
            if (mode) {
                send_led((uint8_t) (buffer[i] >> 4));

                to_send = ((buffer[i] << 12) >> 8);
            } else {
                to_send |= buffer[i] >> 8;

                send_led(to_send);

                send_led((buffer[i] << 8) >> 8);
            }

            mode = !mode;
        }

        commit();

     //   _delay_ms(50);
        //break;
    }*/

    //uint8_t *mem = malloc(9000);

    /*
    while (1) {
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '.';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\n';
        while ( !( UCSR3A & (1<<UDRE3)));
        UDR3 = '\r';
    }


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
    }

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
    }*/
    return 0;
}
