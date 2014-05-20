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
CEmulationDrawing::CEmulationDrawing() :
	mXMLPath(),
	mElementsPos(new ElementMap()),
	mLinesPos(new LinesMap()),
	mImgBuffers(ImageBuffers())
{
	this->add_events(Gdk::BUTTON_PRESS_MASK); //yeah!
	loadImagesSrouces();
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
}

CEmulationDrawing::~CEmulationDrawing()
{
	delete mElementsPos;
	delete mLinesPos;
}

void CEmulationDrawing::resetDrawing(string xml_path)
{
	mXMLPath = xml_path;
	mXMLprs.ParseXML(mXMLPath);
	//clear info vectors or what not
	mElementsPos->clear();
	initial_positions();
}

int CEmulationDrawing::physical_routers_count()
{
	vector<RouterInformation> routers = mXMLprs.GetRoutersInformation();
	int count = 0;
	vector<RouterInformation>::iterator it = routers.begin();
	for (;it!=routers.end();++it) {if (mXMLprs.isPhysicalRouter((*it).sRouterNumber)) {++count;}}
	return count;
}

void CEmulationDrawing::initial_positions()
{
	if (mXMLPath.length()==0) {return;}
	vector<RouterInformation> routers = mXMLprs.GetRoutersInformation();
	multimap<unsigned int,string> phCon = mXMLprs.GetPhysicalConnections();
	int x_percent=CNVS_PAD;
	int y_percent=50;
	//TODO: this works for only two physical routers which are both on either side of the screen
	int pRoutersNum = physical_routers_count();
	int percent_inc = (100-(CNVS_PAD*2))/((routers.size()+1)-pRoutersNum);
	std::pair<int,int> pos;
	LineP line_pos;
	vector<RouterInformation>::iterator it = routers.begin();
	//router & physical connections positions
	for (;it!=routers.end();++it)
	{
		unsigned int id = (*it).sRouterNumber;
		//is source element
		if (mXMLprs.isPhysicalRouter(id))
		{
			pos = next_source_router_pos();
			//insert lines for physical routers
			line_pos = LineP(pos,next_source_con_pos());
			mLinesPos->insert(std::pair< unsigned int,LineP >(id,line_pos));
		}
		else
		{
			x_percent += percent_inc;
			pos = std::pair<int,int>(x_percent,y_percent);
		}
		mElementsPos->insert(std::pair< unsigned int,std::pair<int,int> >(id,pos));
	}

	//virtual connection positions
	ElementMap::iterator eit = mElementsPos->begin();
	vector< pair<int,int> > positions;
	for(;eit!=mElementsPos->end();++eit)
	{
		positions = get_connected_routers(eit->first);
		vector< pair<int,int> >::iterator pit = positions.begin();
		for(;pit!=positions.end();++pit)
		{
			line_pos = LineP(eit->second,(*pit));
			mLinesPos->insert(std::pair< unsigned int,LineP >(eit->first,line_pos));
		}
	}
//	///printout
//	LinesMap::iterator lit = mLinesPos->begin();
//	for (;lit!=mLinesPos->end();++lit)
//	{
//		cout << "id: " << lit->first;
//		cout << "p1: " << lit->second.first.first << ":" << lit->second.first.second << "  ";
//		cout << "p2: " << lit->second.second.first << ":" << lit->second.second.second << endl;
//	}

}

bool CEmulationDrawing::on_event(GdkEvent* event)
{
	cout << "event" << endl;
	return true;
}


std::pair<int, int> CEmulationDrawing::next_source_router_pos()
{
	static int i=0;
	static int r=4;
	int source_pos[][2] = {{CNVS_PAD,50},{100-CNVS_PAD,50},{50,CNVS_PAD},{50,100-CNVS_PAD}}; //matrix of source positions in percent
	if (i>=r) {i=0;}
	std::pair<int,int> ret = std::pair<int,int>(source_pos[i][0],source_pos[i][1]);
	i++;
	return ret;
}

std::pair<int, int> CEmulationDrawing::next_source_con_pos()
{
	static int i=0;
	static int r=4;
	int source_pos[][2] = {{0,50},{100,50},{50,0},{50,100}}; //matrix of source positions in percent
	if (i>=r) {i=0;}
	std::pair<int,int> ret = std::pair<int,int>(source_pos[i][0],source_pos[i][1]);
	i++;
	return ret;
}

int CEmulationDrawing::percent2pixel(int percent, int pixel_value)
{
	float v = (pixel_value/100.0)*percent;
	return (int) v;
}

vector<std::pair<int, int> > CEmulationDrawing::get_connected_routers(int id)
{
	//typedef to the rescue!
	typedef multimap<unsigned int,unsigned int> VirCons;

	vector<std::pair<int, int> > ret = vector<std::pair<int, int> >();

	if (mXMLPath.length()==0) {return ret;}
	VirCons vCons = mXMLprs.GetVirtualConnections();
	//can it be any longer? nope. saved by typedef!
	std::pair<VirCons::iterator,VirCons::iterator> range = vCons.equal_range(id);
	if (range.first == vCons.end()) {return ret;}

	VirCons::iterator it = range.first;
	for (;it!=range.second;++it)
	{
		ret.push_back((mElementsPos->at(it->second)));
	}
	return ret;

}

bool CEmulationDrawing::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (mImgBuffers.empty()) {return false;}
	//get the available draw area
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	int base_pos[] = {0,0};
	int target_pos[] = {0,0};

	//draw connections by positions
	cr->set_line_width(4.0);
	LinesMap::iterator lit = mLinesPos->begin();
	for (;lit!=mLinesPos->end();++lit)
	{
		if (mXMLprs.isPhysicalRouter(lit->first))
		{
			cr->set_source_rgb(1,0,0);
		}
		else
		{
			cr->set_source_rgb(0,0,0);
		}

		base_pos[0] = percent2pixel(lit->second.first.first,width);
		base_pos[1] = percent2pixel(lit->second.first.second,height);

		target_pos[0] = percent2pixel(lit->second.second.first,width);
		target_pos[1] = percent2pixel(lit->second.second.second,height);

		cr->move_to(base_pos[0],base_pos[1]);
		cr->line_to(target_pos[0],target_pos[1]);
		cr->stroke();
	}

	//draw elements by positions
	ElementMap::iterator it = mElementsPos->begin();
	for (;it!=mElementsPos->end();++it)
	{
		string src_name = mXMLprs.isPhysicalRouter(it->first) ? "RouterImageSource" : "RouterImage";
		base_pos[0] = percent2pixel(it->second.first,width);
		base_pos[1] = percent2pixel(it->second.second,height);

		//paint router image
		Gdk::Cairo::set_source_pixbuf(cr,
				mImgBuffers.at(src_name),
				base_pos[0] - mImgBuffers.at(src_name)->get_width()/2,
				base_pos[1] - mImgBuffers.at(src_name)->get_height()/2);
		cr->paint();
	}


	return true;
}



