/*
 * EmulationWrapper.h
 *
 *  Created on: Mar 10, 2014
 *      Author: root
 */

#ifndef EMULATIONWRAPPER_H_
#define EMULATIONWRAPPER_H_
#include "CEmulation.h"
#include "SLogger.h"
#include "SDataController.h"
#include "DInitializationDefs.h"
#include "CSocketNotReadyException.h"
//#include <gtkmm.h>

class EmulationWrapper
{
public:
	/**
	 * @param XMLProp the path to the xml file containing the emulation properties
	 */
	EmulationWrapper(std::string XMLProp);

	/**
	 * runs the emulation with the given xmlfile
	 * @throws CSocketNotReadyException if the h/w socket is not ready (indicating we can try again)
	 * @throws CException if an unrecoverable error occurred
	 */
	void RunEmulation();

	void StopEmulation();

	virtual ~EmulationWrapper();

	void updateRouterFillage(unsigned int routerID,int fillage);
	void updateRouterDropRate(unsigned int routerID,int droprate);
	void refreshTables();
	void toggleRouter(int unsigned rID,const bool active);
private:
	std::string mXMLProperties; //file path to the XML properties path
	CEmulation * Emulator; //the emulation itself

	/**
	 * Verifying that the Setup XML file is valid and does exist else
	 * an exception will be thrown and program will be terminated
	 */
	void EmulationParametersValidator();

	/**
	 * Function receives the command to be executed and if it should be appended to
	 * the end of the system log file.
	 *
	 * @param cmd command to be executed
	 * @param append boolean if command should be appended to the system log file
	 * @return status of executed command
	 */
	int RunCmdRedirect(const char* cmd,bool append = true);

	/**
	 * Function receives the status of the executed system command and the string
	 * that should be printed in case of a failure.
	 *
	 * @param status system command status code
	 * @param toPrint message to print on failure.
	 * @throws CException if system command fails
	 */
	void HandleStatus(int status, string toPrint);

	/**
	 * Calling System to disabled the Network Manager service in order for the emulation
	 * to determine IP's subnet masks and more.
	 *
	 * If the Manager couldn't be stopped an exception will be throw
	 * and the emulation will be terminated.
	 */
	void DisableNetworkManager();

	/**
	 * Calling system to enable the Network Manager Service, to restore system
	 * functionality
	 */
	void EnableNetworkManager();

	/**
	 * the procedure to stop the emulation when any exception is thrown
	 * msg : the error massage to send to log
	 */
	void exceptionHalt(const char * msg);


};

#endif /* EMULATIONWRAPPER_H_ */
