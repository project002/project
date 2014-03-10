/*
 * CSocketNotReadyException.h
 *
 *  Created on: Mar 10, 2014
 *      Author: root
 */

#ifndef CSOCKETNOTREADYEXCEPTION_H_
#define CSOCKETNOTREADYEXCEPTION_H_

#include <exception>

class CSocketNotReadyException: public std::exception
{
public:
	CSocketNotReadyException();
	~CSocketNotReadyException() throw() {};
};

#endif /* CSOCKETNOTREADYEXCEPTION_H_ */
