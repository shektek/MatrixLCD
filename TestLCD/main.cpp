#include "../MatrixLib/LCD.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/MatrixLib.lib")
#elif _RELEASE
#pragma comment(lib, "../Release/MatrixLib.lib")
#endif

int main(int argc, char **argv)
{
	MatrixLib::LCD test(6, 19200, nullptr);

	test.Start();

	unsigned char buf[256] = { 0, 32, 255 };
	unsigned char LED[256] = { 2 };
	test.SendCommand(MatrixLib::CommandCode::GPOOn, LED);
	test.SendCommand(MatrixLib::CommandCode::KeyBrightness, buf);
	test.SendCommand(MatrixLib::CommandCode::DisplayBacklightColour, buf);
	test.SendCommand(MatrixLib::CommandCode::ClearScreen, buf);
	test.SendCommand(MatrixLib::CommandCode::GPOOff, LED);

	test.DisplayText("Hello!");

	test.Close();

	return 0;
}