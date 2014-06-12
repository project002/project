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
	CXMLBuilder();
	CXMLBuilder(string fileName);
	virtual ~CXMLBuilder();
	bool AddRouter(RouterInformation routerInfo);
	bool UpdatePosition(unsigned int routerNumber,unsigned int xPos,unsigned int yPos);
	bool AddPhysicalConnection(string pcName);
	bool RemoveRouter(unsigned int routerNumber);
	bool RemovePhysicalConnection(string pcName);
	bool SetCurrentRouter(unsigned int routerNumber);
	RouterInformation GetCurrentRouterInformation();
	bool EditCurrentRouterInformation(RouterInformation routerInfo);
	bool AddVirtualConnection(unsigned int firstRouter,unsigned int secondRouter);
	bool RemoveVirtualConnection(unsigned int firstRouter,unsigned int secondRouter);
	bool RemoveVirtualConnectionWith(unsigned int router);
	void SetNewFilename(string filename);
	void Finalize();
	void SetThreaded(bool isThreaded);
private:
	bool IsVirtualConnectionExist(unsigned int firstRouter,unsigned int secondRouter);
	void GenerateNextFileName();
	bool CheckRouterCorrectness(RouterInformation routerInfo);
	bool CheckPhysicalConnectionCorrectness(string pcName);
	bool IsRouterNumberFree(unsigned int newRouterNumber);
	void InitXML();
	void OpenFileForStream();
	void BuildDefaultXMLInformation();

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
