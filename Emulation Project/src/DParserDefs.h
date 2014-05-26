/*
 * DParserDefs.h
 *
 *  Created on: Feb 9, 2014
 *      Author: root
 */

#ifndef DPARSERDEFS_H_
#define DPARSERDEFS_H_


#define XML_LAYER_1_NETWORK "Network"
#define XML_LAYER_2_ROUTERS "Routers"
#define XML_LAYER_3_INDIVIDUAL_ROUTERS "Router"
#define XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION "PhysicalConnection"

#define XML_ROUTER_NUMBER_ATTRIBUTE "Number"
#define XML_ROUTER_INITIAL_USED_BUFFER_SIZE_ATTRIBUTE "BufferSizeUsed"
#define XML_ROUTER_BUFFER_SIZE_ATTRIBUTE "BufferSize"
#define XML_ROUTER_DROP_RATE_ATTRIBUTE "DropRate"
#define XML_ROUTER_FILLAGE_ATTRIBUTE "Fillage"
#define XML_ROUTER_DYNAMIC_FILLAGE_ARRAY_ATTRIBUTE "DynamicFillageArray"
#define XML_ROUTER_DYNAMIC_DROP_RATE_ARRAY_ATTRIBUTE "DynamicDropRateArray"

//#define XML_LAYER_1_NETWORK "Network" -Already defined just here for visual
#define XML_LAYER_2_VIRTUAL_CONNECTIONS "VirtualConnections"
#define XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS "VirtualConnection"
#define XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER "Router"

//#define XML_LAYER_1_NETWORK "Network" -Already defined just here for visual
#define XML_LAYER_2_IS_STATIC_TABLE "StaticTable"
#define XML_STATIC_LAYER_ATTRIBUTE "isStatic"

#define ERROR_MSG_XML_PARSER "Error parsing file, make sure a *valid* XML file is present in the Emulation base folder\n"
#define ERROR_MSG_ADDING_PHYSICAL_CONNECTION_TO_ROUTER "Can't find wanted physical connection, make sure XML is provided with legitimate connection name"


#define FORMAT_VERSION "FormatVersion"
#define THREADED_EMULATION "Threaded"
#endif /* DPARSERDEFS_H_ */
