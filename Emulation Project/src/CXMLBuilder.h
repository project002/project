/*
 * CXMLBuilder.h
 *
 *  Created on: Mar 11, 2014
 *      Author: root
 */

#ifndef CXMLBUILDER_H_
#define CXMLBUILDER_H_
#define DEFAULT_FILE_NAME_LENGTH 10
#include "BasicIncludes.h"
#include "pugixml.hpp"
#include "SXMLStructs.h"
class CXMLBuilder
{
public:
	/**
	 * C-tor. Generating the next available file name and initiates the xml build
	 */
	CXMLBuilder();

	/**
	 * Truncates an xml file and initiates the xml build
	 * @param fileName wanted file name for the xml
	 */
	CXMLBuilder(string fileName);

	/**
	 * D-tor. nothing :D
	 */
	virtual ~CXMLBuilder();

	/**
	 * Adds a router to the XML with the wanted properties
	 * @param routerInfo the router's information
	 * @return true if the router was added
	 */
	bool AddRouter(RouterInformation routerInfo);

	/**
	 * Updating the router's GUI position in the XML for saving the GUI looks
	 * @param routerNumber the router number that its position changed
	 * @param xPos x axis value
	 * @param yPos y axis value
	 * @return true if the router was updated
	 */
	bool UpdatePosition(unsigned int routerNumber,unsigned int xPos,unsigned int yPos);

	/**
	 * adding a physical connection to the currently chosen router or the given router number
	 * @param pcName physical connection's name to be removed
	 * @param routerNumber if -1 then add to the currently chosen router, otherwise set the routerNumber router as current
	 * @return true on success on adding physical connection
	 */
	bool AddPhysicalConnection(string pcName, int routerNumber = -1);

	/**
	 * Removing a router from the XML and all the virtual connections related to it
	 * @param routerNumber
	 * @return
	 */
	bool RemoveRouter(unsigned int routerNumber);

	/**
	 * Removing a physical connection from the currently chosen router or the given router number
	 * @param pcName physical connection's name to be removed
	 * @param routerNumber if -1 then add to the currently chosen router, otherwise set the routerNumber router as current
	 * @return true on success on removing physical connection
	 */
	bool RemovePhysicalConnection(string pcName, int routerNumber = -1);

	/**
	 * setting the currently used router for easy editing of the XML file
	 * @param routerNumber router number to be edited
	 * @return true on success of setting the new router
	 */
	bool SetCurrentRouter(unsigned int routerNumber);

	/**
	 * Get the current router information or the wanted router number information that sets the current one
	 * @param routerNumber router number with wanted information
	 * @return router information
	 */
	RouterInformation GetCurrentRouterInformation( int routerNumber = -1);

	/**
	 * Editting the current router information
	 * @param routerInfo new router information to be saved
	 * @return success on editting the router's information
	 */
	bool EditCurrentRouterInformation(RouterInformation routerInfo);

	/**
	 * adding a virtual connection between 2 routers if they exist
	 * @param firstRouter
	 * @param secondRouter
	 * @return true on success of adding a virtual connection to the XML
	 */
	bool AddVirtualConnection(unsigned int firstRouter,unsigned int secondRouter);

	/**
	 * Removing a virtual connection if it exist
	 * @param firstRouter
	 * @param secondRouter
	 * @param removeAll true means all the virtual connections involving the given router numbers
	 * @return true on removing
	 */
	bool RemoveVirtualConnection(unsigned int firstRouter,unsigned int secondRouter,bool removeAll=false);

	/**
	 * removing the virtual connections involving router number
	 * @param router router number to be removed from all virtual connections
	 * @return
	 */
	bool RemoveVirtualConnectionWith(unsigned int router);

	/**
	 * setting the new file name for the XML to be saved to
	 * @param filename wanted file name
	 */
	void SetNewFilename(string filename);

	/**
	 * finalizing the XML to the file
	 */
	void Finalize();

	/**
	 * set the threaded property to the XML
	 * @param isThreaded
	 */
	void SetThreaded(bool isThreaded);

	/**
	 * checking if the string given is a number
	 * @param toCheck string to be checked
	 * @return true if the number is purely a string
	 */
	bool IsNumberString(string toCheck);

	/**
	 * check if the given interface is taken already
	 * @param pcName the interface name
	 * @return true if the interface is free
	 */
	bool IsInterfaceAvailable(string pcName);

private:
	/**
	 * checking if a virtual connection exists to avoid adding multiple VCs
	 * @param firstRouter
	 * @param secondRouter
	 * @return
	 */
	bool IsVirtualConnectionExist(unsigned int firstRouter,unsigned int secondRouter);

	/**
	 * generating the next file name to be saved
	 */
	void GenerateNextFileName();

	/**
	 * checking the given information for the router is correct
	 * including the fillage / droprates etc
	 * @param routerInfo router information to be checked
	 * @return true if the information is correct
	 */
	bool CheckRouterCorrectness(RouterInformation routerInfo);

	/**
	 * check the physical connection wasn't entered already to the current router
	 * @param pcName interface name
	 * @return true if it wasn't already added
	 */
	bool CheckPhysicalConnectionCorrectness(string pcName);

	/**
	 * checking if the router is available
	 * @param newRouterNumber
	 * @return true if the router is available
	 */
	bool IsRouterNumberFree(unsigned int newRouterNumber);

	/**
	 * Opening file for stream
	 * and building the default XML header
	 */
	void InitXML();

	/**
	 *
	 */
	void OpenFileForStream();

	/**
	 *
	 */
	void BuildDefaultXMLInformation();

	/**
	 *
	 */
	bool mNoCurrentRouterChosen;
	string mCurrentFileName;
	ofstream mFile;

	//Doc header
	pugi::xml_document mDoc;

	//Nodes
	pugi::xml_node mNetworkNode;
	pugi::xml_node mVirtualConnectionsNode;
	pugi::xml_node mRoutersNode;
	pugi::xml_node mCurrentRouter;
};

#endif /* CXMLBUILDER_H_ */
