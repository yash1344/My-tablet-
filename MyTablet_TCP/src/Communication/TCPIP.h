/*
 * TCPIP.h
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#ifndef COMMUNICATION_TCPIP_H_
#define COMMUNICATION_TCPIP_H_
#include <string>
#include <process.h>
#include <fstream>
#include <iostream>
#include <vector>

using std::string;
const string adb = "\"C:\\platform-tools\\adb.exe\"";

class TCP_IP {

private:
	string getDevice();
public:
	TCP_IP();
	virtual ~TCP_IP();

	bool forwardPort(int pcPort = 5000, int androidPort = 6000);
};

#endif /* COMMUNICATION_TCPIP_H_ */
