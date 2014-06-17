/*
 * RouterInfoWidget.h
 *
 *  Created on: Jun 1, 2014
 *      Author: root
 */

#ifndef ROUTERINFOWIDGET_H_
#define ROUTERINFOWIDGET_H_

#include "BasicIncludes.h"
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>

class RouterInfoWidget: public Gtk::Grid
{
public:
	RouterInfoWidget();
	virtual ~RouterInfoWidget();
	void setRouterNum(int num) {routerNum = num;}
	int getFillage() {return spnFillage.get_value_as_int();}
	int getDropRate() {return spnDrop.get_value_as_int();}
	bool getActive() {return chkActive.get_active();}
	int getRouterNumber() {return routerNum;}
	typedef sigc::signal<void, bool, int> router_info_signal;
	router_info_signal signal_info_change() {return routerInfoChanged;}
protected:
	void hideWidget();
	void showWidget();
	void updateFillage();
	void updateDrop();
	void updateActive();
private:
	int routerNum;
	bool initFlag; //indicates init of view
	//router info changed signal
	router_info_signal routerInfoChanged;
	//done button
	Gtk::Button btnDone;
	//title
	Gtk::Label lblTitle;
	//controls fillage
	Gtk::Label lblFillage;
	Glib::RefPtr<Gtk::Adjustment> adjFillage;
	Gtk::SpinButton spnFillage;

	//controls drop rate
	Gtk::Label lblDrop;
	Glib::RefPtr<Gtk::Adjustment> adjDrop;
	Gtk::SpinButton spnDrop;

	//toggle activity
	Gtk::CheckButton chkActive;
};

#endif /* ROUTERINFOWIDGET_H_ */
