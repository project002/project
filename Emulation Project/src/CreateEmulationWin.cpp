/*
 * CreateEmulationWin.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#include "CreateEmulationWin.h"

CreateEmulationWin::CreateEmulationWin():
			mPackingBox(Gtk::manage(new Gtk::Box())),
			addRouter(Gtk::manage(new Gtk::Button())),
			remRouter(Gtk::manage(new Gtk::Button())),
			connectRouters(Gtk::manage(new Gtk::Button())),
			saveEmulation(Gtk::manage(new Gtk::Button())),
			XMLBuilder(NULL),
			tempFilename("tempfile.xml"),
			DlgRouterEdit(NULL)
{
	set_border_width(10);
	set_default_size(630,300);
	set_title("Emulation Creation");
	mGrid.set_column_homogeneous(true);
	mGrid.set_column_spacing(10);
	mGrid.set_row_spacing(10);
	mPackingBox->set_orientation(Gtk::ORIENTATION_VERTICAL);
	mPackingBox->pack_start(mGrid,true,true,10);
	add(*mPackingBox);

	//add router button
	addRouter->set_label("Add Router");
	addRouter->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::add_router_dialog));
	mGrid.add(*addRouter);

	//remove router button
	remRouter->set_label("Remove Selected Routers");
//	remRouter->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::remove_router));
	mGrid.add(*remRouter);

	//connect routers button
	connectRouters->set_label("Connect Selected Routers");
//	connectRouters->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::connect_router));
	mGrid.add(*connectRouters);

	//save emulation routers
	saveEmulation->set_label("Save This Emulation");
//	saveEmulation->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::saveEmulation));
	mGrid.add(*saveEmulation);

	mDrawing = new CEmulationDrawing();
	mDrawing->forceDragRefresh();
	mDrawing->signal_button_release_event().connect(sigc::mem_fun(*this,&CreateEmulationWin::handle_router_click));
	mGrid.attach(*mDrawing,0,1,4,60);

	//register events
	signal_hide().connect(sigc::mem_fun(*this,&CreateEmulationWin::reset_win));
	signal_show().connect(sigc::mem_fun(*this,&CreateEmulationWin::init_win));


	show_all();
}

CreateEmulationWin::~CreateEmulationWin()
{
	// TODO Auto-generated destructor stub
	if (mDrawing != NULL) {delete mDrawing;}
	if (XMLBuilder !=NULL) {delete XMLBuilder;}
	if (DlgRouterEdit !=NULL) {delete DlgRouterEdit;}
}

void CreateEmulationWin::reset_win()
{
	std::cout << "RESET shit when the window hides" << std::endl;
	if (XMLBuilder != NULL) {delete XMLBuilder; XMLBuilder = NULL;}
}

void CreateEmulationWin::init_win()
{
	std::cout << "INIT shit when the window show" << std::endl;
	if (XMLBuilder == NULL) {XMLBuilder = new CXMLBuilder(tempFilename);}
	XMLBuilder->Finalize(); //refreshes the file to a empty file
//	mDrawing->resetDrawing(tempFilename);

}

bool CreateEmulationWin::handle_router_click(GdkEventButton* event)
{

	std::cout << "clicked" << event->x_root << std::endl;
	return true;
}

void CreateEmulationWin::add_router_dialog()
{
	if (XMLBuilder == NULL || mDrawing == NULL) {return;}
	if (DlgRouterEdit == NULL) {DlgRouterEdit = new DialogRouterEdit("Router Details",*this,true);}
	DlgRouterEdit->run();
	std::cout << "this happens right after?\n";
	RouterInformation t;
	t.sBufferSize = 10;
	t.sDropRate = 0;

	bool added = XMLBuilder->AddRouter(t);
	if (added) {std::cout << "router added\n";} else {std::cout << "not added\n";}
	XMLBuilder->Finalize();
	mDrawing->resetDrawing(tempFilename);
	mDrawing->queue_draw();
}
