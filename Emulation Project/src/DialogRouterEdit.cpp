/*
 * DialogRouterEdit.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: root
 */

#include "DialogRouterEdit.h"

DialogRouterEdit::DialogRouterEdit(std::string title,Window & parent,bool modal):
	Gtk::Dialog(title,parent,modal),
	_BtnOkay(Gtk::manage(new Gtk::Button())),
	_BtnCancel(Gtk::manage(new Gtk::Button()))
{
	mGrid.set_column_homogeneous(true);
	mGrid.set_column_spacing(10);
	mGrid.set_row_spacing(10);

	add(mGrid);

	_BtnOkay->set_label("OK");
	mGrid.add(*_BtnOkay);

	_BtnCancel->set_label("Cancel");
	mGrid.add(*_BtnCancel);

	mGrid.show();
	add(*_BtnCancel);
	show_all();
}
DialogRouterEdit::~DialogRouterEdit()
{
}
