
#include <stdio.h>
#include <ioavr.h>
#include <intrinsics.h>
//#include <avr/io.h>
//#include <delay.h>

#define MAX7219_DINDDR DDRD
#define MAX7219_DINPORT PORTD
#define MAX7219_DININPUT PD2
#define MAX7219_CLKDDR DDRD
#define MAX7219_CLKPORT PORTD
#define MAX7219_CLKINPUT PD3
#define MAX7219_LOADDDR DDRD
#define MAX7219_LOADPORT PORTD
#define MAX7219_LOADINPUT PD4

//setup number of chip attached to the board
#define MAX7219_ICNUMBER 4

//define registers
#define MAX7218_REGNOOP 0x00
#define MAX7218_REGDIGIT0 0x01
#define MAX7218_REGDIGIT1 0x02
#define MAX7218_REGDIGIT2 0x03
#define MAX7218_REGDIGIT3 0x04
#define MAX7218_REGDIGIT4 0x05
#define MAX7218_REGDIGIT5 0x06
#define MAX7218_REGDIGIT6 0x07
#define MAX7218_REGDIGIT7 0x08
#define MAX7218_REGDECODE 0x09
#define MAX7218_REGINTENSITY 0x0A
#define MAX7218_REGSCANLIMIT 0x0B
#define MAX7218_REGSHUTDOWN 0x0C
#define MAX7218_REGTEST 0x0F


/*
 * shift out a byte
 */
void max7219_shiftout(unsigned char bytedata) 
{
	unsigned char j = 0;

	//the shift is made in reverse order for this ic
	for(j=8; j>0; j--)
        {
		unsigned char val = (bytedata & (1<<(j-1)))>>(j-1);
		MAX7219_CLKPORT &= ~(1 << MAX7219_CLKINPUT); //set the serial-clock pin low
		if(val)
			MAX7219_DINPORT |= (1 << MAX7219_DININPUT);
		else
			MAX7219_DINPORT &= ~(1 << MAX7219_DININPUT);
		MAX7219_CLKPORT |= (1 << MAX7219_CLKINPUT); //set the serial-clock pin high
	}
}


/*
 * shift out data to a selected number
 */
void max7219_send(unsigned char icnum, unsigned char reg, unsigned char data) 
{
	unsigned char i = 0;

	if(icnum < MAX7219_ICNUMBER) 
        {
		MAX7219_LOADPORT &= ~(1<<MAX7219_LOADINPUT); //load down
                
		//send no op to following ic
		for(i=icnum; i<(MAX7219_ICNUMBER-1); i++) 
                {
			max7219_shiftout(MAX7218_REGNOOP); //no op reg
			max7219_shiftout(MAX7218_REGNOOP); //no op data
		}
		
                //send info to current ic
		max7219_shiftout(reg); //send reg
		max7219_shiftout(data); //send data
                
		//send no op to previous ic
		for(i=0; i<icnum; i++) {
			max7219_shiftout(MAX7218_REGNOOP); //no op reg
			max7219_shiftout(MAX7218_REGNOOP); //no op data
		}
                
		MAX7219_LOADPORT |= (1<<MAX7219_LOADINPUT); //load up
	}
}


/*
 * set shutdown for a selected ic
 */
void max7219_shutdown(unsigned char icnum, unsigned char value) 
{
	if(value == 0 || value == 1)
		max7219_send(icnum, MAX7218_REGSHUTDOWN, value);
}


/*
 * set brightness for a selected ic
 */
void max7219_intensity(unsigned char icnum, unsigned char value) 
{
	if(value < 16)
		max7219_send(icnum, MAX7218_REGINTENSITY, value);
}


/*
 * set test mode for a selected ic
 */
void max7219_test(unsigned char icnum, unsigned char value) 
{
	if(value == 0 || value == 1)
		max7219_send(icnum, MAX7218_REGTEST, value);
}


/*
 * set active output for a selected ic
 */
void max7219_scanlimit(unsigned char icnum, unsigned char value) 
{
	max7219_send(icnum, MAX7218_REGSCANLIMIT, value);
}


/*
 * set decode mode for a selected ic
 */
void max7219_decode(unsigned char icnum, unsigned char value) 
{
	max7219_send(icnum, MAX7218_REGDECODE, value);
}



/*
 * init the shift register
 */
void max7219_init() {
	//output
	MAX7219_DINDDR |= (1 << MAX7219_DININPUT);
	MAX7219_CLKDDR |= (1 << MAX7219_CLKINPUT);
	MAX7219_LOADDDR |= (1 << MAX7219_LOADINPUT);
	//low
	MAX7219_DINPORT &= ~(1 << MAX7219_DININPUT);
	MAX7219_CLKPORT &= ~(1 << MAX7219_CLKINPUT);
	MAX7219_LOADPORT &= ~(1 << MAX7219_LOADINPUT);
}

void initDevices()
{
  DDRB = 0x00;
  PORTB = 0xFF;
  TCCR0 |= (1 << CS01);
  TCCR1B |= (1 << CS12);
  TIMSK |= (1 << TOIE0) | (1 << TOIE1) | (1 << TOIE3);
}

int numTable[10][8] = {
  {0},
  { 2, 2, 2, 2, 2, 2, 2, 2 },
  {0},
  {0},
  {0},
  {0},
  {0},
  { 126, 2, 4, 8, 16, 16, 16, 0 },
  { 28, 34, 34, 28, 34, 34, 28, 0 },
  {0}
};
int tempNumTable[10][8];

void draw0(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 34);
  max7219_send(ic, 0x03, 34);
  max7219_send(ic, 0x04, 34);
  max7219_send(ic, 0x05, 34);
  max7219_send(ic, 0x06, 34);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw1(unsigned char ic)
{
  max7219_send(ic, 0x01, 2);
  max7219_send(ic, 0x02, 2);
  max7219_send(ic, 0x03, 2);
  max7219_send(ic, 0x04, 2);
  max7219_send(ic, 0x05, 2);
  max7219_send(ic, 0x06, 2);
  max7219_send(ic, 0x07, 2);
  max7219_send(ic, 0x08, 0);
}
void draw2(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 2);
  max7219_send(ic, 0x03, 2);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 32);
  max7219_send(ic, 0x06, 32);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw3(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 2);
  max7219_send(ic, 0x03, 2);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 2);
  max7219_send(ic, 0x06, 2);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw4(unsigned char ic)
{
  max7219_send(ic, 0x01, 34);
  max7219_send(ic, 0x02, 34);
  max7219_send(ic, 0x03, 34);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 2);
  max7219_send(ic, 0x06, 2);
  max7219_send(ic, 0x07, 2);
  max7219_send(ic, 0x08, 0);
}
void draw5(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 32);
  max7219_send(ic, 0x03, 32);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 2);
  max7219_send(ic, 0x06, 2);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw6(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 32);
  max7219_send(ic, 0x03, 32);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 34);
  max7219_send(ic, 0x06, 34);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw7(unsigned char ic)
{
  max7219_send(ic, 0x01, 126);
  max7219_send(ic, 0x02, 2);
  max7219_send(ic, 0x03, 4);
  max7219_send(ic, 0x04, 8);
  max7219_send(ic, 0x05, 16);
  max7219_send(ic, 0x06, 16);
  max7219_send(ic, 0x07, 16);
  max7219_send(ic, 0x08, 0);
}
void draw8(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 34);
  max7219_send(ic, 0x03, 34);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 34);
  max7219_send(ic, 0x06, 34);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}
void draw9(unsigned char ic)
{
  max7219_send(ic, 0x01, 28);
  max7219_send(ic, 0x02, 34);
  max7219_send(ic, 0x03, 34);
  max7219_send(ic, 0x04, 28);
  max7219_send(ic, 0x05, 2);
  max7219_send(ic, 0x06, 2);
  max7219_send(ic, 0x07, 28);
  max7219_send(ic, 0x08, 0);
}

void DrawNumber(unsigned char ic, int value)
{
  switch ( value )
  {
  case 0:
    draw0(ic);
    break;
  case 1:
    draw1(ic);
    break;
  case 2:
    draw2(ic);
    break;
  case 3:
    draw3(ic);
    break;
  case 4:
    draw4(ic);
    break;
  case 5:
    draw5(ic);
    break;
  case 6:
    draw6(ic);
    break;
  case 7:
    draw7(ic);
    break;
  case 8:
    draw8(ic);
    break;
  case 9:
    draw9(ic);
    break;
  }
}

void clearDisplay(unsigned char ic)
{
  for(int j = 0; j < 8; j++)
   max7219_send(0, j, 0);
}

void FormatDate();

// GLOBAL VARIABLES
int year[4] = { 2, 0, 0, 0 }; // yyyy
int date[4] = { 1, 0, 0, 1 }; // mm.dd
int time[6] = 0; // hh:mm:ss
int time_var = 0;
unsigned char blink_var = 0;
unsigned char click_available = 0;
unsigned char btn_clicked = 0;

// INTERRUPT
#pragma vector = TIMER1_OVF_vect
 __interrupt void timer1()
 {
        TCNT1 = 36735;
   
        // CHANGE
        // FOR TESTING INCREMENTS MINUTES
        time[2]++;    // increment seconds
        //date[3]++;
        
        time_var++; 
 }
#pragma vector = TIMER3_OVF_vect
 __interrupt void timer3()
 {
    TCNT3 = 1000;
    
    click_available = 1;
    btn_clicked = 0;
 }




int main(void) {
	max7219_init();
        initDevices();
        __enable_interrupt();
        
	unsigned char ic = 0;
        
        
	//init ic
	for(ic=0; ic<MAX7219_ICNUMBER; ic++) 
        {
		max7219_shutdown(ic, 1); //power on
		max7219_test(ic, 0); //test mode off
		max7219_decode(ic, 0); //use led matrix
		max7219_intensity(ic, 8); //intensity
		max7219_scanlimit(ic, 15);
	}
        
        int cur_digit = 5;
        int temp_time[6] = 0;
        
        // SET Hour/Minute
        for(;;)
        {
          
          // print time
          DrawNumber(3, temp_time[5]);
          DrawNumber(2, temp_time[4]);
          DrawNumber(1, temp_time[3]);
          DrawNumber(0, temp_time[2]);
              
          //if(click_available)
          //{
            if (!btn_clicked)
            {
                int tmp = ~PINB;
                // Increment 
                if(tmp & 0x01 )
                {
                  temp_time[cur_digit]++;
                  btn_clicked = 1;
                }
                // Decrement
                else if(tmp & 0x02 )
                {
                  temp_time[cur_digit]--;
                  btn_clicked = 1;
                }
                // Proceed
                else if(tmp & 0x04 )
                {
                  cur_digit--;
                  btn_clicked = 1;
                }
                  
            }
            else
            {
              if(!(~PINB & 0x01) && !(~PINB & 0x02) && !(~PINB & 0x04))
              {
                int i = 0;
                for(i = 0; i < 1000; i++) // to make some delay
                  ;
                btn_clicked = 0;
              }
            }
                
                
                // Formatting
                if(temp_time[5] > 2)
                  temp_time[5] = 0;
                if(temp_time[5] == 2 && temp_time[4] > 3)
                  temp_time[4] = 0;
                if(temp_time[4] > 9)
                  temp_time[4] = 0;
                if(temp_time[3] > 5)
                  temp_time[3] = 0;
                if(temp_time[2] > 9)
                  temp_time[2] = 0;
                unsigned char formatVar = 0;
                for(formatVar = 0; formatVar < 6; formatVar++)
                  if(temp_time[formatVar] < 0)
                    temp_time[formatVar] = 0;
                
                if(cur_digit < 2)
                  break;
                
                // click_available = 0;
          //}
            
	}
        unsigned char time_cpy = 0;
        for(time_cpy = 0; time_cpy < 6; time_cpy++)
          time[time_cpy] = temp_time[time_cpy];
        
	for(;;) 
        {
           FormatDate();
            
            // PRINTING
            if ( time_var < 2)
            {
                  // print time
                  DrawNumber(3, time[5]);
                  DrawNumber(2, time[4]);
                  DrawNumber(1, time[3]);
                  DrawNumber(0, time[2]);
            }
            if ( time_var >= 2 && time_var < 7)
            {
                  // print date
                  DrawNumber(3, date[0]);
                  DrawNumber(2, date[1]);
                  DrawNumber(1, date[2]);
                  DrawNumber(0, date[3]);
            }
            if ( time_var >= 7 && time_var < 10)
            {
                  // print year
                  DrawNumber(3, year[0]);
                  DrawNumber(2, year[1]);
                  DrawNumber(1, year[2]);
                  DrawNumber(0, year[3]);
            }
            // reset
            if (time_var >= 10)
              time_var = 0;
            
            
	}
}

void FormatDate()
{
            // FORMATTING DATE  
          // correct time
            if(time[2] > 9)
            {
              time[2] = 0;
              time[3] += 1;
            }
            if(time[3] > 5)
            {
              time[3] = 0;
              time[4] += 1;
            }
            if(time[4] > 9)
            {
              time[4] = 0;
              time[5] += 1;
            }
            if(time[5] == 2 && time[4] > 3)
            {
              time[4] = 0;
              time[5] = 0;
              date[3] += 1; // increment date
            }
            // correct date
            if(date[3] > 9)
            {
              date[3] = 0;
              date[2] += 1;
            }
            if(date[2] == 3 && date[3] > 1)
            {
              date[2] = 0;
              date[3] = 1;
              date[1] += 1;
            }
            if (date[1] > 9)
            {
              date[1] = 0;
              date[0] += 1;
            }
            if(date[0] == 1 && date[1] > 2)
            {
              date[1] = 1;
              date[0] = 0;
              year[3] += 1;
            }
            // correct year
            int year_cond_var = 0;
            for(year_cond_var = 3; year_cond_var >= 0; year_cond_var--)
            {
                if(year[year_cond_var] > 9)
                {
                  year[year_cond_var] = 0;
                  year[year_cond_var - 1] += 1;
                }
            }
}