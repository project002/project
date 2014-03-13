/*
 * CGUIGTK.h
 *
 *  Created on: Mar 9, 2014
 *      Author: root
 */

#ifndef CGUIGTK_H_
#define CGUIGTK_H_
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "EmulationWrapper.h"
#include "CSocketNotReadyException.h"

class CGUIGTK : public Gtk::Window
{
public:
	CGUIGTK();
	virtual ~CGUIGTK();
protected:
	//single handlers
	void open_file_browser();
	void start_emulation_quick();
	void run_emulation();
	void stop_emulation();
	void create_menu_bar();
	void start_emulation_thread();

	//members
	Gtk::Box* mPackingBox;
	Gtk::MenuBar* mMenuBar;
	Gtk::Button* mStopButton;
	Gtk::Button* mQuickStartButton;
	Gtk::Label mInst;
private:
	std::string mImportXMLPath;
	EmulationWrapper* mEmulation;
	bool mEmulationRunning;
	Glib::Thread* EmulationThread;

};

#endif /* CGUIGTK_H_ */
