/*
 * CShell.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: root
 */

#include "CShell.h"

CShell::CShell(char* xmlfile):
	mFilepath(std::string(xmlfile)),
	mEmulation(NULL),
	EmulationThread(NULL),
	InputThread(NULL),
	mEmulationRunning(false)
{
	// TODO Auto-generated constructor stub

}

CShell::~CShell()
{
	if (mEmulation != NULL) {delete mEmulation;}
}

void CShell::run()
{
	start_emulation();
}

void CShell::stop_emulation()
{
	if (mEmulation!=NULL)
	{
		mEmulationRunning = false;
		mEmulation->StopEmulation();
		delete mEmulation;
		mEmulation = NULL;
		mFilepath = "";
	}
}

void CShell::start_emulation()
{
	mEmulationRunning = true;
	EmulationThread = Glib::Thread::create(sigc::mem_fun(*this,&CShell::run_emulation),false);
	InputThread = Glib::Thread::create(sigc::mem_fun(*this,&CShell::input),false);
	loop();
}

void CShell::input()
{
	char exit = 'e';
	char inChar = 'a';
	while (mEmulationRunning)
	{
		std::cin >> inChar;
		if (inChar == exit)
		{
			stop_emulation();
		}
		else
		{
			std::cout << "::Enter 'e' to Exit::\n";
		}
	}
}

void CShell::loop()
{
	stringstream ss;
	stringstream ss1;

	while (mEmulationRunning)
	{
		system("clear");
		int pc = SDataController::getInstance().get_data(SDataController::PACKETPROCCES);
		int pd = SDataController::getInstance().get_data(SDataController::PACKETDROP);
		double lost = SDataController::getInstance().get_dropped_precentage();
		ss << "Packets Processed:" << pc << "\nPackets Dropped:" << pd << "\nLost Percentage:" << lost << "%";
		std::cout << ss.str() << std::endl;

		int ipc = SDataController::getInstance().get_data(SDataController::IPPACKET);
		int arpc = SDataController::getInstance().get_data(SDataController::ARPPACKET);
		int dhcpc = SDataController::getInstance().get_data(SDataController::DHCPPACKET);
		int avgseed = SDataController::getInstance().get_data(SDataController::AVGSPEED);
		ss1 << "\nIP Packets:" << ipc << "\nARP Packets:" << arpc << "\nDHCP Packets:" << dhcpc << "\nSpeed KB:" << avgseed;
		std::cout << ss1.str() << std::endl;

		std::cout << SDataController::getInstance().get_messages() << std::endl;


		sleep(1);
	}
}

void CShell::run_emulation()
{
	std::cout << "Run Started\n" ;
	try
	{
		if (mEmulation==NULL)
		{
			mEmulation = new EmulationWrapper(mFilepath);
			mEmulation->RunEmulation();
		}
	}
	catch(CException & error)
	{
		mEmulationRunning = false;
		delete mEmulation;
		mEmulation = NULL;
		stringstream ss;
		ss << "Exception On Run:\n" << error.what();
		std::cout << ss.str() << std::endl;
	}
}
