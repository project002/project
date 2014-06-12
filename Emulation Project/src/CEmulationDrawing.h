/*
 * CEmulationDrawing.h
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 */

#ifndef CEMULATIONDRAWING_H_
#define CEMULATIONDRAWING_H_

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include "CXMLBasicParser.h"

typedef std::pair<int,int> Point;
typedef std::pair< Point , Point > LineP;
typedef std::map< unsigned int,Point > ElementMap;
typedef std::map< unsigned int,std::vector< Point* > > LineRel;
//typedef std::multimap< unsigned int, LineP > LinesMap;
typedef std::vector< LineP > LinesMap;
typedef std::map< Glib::ustring,Glib::RefPtr<Gdk::Pixbuf> > ImageBuffers;
typedef std::vector<std::pair<std::string,Point> > LabelsPos;
class CEmulationDrawing : public Gtk::DrawingArea
{
public:
	CEmulationDrawing();
	void resetDrawing(string xml_path);
	virtual ~CEmulationDrawing();
	/**
	 * makes the drag action on the cavans to also
	 * refresh the canvas (use only if the canvas is not
	 * refreshed externally)
	 */
	void forceDragRefresh() {mForceDragRefresh = true;}

	Point get_router_position(int unsigned rID) {return mElementsPos->at(rID);}
private:
	//setup xml path
	string mXMLPath;
	//setup xml parser
	CXMLBasicParser* mXMLprs;
	//positions of the objects key:element id value : position (x,y)
	ElementMap* mElementsPos;
	//positions of the lines key:element id value : positions (x1,y1),(x2,y2)
	LinesMap* mLinesPos;
	//stores references to the connections attached to the router by id
	LineRel* mConnectionRelation;
	//hold the physical connection labels positions
	LabelsPos* mLabelPositions;
	//text layout
	Glib::RefPtr<Pango::Layout> mTlayout;
	void insertNewImage(Glib::ustring imageName,Glib::ustring imagePath);
	void loadImagesSrouces();
	int physical_routers_count();
	void initial_positions(); //setup the initial positions of the object in the emulation (in percentage)
	//indicate drag event start
	bool mStartDrag;
	//indicates the currently focused (selected) router ID
	int unsigned mSelectedRouterID;
	Point* mDragRef;
	int canvasW;
	int canvasH;
	Point noPos;
	std::vector< Point* > mConDragRef;
	//allow canvas refresh when dragging (when refresh of the canvas is not external)
	bool mForceDragRefresh;
	//make the router data come statically from the xml file and not the emulation
//	bool mStaticData;

	std::map<int unsigned,Point > get_connected_routers(int id);
	/**
	 * @param reset to reset the next position counter
	 * @return the next position of a source router, if reset = true return value is meaningless
	 */
	std::pair<int,int> next_source_router_pos(bool reset = false);
	std::pair<int,int> next_source_con_pos(bool reset = false);
	/**
	 * returns the pixel value of the given percent and
	 * maximum (100%) pixel value
	 * @param percent the percent form the whole (100%)
	 * @param pixel_value the number which represent the real number of the whole (100%)
	 * @return the pixel value which is "percent" of "pixel_value"
	 */
	int percent2pixel(int percent,int pixel_value);
	int pixel2percent(int pixel,int pixel_value);

	/**
	 * get a reference id of the element being
	 * clicked on
	 * @param px - the position of the x pointer in percent
	 * @param py - the position of the y pointer in percent
	 * @return
	 */
	int get_clicked_element(int px,int py);

	/**
	 * sets the size of the mLinesPos vector
	 * according to the number of connections in the emulation.
	 * this is done so when we create the mConnectionRelation
	 * memory address won't change in mLinesPos (when it is populated)
	 */
	void set_lines_count();

	/**
	 * @param rid router id
	 * @parma pos[] an array representing a point x,y of the router
	 * @param cr the cairo context
	 */
	void draw_router_info(int rid,int pos[],const Cairo::RefPtr<Cairo::Context>& cr);

	/**
	 * draws the interface name of the physical connections
	 * @param cr the cairo context
	 */
	void draw_connections_info(const Cairo::RefPtr<Cairo::Context>& cr);

	void print_relations()
	{
		LineRel::iterator it= mConnectionRelation->begin();
		for (;it!=mConnectionRelation->end();++it)
		{
			cout << "Router " << it->first << ":" << endl;
			std::vector< Point* >::iterator vit = it->second.begin();
			for(;vit!=it->second.end();++vit)
			{
				cout << "\tRef:" << (*vit)->first << "," << (*vit)->second << " Mem: " << (*vit) << endl;
			}
		}
	}

	/**
	 * counts then number of physical connections in the router
	 * @param rID the id of the router
	 * @return number of physical connections
	 */
	int get_physical_con_count(int unsigned rID);
protected:
	//Override default signal handler:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_button_press_event (GdkEventButton* event);
	virtual bool on_button_release_event (GdkEventButton* event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	ImageBuffers mImgBuffers;
	//map iterator for the drawing process
	ImageBuffers::iterator mImageBufferDItr;
};

#endif /* CEMULATIONDRAWING_H_ */


/*
//cml parser tests
CXMLBasicParser prs;
prs.ParseXML(mImportXMLPath);
std::cout << prs.GetNumberOfRouters() << std::endl;
vector<RouterInformation> rInfo= prs.GetRoutersInformation();
vector<RouterInformation>::iterator it = rInfo.begin();
std::cout << "Buffer Information:" << std::endl;
for (;it!=rInfo.end();++it)
{
	std::cout << "\tID:" << (*it).sRouterNumber << std::endl;
	std::cout << "\tDropRate:" << (*it).sDropRate << std::endl;
	std::cout << "\tBufferSize:" << (*it).sBufferSize << std::endl;
	std::cout << "\tUsedBufferSize:" << (*it).sUsedBufferSize << std::endl;
}

std::multimap<unsigned int,unsigned int> vcons = prs.GetVirtualConnections();
std::multimap<unsigned int,unsigned int>::iterator vit= vcons.begin();
std::cout << "Virtual Connections:" << std::endl;
for (;vit!=vcons.end();++vit)
{
	std::cout << "\tFrom: " << vit->first  << " To: " << vit->second << std::endl;
}

std::multimap<unsigned int,std::string> pcons = prs.GetPhysicalConnections();
std::multimap<unsigned int,std::string>::iterator pit= pcons.begin();
std::cout << "Physical Connections:" << std::endl;
for (;pit!=pcons.end();++pit)
{
	std::cout << "\tFrom: " << pit->first  << " To: " << pit->second << std::endl;
}

std::cout << "Connections Number:" << std::endl;
for (it = rInfo.begin();it!=rInfo.end();++it)
{
	std::cout << "\tID: " << (*it).sRouterNumber  << " Count: " << prs.GetNumberOfConnectionPerRouter((*it).sRouterNumber) << std::endl;
}

///test end*/
