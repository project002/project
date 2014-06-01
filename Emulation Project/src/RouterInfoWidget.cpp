/*
 * RouterInfoWidget.cpp
 *
 *  Created on: Jun 1, 2014
 *      Author: root
 */

#include "RouterInfoWidget.h"

RouterInfoWidget::RouterInfoWidget():
	btnDone("Done"),
	lblTitle("Router"),
	lblFillage("Fillage:"),
	adjFillage(Gtk::Adjustment::create(0.,0.,100.0,1.,1.,1.)),
	spnFillage(adjFillage),
	lblDrop("Drop Rate:"),
	adjDrop(Gtk::Adjustment::create(0.,0.,100.0,1.,1.,1.)),
	spnDrop(adjDrop)

{
	set_column_spacing(10);
	//events
	btnDone.signal_clicked().connect(sigc::mem_fun(*this,&RouterInfoWidget::hideWidget));
	spnFillage.signal_value_changed().connect(sigc::mem_fun(*this,&RouterInfoWidget::updateFillage));
	spnDrop.signal_value_changed().connect(sigc::mem_fun(*this,&RouterInfoWidget::updateDrop));
	signal_show().connect(sigc::mem_fun(*this,&RouterInfoWidget::showWidget));

	//order
	attach(lblTitle,0,0,2,1);
	attach(lblFillage,2,0,2,1);
	attach(spnFillage,4,0,2,1);
	attach(lblDrop,6,0,2,1);
	attach(spnDrop,8,0,2,1);
	attach(btnDone,10,0,1,1);

	//show_all();
}

RouterInfoWidget::~RouterInfoWidget()
{
	// TODO Auto-generated destructor stub
}


void RouterInfoWidget::hideWidget()
{
	hide();
}

void RouterInfoWidget::showWidget()
{
	std::stringstream ss;
	ss << "Router " << routerNum << ": ";
	lblTitle.set_label(ss.str());
	//update the fillage of the router
	double Fillage = double(SDataController::getInstance().get_router_data(routerNum,SDataController::FILLAGE));
	double DropRate = double(SDataController::getInstance().get_router_data(routerNum,SDataController::DROPRATE));
	spnFillage.set_value(Fillage);
	spnDrop.set_value(DropRate);
}

void RouterInfoWidget::updateFillage()
{
	std::cout << "update fillage to " << spnFillage.get_value_as_int() << " emmiting signal\n";
	routerInfoChanged.emit(true,1); //values are pretty much irrelevant
}

void RouterInfoWidget::updateDrop()
{
	std::cout << "update drop to " << spnFillage.get_value_as_int() << " emmiting signal\n";
	routerInfoChanged.emit(true,1); //values are pretty much irrelevant
}
