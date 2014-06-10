//============================================================================
// Name        : Emulation.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include "EmulationWrapper.h"
#include "CGUIGTK.h"
#include <gtkmm/application.h>


/**
 * The main function of the program that initializes the loggers
 * and the operating system itself. Some preceding steps should be taken.
 * for more information view the README file attached to raanan's gmail
 * @param argc number of arguments provided for the emulation
 * @param argv the arguments themselves
 * @return Failure value if at some point an exception was thrown
 */
int main(int argc, char *argv[])
{
	try
	{
		Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc,argv,"org.NetworkEmulation");
		CGUIGTK window;
		app->run(window);

		return(EXIT_SUCCESS);
	}
	catch (CException & error)
	{
		return(EXIT_FAILURE);
	}
}

