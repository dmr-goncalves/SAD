#include	<p24FJ128GA010.h>
#include "LCD.h"

unsigned int	_loopsLCD;

void initLCD(){

	//Parallel Master Port (PMP) initialization 
	PMCON = 0x83BF; // Enable the PMP, long waits
	PMMODE = 0x3FF; //Master Mode 1
	PMAEN = 0x0001; //PMA0 enabled
	
	_loopsLCD = LCD_STARTUP; 
	Wait(_loopsLCD); //Waits for internal initialization

	_loopsLCD = LCD_F_INSTR;
 	PMDIN1 = 0b00111000; //Function Set: 8-bit interface, 2 lines, 5x7
	Wait(_loopsLCD);
	
	_loopsLCD = LCD_STARTUP;
	Wait(_loopsLCD); //Waits for internal initialization

	_loopsLCD = LCD_F_INSTR;
 	PMDIN1 = 0b00001100; //Display ON, Cursor & Blink OFF
	Wait(_loopsLCD);

	_loopsLCD = LCD_STARTUP;
	Wait(_loopsLCD); //Waits for internal initialization
	
	_loopsLCD = LCD_S_INSTR;
 	PMDIN1 = 0b00000001; // Clear the display
	Wait(_loopsLCD);
	
	_loopsLCD = LCD_STARTUP;
	Wait(_loopsLCD); //Waits for internal initialization
	
	_loopsLCD = LCD_S_INSTR;
 	PMDIN1 = 0b00000110; //Increment Cursor, No shift
	Wait(_loopsLCD);

	clearLCD();
	homeLCD();
}

void homeLCD(){
	_loopsLCD = LCD_S_INSTR;
	PMADDR = 0x0000; //Resets the Parallel Master Adress Register
	PMDIN1 = 0b00000010; //Return home
	Wait(_loopsLCD);
}

void homeLCDL1(){
	_loopsLCD = LCD_S_INSTR;
	PMADDR = 0x0000; //Resets the Parallel Master Adress Register
	PMDIN1 = 0b10000000; //Go to the LCD's first line
	Wait(_loopsLCD);
}

void homeLCDL2(){
	_loopsLCD = LCD_S_INSTR;
	PMADDR = 0x0000; //Resets the Parallel Master Adress Register
	PMDIN1 = 0b11000000; //Go to the LCD's second line
	Wait(_loopsLCD);
}

void clearLCD(){
	_loopsLCD = LCD_S_INSTR;
	PMADDR = 0x0000; //Resets the Parallel Master Adress Register
 	PMDIN1 = 0b00000001; // Clear the display
	Wait(_loopsLCD);
}

void putCharLCD(char c){
	_loopsLCD = LCD_F_INSTR;
	PMADDR = 0x0001; //Addr0
	PMDIN1 = c; //Puts character c in Input Register
	Wait(_loopsLCD);
	Nop(); //Just consume clocks without operations
	Nop();
	Nop();
	Nop();

}

void Wait(unsigned int w)
{
	while(w)
	w--;
}

void putStringLCD(char *s){
	while(*s){  //Loop until *s == '\0' -> the  end of the string
	putCharLCD(*s++);
	}
}
