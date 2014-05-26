
/**
 * The file contains includes for commonly used libraries and Namespaces
 */

#ifndef BASIC_INCLUDES_H
#define BASIC_INCLUDES_H
/**
 * Basic Streaming Inclusions
 */
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

/**
 * Logger of the emulation
 */
#include "SLogger.h"

/**
 * Emulation report
 */
#include "SReport.h"
using namespace std;

/**
 * Packet handling library
 */
#include <crafter.h>
using namespace Crafter;

/**
 * OS addresses analyzer
 */
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
 * Defined missing STL
 */
#define IPv4_ALEN 4

/**
 * IPv4 Wrapper
 */

#include "CUIPV4.h"

/**
 * Basic GUI singleton
 */
#include "SDataController.h"


#endif


