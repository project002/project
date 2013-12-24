
/**
 * The file contains includes for commonly used libraries and Namespaces
 */

#ifndef BASIC_INCLUDES_H
#define BASIC_INCLUDES_H
/**
 * Basic Streaming Inclusions
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>

typedef std::pair<std::string, std::string> TStrStrPair;
#include "Color.h"
using namespace std;

/**
 * Packet specific related inclusions
 */
#include <crafter.h>
using namespace Crafter;
#include <ifaddrs.h>
/**
 * Exception Related Inclusions
 */
#include <exception>
#include <typeinfo>

/**
 * STL Containers Related Inclusions
 */
#include <vector>
#include <list>
#include <map>
typedef map<std::string, std::string> TStrStrMap;
#include <set>

/**
 * Exception Related Inclusions
 */
#include "CException.h"

/**
 * Error Related Inclusions
 */
#include <errno.h>

/**
 * Thread Related Inclusions
 */
#include <pthread.h>



/**
 * Defined missing STL
 */
#define IPv4_ALEN 4

#endif


