#ifndef MATRIX_LCD_H
#define MATRIX_LCD_H

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <map>
#include <string>

namespace MatrixLib
{
	const int CharHeight = 8;
	const int CharWidth = 5;
	const int LCDHeight = 4;
	const int LCDWidth = 20;

	const unsigned char EscapeChar = 254;
	
	enum CommandCode
	{
		BaudRate = 57,		//1200 = 83, 2400 = 41, 4800 = 207, 9600 = 103, 19200 = 51, 28800 = 34, 38400 = 25, 57600 = 16, 76800 = 12, 115200 = 8
		I2CAddress = 51,	//default = 80, otherwise use an even value
		Protocol = 160,		//0 = I2C, 1 = Serial (RS232/422, TTL, USB)
		BaudNonStd = 164,	//use a calculated value
		ClearScreen = 88,
		BootMsg = 64,		//max 80 chars
		//text behaviour
		AutoScrollOn = 81,	//moves text up a line when end of screen is reached
		AutoScrollOff = 82,	//turns off above behaviour
		LineWrapOn = 67,	//text falls into next line if over end of screen
		LineWrapOff = 68,	//skips a line when a row is full, sets write order to lines 1, 3, 2, 4
		//cursor
		SetCursor = 71,		//column, row : 1-20, 1-4
		ResetCursor = 72,	//puts cursor in top left
		CursorLeft = 76,	//move cursor one position left, obeying wrap settings
		CursorRight = 77,	//move cursor one position right, obeying wrap settings
		CursorUnderlineOn = 74,	//draws line under cursor position
		CursorUnderlineOff = 75,//turns off above behaviour
		CursorBlockOn = 83,	//draw a blinking block on the cursor
		CursorBlockOff = 84,//turns off above behaviour
		//characters
		SpecialCharCreate = 78,	//ID, data : 0-7, 8 bytes. Each byte should be set with bitwise operations, leading 3 bits are not used
		SpecialCharSave = 193,	//Bank, ID, data : As above, but saves it to the specified bank. 0 = startup characters, 1 = horizontal bars, 2 = vertical bars, 3 = medium digits, 4 = large digits
		SpecialCharLoad = 192,	//Bank : load the above digit bank
		SaveStartupChars = 194,	//ID, data : saves a custom character for startup/repeated use. Data defined as with SpecialCharCreate
		InitMedNumbers = 109,	//Loads medium number character bank
		PlaceMedNumbers = 111,	//Row, column, digit : places a 1x2 digit at the specified row, column
		InitLrgNumbers = 110,	//Loads large number character bank
		PlaceLrgNumbers = 35,	//column, digit : places a 3x4 digit at the given column position
		InitHorizontal = 104,	//load horizontal bar graph character bank
		PlaceHrzBar = 124,	//column, row, direction, length : 1-20, 1-4, 0 = right 1 = left, 0-100 (pixels)
		InitVrtNarrow = 115,//load narrow vertical bar characters. Narrow bars are 2 pixels wide
		InitVrtWide = 118,	//load wide vertical bar characters. Wide bars are 5 pixels wide
		PlaceVrtBar = 61,	//column, length : 1-20, 0-32 (pixels)
		//LED control
		GPOOn = 87,			//turns on the specified output, 1-6
		GPOOff = 86,		//turn off the specified output, 1-6
		GPOStartupState = 195,	//GPO, state : 1-6, 0 or 1
		//Keypad
		//AutoTransmitOn = 65,	//send key press to host automatically								function
		//AutoTransmitOff = 79,	//10 key presses will be stored until polled...						function
		//PollKeyPress = 38,	//Read last key. MSB will be set to 1 until all keys are read		function
		ClearKeys = 69,		//Clear the key press buffer
		SetDebounceTime = 85,	//time * 6.5 ms, delay between press and "Read" event
		SetRepeatMode = 126,	//1 for hold, 0 for typematic. Hold mode transmits on down, then a key up after release. Typematic repeats 5 times a second after a 1 second delay
		RepeatModeOff = 96,	//turns off repeat mode
		AssignKeyCodes = 213,	//Keydown, Keyup : use char value for code, 255 is unaltered. order is top shoulder, up, right, left, center, lower shoulder, down
		KeyBacklightOff = 155,
		KeyBrightness = 156,	//brightness, 0 - 255
		KeyBacklightBehaviour = 157,	//0-3, transmits the key. 8-11 ignores first press. 0 & 8 = no lighting change, 1 & 9 = keypad light on, 2 & 10 = display light on, 3 & 11 = both on
		//Display
		DisplayBacklightOn = 66,
		DisplayBacklightOff = 70,
		DisplayBrightness = 153,//brightness, 0 - 255
		DisplayBrightnessSave = 152,	//sets and then saves the brightness for startup. 0 - 255
		DisplayBacklightColour = 130,	//R G B : 0-255 for each.
		DisplayContrast = 80,	//0-255
		//Data security
		SetRemember = 147,	//1 or 0. Allows saving to non volatile memory. Can be slow!
		//SetDataLock			function
		//SetDataLockAndSave	function
		//Misc
		WriteCustomerData = 52,	//save user defined memory block for later. 16 bytes max
		ReadCustomerData = 53,	//load above
		VersionNumber = 54,	//convert to hexadecimal for major and minor version numbers
		ReadModuleType = 55
	};

	class LCD
	{
		private:
			int m_comport;
			int m_baud;
			unsigned long m_readTimeout;
			HANDLE m_port;
			DCB m_portAttributes;
			COMMTIMEOUTS m_timeoutAttributes;

			HANDLE m_listenThread;
			DWORD m_listenThreadId;

			HANDLE m_sendThread;
			DWORD m_sendThreadId;
			
			int(*m_InputCallback)(unsigned char key);

			std::string m_writeBuffer;

		public:
			LCD();
			LCD(const int port, const int baud, int (*InputCallback)(unsigned char key));
			LCD(const int port, const int baud, const unsigned long readTimeout, int (*InputCallback)(unsigned char key));
			~LCD();

			bool Start();
			void Close();
			bool DisplayText(std::string str, unsigned long msDelay = 0);	//if delay != 0, will scroll to fit next text part after ms delay
			bool SendCommand(CommandCode code, unsigned char data[256]);	//for now assume the user knows what data to give
	};
}

#endif
