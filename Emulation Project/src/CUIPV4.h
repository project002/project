/*
 * CUIPV4.h
 *
 *  Created on: Dec 29, 2013
 *      Author: root
 */

#ifndef CUIPV4_H_
#define CUIPV4_H_

#include <cstdlib>
#include <sstream>
#include <string>
/**
 * C-class U- utility
 * IPv4 Wrapper Class
 */

class CUIPV4
{
private:
	static const int IPLEN = 4;
	static const char DOT = '.';
	uint8_t ip_arr[IPLEN];
	std::string ip_str;
	std::string ip_str_alt; // string with no DOT delimiter
public:
	CUIPV4(std::string ipv4):ip_str(ipv4)
	{
		std::stringstream ss_ip(ipv4);
		char dot;
		int arr[IPLEN];
		ss_ip >> arr[0] >> dot >>
				 arr[1] >> dot >>
				 arr[2] >> dot >>
				 arr[3];
		for (int i=0;i<IPLEN;++i) {ip_arr[i] = (uint8_t) arr[i];}
		std::stringstream ss1;
		ss1 << (int)ip_arr[0] << (int)ip_arr[1] << (int)ip_arr[2] << (int)ip_arr[3];
		ip_str_alt = ss1.str();
	}

	CUIPV4(uint8_t* ipv4)
	{
		for (int i=0;i<IPLEN;++i) {ip_arr[i] = ipv4[i];}
		std::stringstream ss;
		ss << (int)ip_arr[0] << DOT << (int)ip_arr[1]<< DOT << (int)ip_arr[2]<< DOT << (int)ip_arr[3];
		ip_str = ss.str();

		std::stringstream ss1;
		ss1 << (int)ip_arr[0] << (int)ip_arr[1] << (int)ip_arr[2] << (int)ip_arr[3];
		ip_str_alt = ss1.str();
	}
	inline const uint8_t* getIpArr() const {return ip_arr;}

	inline const std::string& getIpStr() const {return ip_str;}

	inline const std::string& getIpStrNoDot() const {return ip_str_alt;}

};

#endif /* CUIPV4_H_ */
