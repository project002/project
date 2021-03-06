/*
 * CDHCPMsgTypeException.h
 *
 *  Created on: Jan 20, 2014
 *      Author: root
 */

#ifndef CDHCPMSGTYPEEXCEPTION_H_
#define CDHCPMSGTYPEEXCEPTION_H_


#include <exception>
#include <string>
#include <sstream>

class CDHCPMsgTypeException: public std::exception
{
public:
	CDHCPMsgTypeException();
	CDHCPMsgTypeException(const uint8_t mt):_msg_type(mt),_extra_info("") {}
	CDHCPMsgTypeException(const uint8_t mt,const std::string &info):_msg_type(mt),_extra_info(info) {}
	virtual const char* what() const throw()
	{
		std::stringstream ss;
		std::string verb = "";
		switch (_msg_type)
		{
			case CDHCPMsgTypeException::DHCPACK : verb = "(ACK)"; break;
			case CDHCPMsgTypeException::DHCPDISCOVER : verb = "(DISCOVER)"; break;
			case CDHCPMsgTypeException::DHCPOFFER : verb = "(OFFER)"; break;
			case CDHCPMsgTypeException::DHCPREQUEST : verb = "(REQUEST)"; break;
			case CDHCPMsgTypeException::DHCPDECLINE : verb = "(DECLINE)"; break;
			case CDHCPMsgTypeException::DHCPNAK : verb = "(NAK)"; break;
			case CDHCPMsgTypeException::DHCPRELEASE : verb = "(RELEASE)"; break;
			case CDHCPMsgTypeException::DHCPINFORM : verb = "(INFORM)"; break;
		}
		ss << "CDHCP Message Type Exception with : " << _msg_type << verb << std::endl;
		if (_extra_info.compare("")!=0) {ss << "Extra info: " << _extra_info << std::endl;}
		return ss.str().c_str();
	}
	uint8_t getMsgType() {return _msg_type;}
	~CDHCPMsgTypeException() throw() {};
private:
	uint8_t _msg_type;
	std::string _extra_info;
	static const uint8_t DHCPDISCOVER = 1;
	static const uint8_t DHCPOFFER = 2;
	static const uint8_t DHCPREQUEST = 3;
	static const uint8_t DHCPDECLINE = 4;
	static const uint8_t DHCPACK = 5;
	static const uint8_t DHCPNAK = 6;
	static const uint8_t DHCPRELEASE = 7;
	static const uint8_t DHCPINFORM = 8;
};



#endif /* CDHCPMSGTYPEEXCEPTION_H_ */
