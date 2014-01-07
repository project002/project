//============================================================================
// Name        : Emulation.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "CEmulation.h"
#define SETUP_XML_ARGUMENT_POSITION 1
#define MINIMUM_NUMBER_OF_ARGUMENTS 2
#define STATUS_FAILURE -1
#define ERROR_MSG_XML_FILE_ARGUMENT_MISSING "Please enter a setup xml file name as first argument\n"
#define ERROR_MSG_DISABLING_NETWORK_MANAGER "Can't disable the Network Manager"
#define STOP_NETWORK_MANAGER_COMMAND "sudo service network-manager stop"

/**
 * Verifying that the Setup XML file was provided while lunching the
 * emulation.
 * If no file was provided an exception will be thrown and program will
 * be terminated
 *
 * @param argc number of arguments provided for the emulation
 * @param argv the arguments themselves
 */
void EmulationParametersValidator(int argc, char *argv[])
{
	try
	{
		if (argc<MINIMUM_NUMBER_OF_ARGUMENTS)
		{
			throw CException(ERROR_MSG_XML_FILE_ARGUMENT_MISSING);
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

/**
 * Calling System to disabled the Network Manager service in order for the emulation
 * to determine IP's subnet masks and more.
 *
 * If the Manager couldn't be stopped an exception will be throw
 * and the emulation will be terminated.
 */
void DisableNetworkManager()
{
	try
	{
		int status = system(STOP_NETWORK_MANAGER_COMMAND);
		if (status == STATUS_FAILURE || WEXITSTATUS(status) == STATUS_FAILURE)
		{
			throw CException(ERROR_MSG_DISABLING_NETWORK_MANAGER);
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

/**
 *
 * @param argc number of arguments provided for the emulation
 * @param argv the arguments themselves
 * @return Failure value if at some point an exception was thrown
 */
int main(int argc, char *argv[])
{
	try
	{
		DisableNetworkManager();
 		EmulationParametersValidator(argc,argv);
 		CEmulation * Emulator= new CEmulation();
 		Emulator->EmulationBuilder(argv[SETUP_XML_ARGUMENT_POSITION]);
 		Emulator->StartEmulation();

		delete Emulator;
		return(EXIT_SUCCESS);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		return(EXIT_FAILURE);
	}
}

