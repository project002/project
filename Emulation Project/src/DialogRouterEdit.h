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

	RouterInformation getData() {return mRData;}
protected:
	Gtk::Grid mGrid;
	//buttons
	Gtk::Button* _BtnOkay;
	Gtk::Button* _BtnCancel;
private:
	RouterInformation mRData;
};

#endif /* DIALOGROUTEREDIT_H_ */

