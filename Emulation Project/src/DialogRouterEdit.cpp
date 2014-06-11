/*
 * DialogRouterEdit.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#include "DialogRouterEdit.h"
#define MAX_ROUTERS 16.0
#define MAX_BUF_SIZE 10000.0
DialogRouterEdit::DialogRouterEdit(std::string title,Window & parent,bool modal):
	Gtk::Dialog(title,parent,modal),
	lblRID("Router ID:"),
	adjRID(Gtk::Adjustment::create(0.,1.,MAX_ROUTERS,1.,1.,1.)),
	spnRID(adjRID),
	lblBufSz("Buffer Size:"),
	adjBufSz(Gtk::Adjustment::create(100.,0.,MAX_BUF_SIZE,10.,10.,10.)),
	spnBufSz(adjBufSz),
	lblDrop("Drop Rate %:"),
	adjDrop(Gtk::Adjustment::create(0.,0.,101.0,1.,1.,1.)),
	spnDrop(adjDrop),
	lblInitBufSz("Initial Buffer Fillage %:"),
	adjInitBufSz(Gtk::Adjustment::create(0.,0.,101.0,1.,1.,1.)),
	spnInitBufSz(adjInitBufSz),
	lblFill("Buffer Fillage %:"),
	adjFill(Gtk::Adjustment::create(0.,0.,101.0,1.,1.,1.)),
	spnFill(adjFill),
	lblAutoFill("Automatic Fillage Script:"),
	lblAutoDrop("Automatic Drop Rate Script:"),
	lblPhy("Here you can choose connection to available physical \nLAN connections:")

{

	set_border_width(10);
	mGrid.set_column_homogeneous(true);
	mGrid.set_column_spacing(10);
	mGrid.set_row_spacing(10);


	mGrid.attach(lblRID,0,0,2,1);
	mGrid.attach(spnRID,2,0,2,1);

	mGrid.attach(lblBufSz,0,1,2,1);
	mGrid.attach(spnBufSz,2,1,2,1);

	mGrid.attach(lblDrop,0,2,2,1);
	mGrid.attach(spnDrop,2,2,2,1);

	mGrid.attach(lblFill,0,3,2,1);
	mGrid.attach(spnFill,2,3,2,1);

	mGrid.attach(lblInitBufSz,0,4,2,1);
	mGrid.attach(spnInitBufSz,2,4,2,1);

	mGrid.attach(lblAutoFill,0,5,2,1);
	mGrid.attach(entAutoFill,2,5,2,1);

	mGrid.attach(lblAutoDrop,0,6,2,1);
	mGrid.attach(entAutoDrop,2,6,2,1);

	lblPhy.set_justify(Gtk::JUSTIFY_LEFT);
	mGrid.attach(lblPhy,0,7,4,1);
	mGrid.attach(boxCon,0,8,4,1);
	mGrid.show();

	Gtk::Box* con_area = get_content_area();
	con_area->add(mGrid);
	con_area->show_all();

	add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::OK,Gtk::RESPONSE_OK);

	signal_show().connect(sigc::mem_fun(*this,&DialogRouterEdit::addPhysicalCon));
}
DialogRouterEdit::~DialogRouterEdit()
{
}

void DialogRouterEdit::okay_clicked()
{
	hide();
}

RouterInformation DialogRouterEdit::getData()
{
	mRData.sRouterNumber = spnRID.get_value_as_int();
	mRData.sDropRate = spnDrop.get_value();
	mRData.sFillage = spnFill.get_value();
	mRData.sUsedBufferSize = spnInitBufSz.get_value_as_int();
	mRData.sBufferSize = spnBufSz.get_value_as_int();
	mRData.sDynamicFillage = entAutoFill.get_text();
	mRData.sDynamicDropRate = entAutoDrop.get_text();
	return mRData;
}

void DialogRouterEdit::on_my_response(int response_id)
{
	std::cout << response_id << std::endl;
}

std::vector<std::string> DialogRouterEdit::getPhysicalConnections()
{
	chosenCons.clear();
	std::vector<Gtk::CheckButton*>::iterator itr = chkConList.begin();
	for (;itr != chkConList.end();++itr)
	{

		if ((*itr)->get_active())
		{
			chosenCons.push_back((*itr)->get_label());
			std::cout << chosenCons.back() << "\n";
		}
	}
	return chosenCons;
}

void DialogRouterEdit::addPhysicalCon()
{
	//remove the last checkboxes
	std::vector<Gtk::CheckButton*>::iterator itr = chkConList.begin();
	for (;itr != chkConList.end();++itr)
	{
		boxCon.remove(**itr);
		delete (*itr);
	}
	chkConList.clear();
	std::vector<std::string>::iterator it = phyCons.begin();
	for (;it!=phyCons.end();++it)
	{
		std::cout << "added " << (*it) << std::endl;
		chkConList.push_back(new Gtk::CheckButton((*it)));
		boxCon.add(*(chkConList.back()));
	}

	boxCon.show_all();
}
