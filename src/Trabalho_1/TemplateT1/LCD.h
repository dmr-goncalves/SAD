
// Looptime is 1 us per loop count
// Define the startup time for the LCD due to its internal initialization
// typically > 40ms
#define	LCD_STARTUP		50000

// Define a fast instruction execution time in terms of loop time
// typically > 43us
#define	LCD_F_INSTR		50

// Define a slow instruction execution time in terms of loop time
// typically > 1.53ms
#define	LCD_S_INSTR		1600


void initLCD();
void homeLCD();
void homeLCDL1();	
void homeLCDL2();
void clearLCD();
void putCharLCD(char c);
void Wait(unsigned int w);
void putStringLCD(char *s);

