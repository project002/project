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
#include "CXMLBasicParser.h"
#include "DialogRouterEdit.h"
#include "SDataController.h"

typedef std::vector<unsigned int> RouterVec;

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
	bool handle_router_release(GdkEventButton* event);
	//button clicks
	void add_router_dialog();
	void connect_routers();
	void remove_routers();
	void saveEmulationFile();
	void toggleHighEnd();
	void loadEmulation();
	void updateRouter(int unsigned rid);

	Gtk::Box* mPackingBox;
	Gtk::Grid mGrid;
	//draw area
	CEmulationDrawing *mDrawing;
	//buttons
	Gtk::Button* addRouter;
	Gtk::Button* remRouter;
	Gtk::Button* connectRouters;
	Gtk::Button* saveEmulation;
	Gtk::Button* openEmulation;
	//checkbox
	Gtk::CheckButton* HighEnd;
	//status bar
	Gtk::Label* statusBar;
	//xml creator
	CXMLBuilder* XMLBuilder;
private:
	std::string tempFilename;

	DialogRouterEdit* DlgRouterEdit;

	std::vector<std::string> pCons;

	RouterVec selectedRouters;

	void initPhysicalConnections();

	void refresh();

	void loadEmulationToCreator(std::string filename);

	void updateDrawingModule(RouterInformation t);
};

#endif /* CREATEEMULATIONWIN_H_ */
