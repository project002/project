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
		return(0);
	}
	catch(CException & error)
	{

	}
}
