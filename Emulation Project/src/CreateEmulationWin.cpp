/*
 * CreateEmulationWin.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#include "CreateEmulationWin.h"
#define MAX_SEL 2
CreateEmulationWin::CreateEmulationWin():
			mPackingBox(Gtk::manage(new Gtk::Box())),
			addRouter(Gtk::manage(new Gtk::Button())),
			remRouter(Gtk::manage(new Gtk::Button())),
			remVCon(Gtk::manage(new Gtk::Button())),
			connectRouters(Gtk::manage(new Gtk::Button())),
			saveEmulation(Gtk::manage(new Gtk::Button())),
			openEmulation(Gtk::manage(new Gtk::Button())),
			HighEnd(Gtk::manage(new Gtk::CheckButton("Use High-End Performance"))),
			statusBar(Gtk::manage(new Gtk::Label("Status",Gtk::ALIGN_START))),
			XMLBuilder(NULL),
			tempFilename("tempfile.xml"),
			DlgRouterEdit(NULL)
{
	initPhysicalConnections();
	set_border_width(10);
	set_default_size(630,300);
	set_title("Emulation Creation");
	mGrid.set_column_homogeneous(true);
	mGrid.set_column_spacing(10);
	mGrid.set_row_spacing(10);
	mPackingBox->set_orientation(Gtk::ORIENTATION_VERTICAL);
	mPackingBox->pack_start(mGrid,true,true,10);
	add(*mPackingBox);


	//open old emulation
	openEmulation->set_label("Open Emulation");
	openEmulation->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::loadEmulation));
	mGrid.attach(*openEmulation,0,0,4,1);

	//save emulation routers
	saveEmulation->set_label("Save This Emulation");
	saveEmulation->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::saveEmulationFile));
	mGrid.attach(*saveEmulation,4,0,4,1);

	//save emulation routers
	HighEnd->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::toggleHighEnd));
	mGrid.attach(*HighEnd,8,0,4,1);

	//add router button
	addRouter->set_label("Add Router");
	addRouter->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::add_router_dialog));
	mGrid.attach(*addRouter,0,1,3,1);

	//remove router button
	remRouter->set_label("Remove Selected Routers");
	remRouter->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::remove_routers));
	mGrid.attach(*remRouter,3,1,3,1);

	//connect routers button
	connectRouters->set_label("Connect Selected Routers");
	connectRouters->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::connect_routers));
	mGrid.attach(*connectRouters,6,1,3,1);

	//remove connection button
	remVCon->set_label("Remove Connection");
	remVCon->signal_clicked().connect(sigc::mem_fun(*this,&CreateEmulationWin::remove_connection));
	mGrid.attach(*remVCon,9,1,3,1);


	mDrawing = new CEmulationDrawing();
	mDrawing->forceDragRefresh();
	mDrawing->signal_button_release_event().connect(sigc::mem_fun(*this,&CreateEmulationWin::handle_router_release));
	mGrid.attach(*mDrawing,0,2,12,60);

	statusBar->set_justify(Gtk::JUSTIFY_LEFT);
	mGrid.attach(*statusBar,0,62,12,1);


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
	if (XMLBuilder != NULL) {delete XMLBuilder; XMLBuilder = NULL;}
}

void CreateEmulationWin::init_win()
{
	if (XMLBuilder == NULL) {XMLBuilder = new CXMLBuilder(tempFilename);}
	XMLBuilder->Finalize(); //refreshes the file to a empty file
	mDrawing->resetDrawing(tempFilename);
	statusBar->set_text("");
	HighEnd->set_active(false);

}

bool CreateEmulationWin::handle_router_release(GdkEventButton* event)
{
	int unsigned rid = event->x_root;
	if (event->button == 3) {updateRouter(rid); return true;}
	stringstream ss;
	if (selectedRouters.size() >= MAX_SEL)
	{
		selectedRouters.clear();
		selectedRouters.push_back(rid);
	}
	else
	{
		RouterVec::iterator it= find(selectedRouters.begin(),selectedRouters.end(),rid);
		if (it == selectedRouters.end()) {selectedRouters.push_back(rid);}
	}
	ss << "Selected Router Number ";
	std::string sep = "";
	for (RouterVec::iterator i = selectedRouters.begin();
		 i!=selectedRouters.end();
		 ++i)
	{
		ss <<  sep << *i;
		sep = " And Router ";
	}
	statusBar->set_text(ss.str());
	//update the position of the router
	Point pos = mDrawing->get_router_position(rid);
	XMLBuilder->UpdatePosition(rid,pos.first,pos.second);

	return true;
}

void CreateEmulationWin::updateRouter(unsigned int rid)
{
	if (DlgRouterEdit == NULL) {DlgRouterEdit = new DialogRouterEdit("Router Details",*this,true);}
	bool ok = XMLBuilder->SetCurrentRouter(rid);
	if (ok)
	{
		RouterInformation t = XMLBuilder->GetCurrentRouterInformation();
		DlgRouterEdit->setAvailablePhysicalConnections(std::vector<std::string>());
		DlgRouterEdit->setInitialData(t); //also add reset to normal when exited
		int res = DlgRouterEdit->run();

		switch(res)
		{
			case Gtk::RESPONSE_CANCEL: DlgRouterEdit->hide(); DlgRouterEdit->resetToNormal(); return;
			case Gtk::RESPONSE_OK:
				t = DlgRouterEdit->getData();
				updateDrawingModule(t);
				XMLBuilder->EditCurrentRouterInformation(t);
				DlgRouterEdit->hide();
				DlgRouterEdit->resetToNormal();
				refresh();
				break;
		}
	}
}


//fix the virtual connection insertion in the xml builder
void CreateEmulationWin::connect_routers()
{
	if (selectedRouters.size() != MAX_SEL)
	{
		statusBar->set_text("Not Enough Routers To Connect");
		return;
	}
	bool ret = XMLBuilder->AddVirtualConnection(selectedRouters[0],selectedRouters[1]);
	if (ret)
	{
		statusBar->set_text("Routers Connected");
		refresh();
	}
	else
	{
		statusBar->set_text("Connection Failed");
	}
}

void CreateEmulationWin::remove_routers()
{
	if (selectedRouters.size() < 1)
	{
		statusBar->set_text("No Router/s Selected To Be Removed");
		return;
	}
	bool is_changed = false;
	stringstream ss;
	for (RouterVec::iterator i = selectedRouters.begin();
			 i!=selectedRouters.end();
			 ++i)
	{
		bool ret = XMLBuilder->RemoveRouter(*i);
		is_changed = is_changed || ret;
		if (!ret)
		{
			ss << "Can't Remove Router " << *i;
			statusBar->set_text(ss.str());
		}
		else
		{
			ss << "Removed Router " << *i;
			statusBar->set_text(ss.str());
		}
	}
	if (is_changed) {refresh();}
}

void CreateEmulationWin::toggleHighEnd()
{
	bool selected = HighEnd->get_active();
	XMLBuilder->SetThreaded(selected);
}

void CreateEmulationWin::remove_connection()
{
	if (selectedRouters.size() < 2)
	{
		statusBar->set_text("No Router/s Selected To Be Disconnected");
		return;
	}
	bool ret = XMLBuilder->RemoveVirtualConnection(selectedRouters.front(),selectedRouters.back());
	if (ret)
	{
		statusBar->set_text("Removed Connection");
		refresh();
	}
	else
	{
		statusBar->set_text("Connection doesn't exist");
	}
}

void CreateEmulationWin::updateDrawingModule(RouterInformation t)
{
	SDataController::getInstance().insertRouterData(t.sRouterNumber,SDataController::DROPRATE,t.sDropRate);
	SDataController::getInstance().insertRouterData(t.sRouterNumber,SDataController::FILLAGE,t.sFillage);
	SDataController::getInstance().insertRouterData(t.sRouterNumber,SDataController::BUFFERSIZE,t.sBufferSize);
	SDataController::getInstance().insertRouterData(t.sRouterNumber,SDataController::BUFFERUS,t.sUsedBufferSize);
}

void CreateEmulationWin::add_router_dialog()
{
	if (XMLBuilder == NULL || mDrawing == NULL) {return;}
	if (DlgRouterEdit == NULL) {DlgRouterEdit = new DialogRouterEdit("Router Details",*this,true);}
	PhysicalLabels availPCon = PhysicalLabels();
	for (PhysicalLabels::iterator it = pCons.begin();it!=pCons.end();++it)
	{
		if (XMLBuilder->IsInterfaceAvailable(*it)) {availPCon.push_back(*it);}
	}
	DlgRouterEdit->setAvailablePhysicalConnections(availPCon);
	int res = DlgRouterEdit->run();
	RouterInformation t;
	std::vector<std::string> chosenCon = std::vector<std::string>();
	switch(res)
	{
		case Gtk::RESPONSE_CANCEL: DlgRouterEdit->hide(); return;
		case Gtk::RESPONSE_OK:
			t = DlgRouterEdit->getData();
			updateDrawingModule(t);
			chosenCon  = DlgRouterEdit->getPhysicalConnections();
			DlgRouterEdit->hide();
			break;
	}


	bool added = XMLBuilder->AddRouter(t);
	if (added)
	{
		std::vector<std::string>::iterator it;
		//add physical connections if any
		for(it=chosenCon.begin();it!=chosenCon.end();++it)
		{
			XMLBuilder->AddPhysicalConnection((*it));
		}
		statusBar->set_text("New Router Added");
	}
	else
	{
		statusBar->set_text("Router Insert Failed. Make sure the ID is valid");
	}
	refresh();
}

void CreateEmulationWin::saveEmulationFile()
{
	std::string new_filename = "";
	Gtk::FileChooserDialog dialog("Save Emulation",
		  Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
		  new_filename = dialog.get_filename();
		  XMLBuilder->SetNewFilename(new_filename);
		  XMLBuilder->Finalize();
		  stringstream ss;
		  ss << "Emulation Saved as: " << new_filename;
		  statusBar->set_text(ss.str());
		  break;
		}
		default:
		{
		  //how??
		  break;
		}
	}
}

void CreateEmulationWin::loadEmulation()
{
	Gtk::FileChooserDialog dialog("Select the Emulation XML file",
				  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:

	Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
	filter_text->set_name("XML Emulation files");
	filter_text->add_mime_type("application/xml");
	dialog.add_filter(filter_text);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			loadEmulationToCreator(dialog.get_filename());
			refresh();
			break;
		}
		default:
		{
			//how??
			break;
		}
	}
}

void CreateEmulationWin::initPhysicalConnections()
{
	struct ifaddrs *ifaddr;
	struct ifaddrs *ifa;

	std::string name = "";

	if (getifaddrs(&ifaddr) == -1) {std::cout << "no available interface list\n"; return;}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL) {continue;}
		name = std::string(ifa->ifa_name);
		if (name.compare("lo")==0) {continue;}
//		std::cout << name << std::endl;
		pCons.push_back(name);
	}
	freeifaddrs(ifaddr);
	ifa = NULL;

	//remove duplicates
	sort( pCons.begin(), pCons.end() );
	pCons.erase( unique( pCons.begin(), pCons.end() ), pCons.end() );
}


void CreateEmulationWin::refresh()
{
	XMLBuilder->Finalize();
	mDrawing->resetDrawing(tempFilename);
	mDrawing->queue_draw();
}

void CreateEmulationWin::loadEmulationToCreator(std::string filename)
{
	typedef vector<RouterInformation> rInfo;
	typedef multimap<unsigned int,string> PCons;
	typedef multimap<unsigned int,unsigned int> VCons;
	//refresh the builder
	if (XMLBuilder != NULL) {delete XMLBuilder; XMLBuilder = new CXMLBuilder(tempFilename);}
	//parse the new file
	CXMLBasicParser parser;
	parser.ParseXML(filename);
	bool is_high = parser.isThreaded();
	XMLBuilder->SetThreaded(is_high);
	HighEnd->set_active(is_high);
	rInfo r = parser.GetRoutersInformation();
	PCons pc = parser.GetPhysicalConnections();
	//add routers
	for (rInfo::iterator it=r.begin();it!=r.end();++it)
	{
	  XMLBuilder->AddRouter(*it);
	  updateDrawingModule(*it);
	  std::pair<PCons::iterator,PCons::iterator> range = pc.equal_range(it->sRouterNumber);
	  for (PCons::iterator j=range.first;j!=range.second;++j) {XMLBuilder->AddPhysicalConnection(j->second);}
	  Point pos = parser.GetRouterPosition(it->sRouterNumber);
	  if (pos.first != 0 || pos.second!= 0){XMLBuilder->UpdatePosition(it->sRouterNumber,pos.first,pos.second);}

	}
	//add connections
	VCons vc = parser.GetVirtualConnections();
	for (VCons::iterator it=vc.begin();it!=vc.end();++it)
	{XMLBuilder->AddVirtualConnection(it->first,it->second);}
}
