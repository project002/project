/*
 * CEmulationDrawing.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 */

#include "CEmulationDrawing.h"
#include <cairomm/context.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>
#include "CException.h"
//canvas padding for the physical router & connection
#define CNVS_PAD 20
#define NO_POS -2
#define NO_SEL 0
CEmulationDrawing::CEmulationDrawing() :
	mXMLPath(),
	mXMLprs(NULL),
	mElementsPos(new ElementMap()),
	mLinesPos(new LinesMap()),
	mConnectionRelation(new LineRel()),
	mLabelPositions(new LabelsPos()),
	mStartDrag(false),
	mSelectedRouterID(NO_SEL),
	mDragRef(NULL),
	canvasW(0),canvasH(0),
	noPos(std::pair<int,int>(NO_POS,NO_POS)),
	mConDragRef(std::vector< Point* >()),
	mForceDragRefresh(false),
//	mStaticData(false),
	mImgBuffers(ImageBuffers())
{
	this->add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK); //yeah!
	loadImagesSrouces();
	//init the text font layout
	Pango::FontDescription font;
	font.set_family("Monospace");
	font.set_weight(Pango::WEIGHT_BOLD);
	mTlayout = create_pango_layout("");
	mTlayout->set_font_description(font);

}

void CEmulationDrawing::insertNewImage(Glib::ustring imageName,Glib::ustring imagePath)
{
	std::pair<Glib::ustring,Glib::RefPtr<Gdk::Pixbuf> > newImg;
	newImg.first = imageName;
	try
	{
		newImg.second = Gdk::Pixbuf::create_from_file(imagePath);
	}
	catch(const Glib::FileError& ex)
	{
		std::cerr << "Image File Missing: " << ex.what() << std::endl;
		throw CException("Missing Image File");
		return;
	}
	catch(const Gdk::PixbufError& ex)
	{
		std::cerr << "PixbufError: " << ex.what() << std::endl;
		throw CException("Unable To Load an Image");
		return;
	}
	mImgBuffers.insert(newImg);
}

void CEmulationDrawing::loadImagesSrouces()
{
	//insert router image
	insertNewImage("RouterImage","ricon.png");
	insertNewImage("RouterImageSource","rsicon.png");
	insertNewImage("RouterImageSel","ricon_sel.png");
	insertNewImage("RouterImageSourceSel","rsicon_sel.png");
}

CEmulationDrawing::~CEmulationDrawing()
{
	delete mElementsPos;
	delete mLinesPos;
	delete mConnectionRelation;
	delete mLabelPositions;
}

void CEmulationDrawing::resetDrawing(string xml_path)
{
	mXMLPath = xml_path;
	if (mXMLprs != NULL) {delete mXMLprs;}
	mXMLprs = new CXMLBasicParser();
	mXMLprs->ParseXML(mXMLPath);
	//clear info vectors and what not
	mConnectionRelation->clear();
	mElementsPos->clear();
	mLinesPos->clear();
	mLabelPositions->clear();
	next_source_con_pos(true);
	next_source_router_pos(true);
	initial_positions();
}

int CEmulationDrawing::physical_routers_count()
{
	vector<RouterInformation> routers = mXMLprs->GetRoutersInformation();
	int count = 0;
	vector<RouterInformation>::iterator it = routers.begin();
	for (;it!=routers.end();++it) {if (mXMLprs->isPhysicalRouter((*it).sRouterNumber)) {++count;}}
	return count;
}

void CEmulationDrawing::set_lines_count()
{
	int p = mXMLprs->GetPhysicalConnections().size();
	int v = mXMLprs->GetVirtualConnections().size();
	mLinesPos->resize((((p+v)*((p+v))-1)/2)+4);
}

void CEmulationDrawing::initial_positions()
{
	//typedef!!!
	typedef multimap<unsigned int,string> PhyCons;
	if (mXMLPath.length()==0) {return;}
	vector<RouterInformation> routers = mXMLprs->GetRoutersInformation();
	PhyCons phCon = mXMLprs->GetPhysicalConnections();
	//resize the vector so it wont re-allocate memory mid-init and fuck up the Point references at mConnectionRelation
	set_lines_count();
	int x_percent=CNVS_PAD;
	int y_percent=50;
	int pRoutersNum = physical_routers_count();
	int percent_inc = (100-(CNVS_PAD*2))/((routers.size()+1)-pRoutersNum);
	std::pair<int,int> pos;
	LineP line_pos;
	vector<RouterInformation>::iterator it = routers.begin();
	bool isPhysical = false;
	//router & physical connections positions
	for (;it!=routers.end();++it)
	{
		unsigned int id = (*it).sRouterNumber;
		isPhysical = mXMLprs->isPhysicalRouter(id);
		mConnectionRelation->insert(std::pair<unsigned int ,std::vector< Point* > >(id,std::vector< Point* >()));
		//check if there is a position in the file itself
		Point rPos = mXMLprs->GetRouterPosition(id);
		bool posPresent = (rPos.first!=0 || rPos.second);
		//is source element
		if (isPhysical)
		{
			if (posPresent) {pos = rPos;} else {pos = next_source_router_pos();}
			//make this foreach physical connection this router has
			std::pair<PhyCons::iterator,PhyCons::iterator> range = phCon.equal_range(id);
			for (PhyCons::iterator n = range.first; n!=range.second;++n)
			{
				mXMLprs->GetPhysicalConnections();

				//insert lines for physical routers
				line_pos = LineP(pos,next_source_con_pos());
				//mLinesPos->insert(std::pair< unsigned int,LineP >(id,line_pos));
				mLinesPos->push_back(line_pos);
				//add relation of connection and router
				mConnectionRelation->at(id).push_back(&(mLinesPos->back().first));
	//			cout << "inserted reference " << id << " with: " << mLinesPos->back().first.first << "," << mLinesPos->back().first.second  << " Mem " << &(mLinesPos->back().first) << std::endl;
				mLabelPositions->push_back(std::pair<string,Point>(n->second,line_pos.second));
			}
		}
		else
		{
			if (posPresent)
			{
				pos = rPos;
			}
			else
			{
				x_percent += percent_inc;
				pos = std::pair<int,int>(x_percent,y_percent);
			}
		}
		mElementsPos->insert(std::pair< unsigned int,std::pair<int,int> >(id,pos));
	}

	try {
	//virtual connection positions
	ElementMap::iterator eit = mElementsPos->begin();
	std::map<int unsigned,Point > positions;
	for(;eit!=mElementsPos->end();++eit)
	{
		positions = get_connected_routers(eit->first);
		std::map<int unsigned,Point >::iterator pit = positions.begin();
		for(;pit!=positions.end();++pit)
		{
			line_pos = LineP(eit->second,(*pit).second);
			mLinesPos->push_back(line_pos);
			mConnectionRelation->at(eit->first).push_back(&(mLinesPos->back().first));
			mConnectionRelation->at((*pit).first).push_back(&(mLinesPos->back().second));
		}
	}

	}
	catch (std::exception & err) {cout << "Missing Router For Connection";}
	///printout
//	LinesMap::iterator lit = mLinesPos->begin();
//	for (;lit!=mLinesPos->end();++lit)
//	{
//		cout << "p1: " << lit->first.first << ":" << lit->first.second << " Mem " << &(lit->first) << "  ";
//		cout << "p2: " << lit->second.first << ":" << lit->second.second << " Mem " << &(lit->second) << endl;
//	}
//
//	print_relations();

}


std::pair<int, int> CEmulationDrawing::next_source_router_pos(bool reset)
{
	static int i=0;
	static int r=4;
	static int offset = 0;
	offset = ((i+1) % 5 == 0) ? ((offset+5) % 50) : offset;
	int source_pos[][2] = {{CNVS_PAD,50+offset},{100-CNVS_PAD,50+offset},{50+offset,CNVS_PAD},{50+offset,100-CNVS_PAD}}; //matrix of source positions in percent
	if (i>=r) {i=0;}
	std::pair<int,int> ret = std::pair<int,int>(source_pos[i][0],source_pos[i][1]);
	i++;
	if (reset) {i=0; offset=0;}
	return ret;
}

std::pair<int, int> CEmulationDrawing::next_source_con_pos(bool reset)
{
	static int i=0;
	static int r=4;
	static int offset = 0;
	offset = ((i+1) % 5 == 0) ? ((offset+5) % 50) : offset;
	int source_pos[][2] = {{0,50+offset},{100,50+offset},{50+offset,0},{50+offset,100}}; //matrix of source positions in percent
	if (i>=r) {i=0;}
	std::pair<int,int> ret = std::pair<int,int>(source_pos[i][0],source_pos[i][1]);
	i++;
	if (reset) {i=0; offset=0;}
	return ret;
}

int CEmulationDrawing::percent2pixel(int percent, int pixel_value)
{
	float v = (pixel_value/100.0)*percent;
	return (int) v;
}

int CEmulationDrawing::pixel2percent(int pixel, int pixel_value)
{
	float v = (100.0/pixel_value)*pixel;
	return (int) v;
}

std::map<int unsigned,Point > CEmulationDrawing::get_connected_routers(int id)
{
	//typedef to the rescue!
	typedef multimap<unsigned int,unsigned int> VirCons;

	std::map<int unsigned,Point > ret = std::map<int unsigned,std::pair<int, int> >();

	if (mXMLPath.length()==0) {return ret;}
	VirCons vCons = mXMLprs->GetVirtualConnections();
	//can it be any longer? nope. saved by typedef!
	std::pair<VirCons::iterator,VirCons::iterator> range = vCons.equal_range(id);
	if (range.first == vCons.end()) {return ret;}

	VirCons::iterator it = range.first;
	for (;it!=range.second;++it)
	{
		ret.insert(std::pair<int unsigned,Point>(it->second,(mElementsPos->at(it->second))));
	}
	return ret;

}

int CEmulationDrawing::get_physical_con_count(unsigned int rID)
{
	//typedef!!!
	typedef multimap<unsigned int,string> PhyCons;
	PhyCons cons = mXMLprs->GetPhysicalConnections();
	return cons.count(rID);
}

int CEmulationDrawing::get_clicked_element(int px, int py)
{
	ElementMap::iterator eit = mElementsPos->begin();
	int sp_w = pixel2percent(mImgBuffers.at("RouterImage")->get_width(),canvasW)/2;
	int sp_h = pixel2percent(mImgBuffers.at("RouterImage")->get_height(),canvasH)/2;
	for(;eit!=mElementsPos->end();++eit)
	{
		int spx = eit->second.first;
		int spy = eit->second.second;
		if ((px<=(spx+sp_w) && px>=(spx-sp_w)) && (py<=(spy+sp_h) && py>(spy-sp_h)))
		{
			return eit->first;
		}
	}
	return NO_POS;
}


void CEmulationDrawing::draw_router_info(int rid,int pos[],const Cairo::RefPtr<Cairo::Context>& cr)
{

	stringstream ss;
	ss << rid << "\n";

	ss << "Buffer Size: " << SDataController::getInstance().get_router_data(rid,SDataController::BUFFERSIZE) << "\n";
	ss << "Buffer Fill Init.: " << SDataController::getInstance().get_router_data(rid,SDataController::BUFFERUS) << "\n";
	ss << "Fillage: " << SDataController::getInstance().get_router_data(rid,SDataController::FILLAGE) << "\n";
	ss << "DropRate: " <<  SDataController::getInstance().get_router_data(rid,SDataController::DROPRATE)<< "\n";

	mTlayout->set_text(ss.str());

	int text_width;
	int text_height;

	//get the text dimensions (it updates the variables -- by reference)
	mTlayout->get_pixel_size(text_width, text_height);


	// Position the text in the middle
	cr->move_to(pos[0]-(text_width/2), pos[1]+(mImgBuffers.at("RouterImage")->get_height()/2));

	mTlayout->show_in_cairo_context(cr);
}

void CEmulationDrawing::draw_connections_info(const Cairo::RefPtr<Cairo::Context>& cr)
{
	int text_width;
	int text_height;
	int x,y;
	for (LabelsPos::iterator it=mLabelPositions->begin(); it!=mLabelPositions->end(); ++it)
	{
		mTlayout->set_text(it->first);
		mTlayout->get_pixel_size(text_width, text_height);
		x = percent2pixel(it->second.first,canvasW);
		y = percent2pixel(it->second.second,canvasH);
		x = it->second.first == 100 ? x-text_width : x;
		y = it->second.second == 100 ? y - text_height : y;
		cr->move_to(x,y);
		mTlayout->show_in_cairo_context(cr);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////EVENTS/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CEmulationDrawing::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (mImgBuffers.empty()) {return false;}
	//get the available draw area
	Gtk::Allocation allocation = get_allocation();
	canvasW = allocation.get_width();
	canvasH = allocation.get_height();
	int base_pos[] = {0,0};
	int target_pos[] = {0,0};

	//draw connections by positions
	cr->set_line_width(4.0);
	//quickNdirty_lines_update();
	LinesMap::iterator lit = mLinesPos->begin();
	for (;lit!=mLinesPos->end();++lit)
	{
		base_pos[0] = percent2pixel(lit->first.first,canvasW);
		base_pos[1] = percent2pixel(lit->first.second,canvasH);

		target_pos[0] = percent2pixel(lit->second.first,canvasW);
		target_pos[1] = percent2pixel(lit->second.second,canvasH);

		cr->move_to(base_pos[0],base_pos[1]);
 		cr->line_to(target_pos[0],target_pos[1]);
		cr->stroke();
	}
	draw_connections_info(cr);

	//draw elements by positions
	ElementMap::iterator it = mElementsPos->begin();
	for (;it!=mElementsPos->end();++it)
	{
		string src_name = "RouterImage";
		if (mXMLprs->isPhysicalRouter(it->first))
		{
			src_name = (it->first == mSelectedRouterID) ? "RouterImageSourceSel" : "RouterImageSource";
		}
		else
		{
			src_name = (it->first == mSelectedRouterID) ? "RouterImageSel" : "RouterImage";
		}
		base_pos[0] = percent2pixel(it->second.first,canvasW);
		base_pos[1] = percent2pixel(it->second.second,canvasH);

		//paint router image
		Gdk::Cairo::set_source_pixbuf(cr,
				mImgBuffers.at(src_name),
				base_pos[0] - mImgBuffers.at(src_name)->get_width()/2,
				base_pos[1] - mImgBuffers.at(src_name)->get_height()/2);
		cr->paint();

		cr->set_source_rgb(0,0,0);
		draw_router_info(it->first,base_pos,cr);
	}


	return true;
}



bool CEmulationDrawing::on_button_press_event(GdkEventButton* event)
{
	if (mXMLprs == NULL) {return true;}
	mStartDrag = true;
	int eid = get_clicked_element(pixel2percent(event->x,canvasW),pixel2percent(event->y,canvasH));
	if (eid==NO_POS)
	{
		mDragRef = NULL;
		mConDragRef = std::vector< Point* >();
		mSelectedRouterID = NO_SEL;
	}
	else
	{
		mSelectedRouterID = eid;
		mDragRef = &mElementsPos->at(eid);
		//connections to update
		mConDragRef = mConnectionRelation->at(eid);
	}
	if (mForceDragRefresh) {queue_draw();}
	return true;
}

bool CEmulationDrawing::on_button_release_event(GdkEventButton* event)
{
	if (mXMLprs == NULL) {return true;}
	mStartDrag = false;
	mDragRef = NULL;
//	if (event->button == 3) //3 is the right mouse button (NO COSTANT FOR THAT)
//	{
		int eid = get_clicked_element(pixel2percent(event->x,canvasW),pixel2percent(event->y,canvasH));
		//event->button = eid; //a hacky way to send information upward;
		event->x_root = eid;
		return eid==NO_POS; //propagate if false => if clicked on a router
//	}
//	return true;
}

bool CEmulationDrawing::on_motion_notify_event(GdkEventMotion* event)
{
	if (mXMLprs == NULL) {return true;}
	if (mStartDrag && mDragRef!=NULL)
	{
		int px = pixel2percent(event->x,canvasW);
		int py = pixel2percent(event->y,canvasH);

		if (px<0) {px=0;}
		if (px>100) {px=100;}
		if (py<0) {py = 0;}
		if (py>100) {py=100;}

		mDragRef->first = px;
		mDragRef->second = py;
		//update connections positions
		std::vector< Point* >::iterator it = mConDragRef.begin();
		for (;it!=mConDragRef.end();++it)
		{
			(*it)->first = px;
			(*it)->second = py;
		}
		if (mForceDragRefresh) {queue_draw();}
	}
	return true;
}
