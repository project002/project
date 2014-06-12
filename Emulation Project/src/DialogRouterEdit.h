/*
 * DialogRouterEdit.h
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#ifndef DIALOGROUTEREDIT_H_
#define DIALOGROUTEREDIT_H_

#include <iostream>
#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menubar.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "SXMLStructs.h"

class DialogRouterEdit: public Gtk::Dialog
{
public:
	DialogRouterEdit(std::string title,Window & parent,bool modal);
	virtual ~DialogRouterEdit();

	RouterInformation getData();
	std::vector<std::string> getPhysicalConnections();

	void setFreeRouterNumber(int unsigned numid) {freeRouterNum = numid;}
	void setAvailablePhysicalConnections(std::vector<std::string> names) {phyCons = names;}
	void setInitialData(RouterInformation ri);
	void resetToNormal();
protected:
    void on_my_response(int response_id);
    void addPhysicalCon();

	Gtk::Grid mGrid;
	//controls
	//router number
	Gtk::Label lblRID;
	Glib::RefPtr<Gtk::Adjustment> adjRID;
	Gtk::SpinButton spnRID;
	//buffer size
	Gtk::Label lblBufSz;
	Glib::RefPtr<Gtk::Adjustment> adjBufSz;
	Gtk::SpinButton spnBufSz;
	//drop rate
	Gtk::Label lblDrop;
	Glib::RefPtr<Gtk::Adjustment> adjDrop;
	Gtk::SpinButton spnDrop;
	//Initial Buffer Size
	Gtk::Label lblInitBufSz;
	Glib::RefPtr<Gtk::Adjustment> adjInitBufSz;
	Gtk::SpinButton spnInitBufSz;
	//Fillage
	Gtk::Label lblFill;
	Glib::RefPtr<Gtk::Adjustment> adjFill;
	Gtk::SpinButton spnFill;
	//Auto Fill
	Gtk::Label lblAutoFill;
	Gtk::Entry entAutoFill;
	//Auto Drop Rate
	Gtk::Label lblAutoDrop;
	Gtk::Entry entAutoDrop;
	//physical connection
	Gtk::Label lblPhy;
	//box containers
	Gtk::Box boxCon;
	//connections list
	std::vector<Gtk::CheckButton*> chkConList;
private:
	RouterInformation mRData;
	//default free router number
	int unsigned freeRouterNum;
	//physical connections
	std::vector<std::string> phyCons;
	//chosen connection
	std::vector<std::string> chosenCons;

};

#endif /* DIALOGROUTEREDIT_H_ */

