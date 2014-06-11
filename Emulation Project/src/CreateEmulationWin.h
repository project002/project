/*
 * CreateEmulationWin.h
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#ifndef CREATEEMULATIONWIN_H_
#define CREATEEMULATIONWIN_H_
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/messagedialog.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "CEmulationDrawing.h"
#include "CXMLBuilder.h"
#include "DialogRouterEdit.h"
#include "SDataController.h"

class CreateEmulationWin: public Gtk::Window
{
public:
	CreateEmulationWin();
	virtual ~CreateEmulationWin();
protected:
	//events
	void reset_win();
	void init_win();
	//mouse button clicks on drawing area
	bool handle_router_click(GdkEventButton* event);
	//button clicks
	void add_router_dialog();

	Gtk::Box* mPackingBox;
	Gtk::Grid mGrid;
	//draw area
	CEmulationDrawing *mDrawing;
	//buttons
	Gtk::Button* addRouter;
	Gtk::Button* remRouter;
	Gtk::Button* connectRouters;
	Gtk::Button* saveEmulation;
	//xml creator
	CXMLBuilder* XMLBuilder;
private:
	std::string tempFilename;

	DialogRouterEdit* DlgRouterEdit;

	std::vector<std::string> pCons;

	void initPhysicalConnections();
};

#endif /* CREATEEMULATIONWIN_H_ */
