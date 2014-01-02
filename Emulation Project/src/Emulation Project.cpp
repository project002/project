//============================================================================
// Name        : Emulation.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CEmulation.h"
#define SETUP_XML_ARGUMENT_POSITION 1

/**
 * Verifying the XML was declared as an argument.
 *
 * @param argc
 * @param argv
 */
void EmulationParametersValidator(int argc, char *argv[])
{
	try
	{
		if (argc<2)
		{
			cout<< "Please enter a setup xml file name as first argument\n";
			exit(EXIT_FAILURE);
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void DisableNetworkManager()
{
	cout<< "Disabling Network Manager\n";
	int status = system("sudo service network-manager stop");
	if (status ==-1 || WEXITSTATUS(status)==-1)
	{
		throw CException("Can't call a system command to disable the network manager");
	}
}

int main(int argc, char *argv[])
{
	try
	{
		DisableNetworkManager();
 		CEmulation * Emulator= new CEmulation();
 		EmulationParametersValidator(argc,argv);
 		Emulator->EmulationBuilder(argv[SETUP_XML_ARGUMENT_POSITION]);
 		Emulator->StartEmulation();

		delete Emulator;
		return(EXIT_SUCCESS);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

