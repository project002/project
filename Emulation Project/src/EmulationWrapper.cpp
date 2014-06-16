/*
 * EmulationWrapper.cpp
 *
 *  Created on: Mar 10, 2014
 *      Author: root
 */

#include "EmulationWrapper.h"

EmulationWrapper::EmulationWrapper(std::string XMLProp) :mXMLProperties(XMLProp),Emulator(NULL)
{

}

EmulationWrapper::~EmulationWrapper()
{
}

int EmulationWrapper::RunCmdRedirect(const char* cmd, bool append)
{
	sleep(SYSTEM_COMMANDS_TIME_TO_COMPLETE);
	char new_cmd[1024];
	if (append)
	{
		sprintf(new_cmd,"%s >> %s 2>&1",cmd,REDIRECT_SYSTEM_FILE);
	}
	else
	{
		sprintf(new_cmd,"%s > %s  2>&1",cmd,REDIRECT_SYSTEM_FILE);
	}
	return system(new_cmd);
}

void EmulationWrapper::HandleStatus(int status, string toPrint)
{
	try
	{
		if (status == STATUS_FAILURE || WEXITSTATUS(status) == STATUS_FAILURE)
		{
			throw CException(toPrint);
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void EmulationWrapper::DisableNetworkManager()
{
	try
	{
		HandleStatus(RunCmdRedirect(STOP_NETWORK_MANAGER_COMMAND,false),ERROR_MSG_DISABLING_NETWORK_MANAGER);

		HandleStatus(RunCmdRedirect(STOP_IP_FORWARDING),ERROR_MSG_DISABLING_IP_FORWARDING);

		HandleStatus(RunCmdRedirect(STOP_ICMP_RESPONSE),ERROR_MSG_DISABLING_ICMP_RESPONSE);

		HandleStatus(RunCmdRedirect(ENABLE_FIREWALL),ERROR_FW);

		HandleStatus(RunCmdRedirect(STOP_ALL_INCOMING_PACKETS),ERROR_DISABLING_PACKETS_TRAFFIC);

		HandleStatus(RunCmdRedirect(STOP_ALL_OUTGOING_PACKETS),ERROR_DISABLING_PACKETS_TRAFFIC);

	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void EmulationWrapper::EmulationParametersValidator()
{
	try
	{
		if (mXMLProperties.compare("")==0)
		{
			throw CException(ERROR_MSG_XML_FILE_ARGUMENT_MISSING);
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void EmulationWrapper::EnableNetworkManager()
{
	try
	{
		HandleStatus(RunCmdRedirect(START_NETWORK_MANAGER_COMMAND),ERROR_MSG_ENABLING_NETWORK_MANAGER);
		HandleStatus(RunCmdRedirect(DISABLE_FIREWALL),ERROR_DISABLING_FW);
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}


void EmulationWrapper::StopEmulation()
{
	if (Emulator==NULL) {return;}
	Emulator->StopEmulation();
	SLogger::getInstance().Log("Emulation exiting");
	delete Emulator;
	SLogger::getInstance().Log("Enabling Networking");
	EnableNetworkManager();
	SLogger::getInstance().Log("Destroying Controller Thread");
	SDataController::getInstance().destroy();
	SLogger::getInstance().Log("Destroying Logger and exiting program");
	SLogger::getInstance().DestroyLogger();
	SReport::getInstance().DestroyReport();
}

void EmulationWrapper::RunEmulation()
{
	try
	{
		SDataController::getInstance().init();
		SLogger::getInstance().InitLogger();
		SReport::getInstance().InitReport();
		SLogger::getInstance().Log("Disabling Linux Networking");
		DisableNetworkManager();
		SLogger::getInstance().Log("Validating XML file is present");
		EmulationParametersValidator();
		Emulator= new CEmulation();
		SLogger::getInstance().Log("Building the emulation");
		Emulator->EmulationBuilder(mXMLProperties.c_str());
		SLogger::getInstance().Log("Starting the emulation");
		Emulator->StartEmulation();
	}
	catch (CDHCPMsgTypeException & error)
	{
		const char* msg = error.what();
		exceptionHalt(msg);
		std::cout << "thrown an dhcp exception" << std::endl;
		throw new CException(msg);
	}
	catch (CException & error)
	{
		exceptionHalt(error.what());
		throw error;
	}
}

void EmulationWrapper::updateRouterFillage(unsigned int routerID, int fillage)
{
	if (Emulator!=NULL) {Emulator->updateFillage(routerID,fillage);}
}

void EmulationWrapper::updateRouterDropRate(unsigned int routerID, int dropRate)
{
	if (Emulator!=NULL) {Emulator->updateDropRate(routerID,dropRate);}
}

void EmulationWrapper::refreshTables()
{
	if (Emulator==NULL) {return;}
	Emulator->EmptyRoutingTables();
}

void EmulationWrapper::exceptionHalt(const char * msg)
{
	if (Emulator!=NULL) {delete Emulator;}
	SLogger::getInstance().Log(msg);
	SLogger::getInstance().Log(__PRETTY_FUNCTION__);
	SLogger::getInstance().DestroyLogger();
	SReport::getInstance().DestroyReport();
	EnableNetworkManager();
}
