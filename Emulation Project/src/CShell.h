/*
 * CShell.h
 *
 *  Created on: Jun 16, 2014
 *      Author: root
 */

#ifndef CSHELL_H_
#define CSHELL_H_
#include <gtkmm.h>
#include <iostream>
#include <string>
#include "EmulationWrapper.h"
#include "SDataController.h"

class CShell
{
public:
	CShell(char* filepath);
	virtual ~CShell();
	void run();
protected:
	void stop_emulation();
	void start_emulation();
	void loop();
	void input();
private:
	std::string mFilepath;
	EmulationWrapper* mEmulation;
	Glib::Thread* EmulationThread;
	Glib::Thread* InputThread;
	boost::signals2::mutex insertMTX;
	bool mEmulationRunning;

	void run_emulation();
};

#endif /* CSHELL_H_ */
