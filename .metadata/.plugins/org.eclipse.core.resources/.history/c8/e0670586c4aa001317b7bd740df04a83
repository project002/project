/*
 * CGUIGTK.cpp
 *
 *  Created on: Mar 9, 2014
 *      Author: root
 */

#include "CGUIGTK.h"

CGUIGTK::CGUIGTK() :mPackingBox(Gtk::manage(new Gtk::Box())),
					mMenuBar(Gtk::manage(new Gtk::MenuBar())),
					mStopButton(Gtk::manage(new Gtk::Button())),
					mImportXMLPath(""),
					mEmulation(NULL)
{
	set_border_width(10);
	set_default_size(530,320);
	set_title("Bandwidth Variance Emulation");

	mPackingBox->set_orientation(Gtk::ORIENTATION_VERTICAL);
	add(*mPackingBox);
	createMenuBar();

	mInst.set_label("chosse file->import to select the xml file\n and file->run to run\n so far we need to make the emulation run in a \n different thread so i can be able to stop it\n now it is possible via eclipse or terminal\n You know... the shitty way");
	mPackingBox->pack_start(mInst,Gtk::PACK_SHRINK);

	//make stop button
	mStopButton->set_label("Stop Emulation");
	mStopButton->signal_clicked().connect(sigc::mem_fun(*this,&CGUIGTK::stop_emulation));
	mPackingBox->pack_end(*mStopButton,Gtk::PACK_SHRINK);


	mPackingBox->show_all();
}

void CGUIGTK::createMenuBar()
{
	mPackingBox->pack_start(*mMenuBar,Gtk::PACK_SHRINK,0);

	//menu items top 0
	Gtk::MenuItem* menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
	mMenuBar->append(*menuitem_file);
	Gtk::Menu* file_sub_menu = Gtk::manage(new Gtk::Menu());
	menuitem_file->set_submenu(*file_sub_menu);
	//submenu items
	Gtk::MenuItem* import_file = Gtk::manage(new Gtk::MenuItem("Import..", true));
	//register imoprt file action
	import_file->signal_activate().connect(sigc::mem_fun(*this,&CGUIGTK::open_file_browser));
	file_sub_menu->append(*import_file);
	Gtk::MenuItem *run_emu = Gtk::manage(new Gtk::MenuItem("Run", true));
	//register run emulation action
	run_emu->signal_activate().connect(sigc::mem_fun(*this,&CGUIGTK::run_emulation));
	file_sub_menu->append(*run_emu);
}

void CGUIGTK::open_file_browser()
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
	      mImportXMLPath = dialog.get_filename();
	      break;
	    }
	    default:
	    {
	      //how??
	      break;
	    }
	  }
}

void CGUIGTK::stop_emulation()
{
	if (mEmulation!=NULL)
	{
		mEmulation->StopEmulation();
	}
}

void CGUIGTK::run_emulation()
{
	try
	{
		if (mEmulation==NULL)
		{
			mEmulation = new EmulationWrapper(mImportXMLPath);
			mEmulation->RunEmulation();
		}
	}
//	catch(CSocketNotReadyException & error)
//	{
//		//when the emulation existing it also throws this exception
////		cout << "trying again" << endl;
////		if (mEmulation==NULL) //try again
////		{
////			mEmulation = new EmulationWrapper(mImportXMLPath);
////			mEmulation->RunEmulation();
////		}
//	}
	catch(CException & error)
	{
		throw CException("Emulation Failed");
	}
}

CGUIGTK::~CGUIGTK()
{
	// TODO Auto-generated destructor stub
	delete mMenuBar;
	delete mPackingBox;
	delete mStopButton;
	if (mEmulation!=NULL) {delete mEmulation;}

}

