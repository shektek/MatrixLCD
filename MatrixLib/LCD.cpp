#include "LCD.h"

namespace MatrixLib
{
	std::map<CommandCode, int> CodeInfo =
	{ { ClearScreen, 0 }, 
	{GPOOn, 1},
	{GPOOff, 1},
	{ DisplayBacklightColour, 3 } ,
	{KeyBrightness, 1} };

	LCD::LCD()
	{
		m_comport = 4;
		m_baud = 19200;
		m_readTimeout = 500;
	}

	LCD::LCD(const int port, const int baud, int(*InputCallback)(unsigned char key))
	{
		m_comport = port;
		m_baud = baud;
		m_readTimeout = 500;
		m_InputCallback = InputCallback;
	}

	LCD::LCD(const int port, const int baud, const unsigned long readTimeout, int(*InputCallback)(unsigned char key))
	{
		m_comport = port;
		m_baud = baud;
		m_readTimeout = readTimeout;
		m_InputCallback = InputCallback;
	}

	LCD::~LCD()
	{
		Close();
	}

	bool LCD::Start()
	{
		bool ret = false;

		char port[8] = { 0 };
		sprintf(port, "COM%d", m_comport);
		m_port = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (m_port != INVALID_HANDLE_VALUE)
		{
			GetCommState(m_port, &m_portAttributes);
			m_portAttributes.BaudRate = m_baud;	//generally 1:1
			m_portAttributes.ByteSize = 8;
			m_portAttributes.Parity = NOPARITY;
			m_portAttributes.StopBits = ONESTOPBIT;
			m_portAttributes.fRtsControl = RTS_CONTROL_DISABLE;
			SetCommState(m_port, &m_portAttributes);

			m_timeoutAttributes.ReadIntervalTimeout = m_readTimeout;
			SetCommTimeouts(m_port, &m_timeoutAttributes);

			//start the send/receive threads

			ret = true;
		}

		return ret;
	}

	void LCD::Close()
	{
		CloseHandle(m_port);
		m_port = INVALID_HANDLE_VALUE;
		//shut down any running threads...
	}

	bool LCD::DisplayText(std::string str, unsigned long delayMS)
	{
		unsigned long written = 0;
		WriteFile(m_port, str.c_str(), str.size(), &written, NULL);
		return (written != 0);
	}

	bool LCD::SendCommand(CommandCode code, unsigned char data[256])
	{
		unsigned char sendBuf[258] = { 0 };
		sendBuf[0] = EscapeChar;
		sendBuf[1] = (unsigned char)code;
		memcpy(&sendBuf[2], data, 255);
		unsigned long written;

		int bytesToWrite = CodeInfo[code] + 2;

		WriteFile(m_port, sendBuf, bytesToWrite, &written, NULL);

		return (written != 0);
	}
}