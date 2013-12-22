//============================================================================
// Name        : Emulation.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CEmulation.h"

int main()
{
	try
	{
 		CEmulation * Emulator= new CEmulation();
		Emulator->EmulationBuilder();
		delete Emulator;
		return(EXIT_SUCCESS);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
