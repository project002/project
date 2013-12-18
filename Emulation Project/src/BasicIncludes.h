
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
#include <cstring>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;

/**
 * Socket Related Inclusions
 */
#include <netdb.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>

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


