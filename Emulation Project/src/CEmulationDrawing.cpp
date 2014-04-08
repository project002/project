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

CEmulationDrawing::CEmulationDrawing() : mImgBuffers(std::map<Glib::ustring,Glib::RefPtr<Gdk::Pixbuf> >())
{
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
	insertNewImage("RouterImage","test.png");
	insertNewImage("Some","test2.png");
}

CEmulationDrawing::~CEmulationDrawing()
{

}

bool CEmulationDrawing::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (mImgBuffers.empty()) {return false;}

	//get the available draw area
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	//draw all images
//	mImageBufferDItr = mImgBuffers.begin();
//	for (;mImageBufferDItr!=mImgBuffers.end();++mImageBufferDItr)
//	{
//		Gdk::Cairo::set_source_pixbuf(cr, mImageBufferDItr->second,
//			(width - mImageBufferDItr->second->get_width())/2,
//			(height - mImageBufferDItr->second->get_height())/2);
//
//		cr->paint();
//	}


//	Gdk::Cairo::set_source_pixbuf(cr, mImgBuffers.at("routerImage").second,
//				(width -  mImgBuffers.at("routerImage")->second->get_width())/2,
//				(height -  mImgBuffers.at("routerImage")->second->get_height())/2);

	cr->paint();

	return true;
}



