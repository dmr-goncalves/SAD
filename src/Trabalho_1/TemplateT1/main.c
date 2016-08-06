	#include <p24fxxxx.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "UARTFuncs.h"
	#include "LCD.h"
	
	// Configuration Bits
	#ifdef __PIC24FJ64GA004__ //Defined by MPLAB when using 24FJ64GA004 device
	_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1 & IOL1WAY_ON) 
	_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & I2C1SEL_SEC)
	#else
	_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
	_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)
	#endif
	
	
	#define stringAlarmeUART "INCENDIO!\r\n"
	#define stringAlarmeAtivadoUART "ALARME ATIVADO!\r\n"
	#define stringEnterPasswordUART "Enter a 12 Character Password!\r\n"
	#define stringWrongPasswordUART "Password errada!\r\n"
	#define stringCorrectPasswordUART "Password aceite!\r\n"
	#define stringSystemClearUART "Sistema reposto.\r\n"
	#define stringAlarmeLCD "INCENDIO!"
	#define stringAlarmeAtivadoLCD "ALARME ATIVADO!"
	#define stringEnterPasswordLCD "Password!"
	#define stringWrongPasswordLCD "Password errada!"
	#define stringCorrectPasswordLCD "Password aceite!"
	#define stringSystemClearLCD "Sistema reposto."
	#define stringPassword "SAD15169394"
	#define stringSituacaoNormal "Situação  Normal"
	#define stringCarriageReturnLineFeed "\r\n"
	#define stringCarriageReturn "\r"
	#define stringLineFeed "\n"
	#define stringSpace " "
	
	void initADCPotentiometer(){ //A/D Converter
		AD1PCFG = 0xFFFB;		//AN2
		AD1CON1 = 0x0000; 		// SAMP bit = 0 ends sampling and starts converting
		AD1CHS = 0x0005; 		// Connect AN5 as CH0 input
		AD1CSSL = 0;
		AD1CON3 = 0x0002; // Manual Sample, Tad = 2 Tcy
		AD1CON2 = 0;
		AD1CON1bits.ADON = 1; // turn ADC ON
	}
	
	
	int main(void)
	{
		initADCPotentiometer(); //Initialize ADC for Potentiometer
		initUART(); //Initialize UART
		initLCD();  //Initialize LCD
		TRISDbits.TRISD6 = 1; //Botão
		TRISDbits.TRISD13 = 1; 		
		TRISAbits.TRISA0 = 0; //Led
		long int i = 0;
		char buff[100];			  	//Support buffer
		char password[100];          //Buffer that stores the password
		int potentiometerValue = 0; //Value of potentiometer
		int temperatureValue = 0;   //Value of temperature
		int alarm = 0;
		int enteredPassword = 0;
		long int flux = 20000;
		int eraseBuff = 0;
	
	
	
		while(1){
	
			/***************** Potentiometer Value **************/
				AD1PCFG = 0xFFDF;
				AD1CHS = 0x0005; 
				AD1CON1bits.SAMP = 1; 		// start sampling...
				for( i = 0 ; i < 200 ; i++){}; // Ensure the correct sampling time has elapsed before starting conversion.
				AD1CON1bits.SAMP = 0; 		// start Converting
				while (!AD1CON1bits.DONE); 	// conversion done?
				potentiometerValue = ADC1BUF0; // yes then get ADC value
				sprintf(buff,"P: %4d",potentiometerValue);
			/***************** Potentiometer Value **************/	
	
			/***************** Temperature Value ****************/
				AD1PCFG = 0xFFFB;
				AD1CHS = 0x0002; 
				AD1CON1bits.SAMP = 1; 		// start sampling...
				for( i = 0 ; i < 200 ; i++){}; // Ensure the correct sampling time has elapsed before starting conversion.
				AD1CON1bits.SAMP = 0; 		// start Converting
				while (!AD1CON1bits.DONE); 	// conversion done?
				temperatureValue = ADC1BUF0; // yes then get ADC value
				temperatureValue = convertTemp(temperatureValue, 1);
				sprintf(buff,"T: %4d",temperatureValue);
			/***************** Temperature Value ****************/
	
			/******************* UART Receiving ***************/
			if(U2STAbits.URXDA){ //If there is something to receive
				char c = getcharUART();
					if(c == 'T' || c == 't'){
						putcharUART(c);
						putstringUART("\n");
						sprintf(buff,"Temperature: %d\n", temperatureValue); //buff is the pointer to an array of char elements where the resulting C string is stored.
						putstringUART(buff); //Sends the temperature value to UART in normal state
						}
					
					if(c == 'P' || c == 'p'){
						putcharUART(c);
						putstringUART("\n");
						sprintf(buff,"Potentiometer: %d\n", potentiometerValue); //buff is the pointer to an array of char elements where the resulting C string is stored.
						putstringUART(buff); //Sends the potentiometer value to UART in normal state
					} 	
			}
			/******************* UART Receiving ***************/
			
			/******************* Set Sprinklers Flux ***************/
			if(potentiometerValue >= 0 && potentiometerValue <= 200){
				flux = 4000;
			}
			if(potentiometerValue > 200 && potentiometerValue <= 400){
				flux= 2350;
			}
			if(potentiometerValue > 600 && potentiometerValue <= 800){
				flux = 1000;
			}
			if(potentiometerValue > 800 && potentiometerValue <= 1023){
				flux = 750;
			}
			if(potentiometerValue > 1023){
				flux = 100;
			}
			/******************* Set Sprinklers Flux ***************/
	
	
			/******************* Set Led's Blinking if alarm is ON ***************/
			if(alarm == 1){
				PORTAbits.RA0 = 1; //LED On
				for( i = 0 ; i < flux ; i++){}; //Time that the LED stays on
				PORTAbits.RA0 = 0; //LED  Off
				for( i = 0 ; i < flux ; i++){}; //Time that LED stays off
			}
			/******************* Set Led's Blinking if alarm is ON ***************/
	
			/******************* Set Alarm State ***************/
			
			if(temperatureValue >= 8 && alarm == 0){
				alarm = 1;
				putstringUART(stringAlarmeAtivadoUART);
				putstringUART(stringAlarmeUART);
				clearLCD();
				homeLCDL1();
				putStringLCD(stringAlarmeAtivadoLCD);
				homeLCDL2();
				putStringLCD(stringAlarmeLCD);
			}
			else if(temperatureValue < 8 && alarm == 1){
					putstringUART(stringSystemClearUART);
					clearLCD();
					homeLCDL1();
					putStringLCD(stringSystemClearLCD);
					homeLCDL2();	
					sprintf(buff,"T: %d     P: %d", temperatureValue, potentiometerValue); //buff is the pointer to an array of char elements where the resulting C string is stored.
					putStringLCD(buff);
				}
			else if(temperatureValue < 7 && alarm == 0){
					clearLCD();
					homeLCDL1();
					putStringLCD(stringSituacaoNormal);
					homeLCDL2();	
					sprintf(buff,"T: %d     P: %d", temperatureValue, potentiometerValue); //buff is the pointer to an array of char elements where the resulting C string is stored.
					putStringLCD(buff);
				}
			/******************* Set Alarm State ***************/
			
			
			/******************* Reset Alarm ***************/
	
			if(alarm == 1 && !PORTDbits.RD6 && !PORTDbits.RD13){  
				enteredPassword = 0;
				putstringUART(stringEnterPasswordUART);
				
				while(!enteredPassword){
					if(U2STAbits.URXDA){ //If there is something to receive
						char c;
						int x = 0;
						
						while(x < 11){
							c = getcharUART();
							buff[x] = c;
							x++;
						}	
						buff[x] = '\0';					
							
				  			if (strcmp(buff, stringPassword) == 0){
								  putstringUART(stringCarriageReturnLineFeed);
								  putstringUART(stringCorrectPasswordUART);
								  putstringUART(stringSystemClearUART);			  
							      clearLCD();
								  homeLCDL1();
								  putStringLCD(stringCorrectPasswordLCD);
								  homeLCDL2();    
								  putStringLCD(stringSystemClearLCD);
								  alarm = 0;
							} else{
								  putstringUART(stringCarriageReturnLineFeed);
								  putstringUART(stringWrongPasswordUART);
							      putstringUART(stringAlarmeAtivadoUART);
								  clearLCD();
								  homeLCDL1();
								  putStringLCD(stringWrongPasswordLCD);
								  homeLCDL2();    
								  putStringLCD(stringAlarmeAtivadoLCD);
							}							
						  enteredPassword = 1;
					}	
				}
			}
			/******************* Reset Alarm ***************/
			
		}
	}
	
	int convertTemp(int temp, int degreesOrFahrenheit){
		float tempFinal = 0;
		tempFinal = (float) temp / 10;
		if(degreesOrFahrenheit){
			tempFinal = (tempFinal - 32.0) * (5.0/9.0);
		}
		return tempFinal;
}
