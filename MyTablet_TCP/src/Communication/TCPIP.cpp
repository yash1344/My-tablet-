/*
 * TCPIP.cpp
 *
 *  Created on: Mar 13, 2025
 *      Author: vagha
 */

#include "TCPIP.h"

using namespace std;

TCP_IP::TCP_IP() {
	// TODO Auto-generated constructor stub

}

TCP_IP::~TCP_IP() {
	// TODO Auto-generated destructor stub
}

string TCP_IP::getDevice() {

	string adbCommand = adb + " devices > devices.txt"; // Redirect output to a file
	system(adbCommand.c_str());

	// Read the devices from the file
	ifstream file("devices.txt");
	string line;
	vector<string> devices;

	while (getline(file, line)) {
		if (line.find("device") != string::npos
				&& line.find("List") == string::npos) {
			string deviceID = line.substr(0, line.find('\t'));
			devices.push_back(deviceID);
		}
	}
	file.close();
	system("del devices.txt"); // Clean up (Windows) - use "rm devices.txt" on Linux/macOS

	if (devices.empty()) {
		cerr << "No devices found!" << endl;
		return "";
	}

	cout << "Available devices:\n";
	for (size_t i = 0; i < devices.size(); i++) {
		cout << i + 1 << ". " << devices[i] << endl;
	}

	cout << "Select a device (enter number): ";
	int choice;
	cin >> choice;

	if (choice < 1 || choice > (int) devices.size()) {
		cerr << "Invalid choice!";
		return "";
	}

	return devices[choice - 1]; // Return selected device ID
}

bool TCP_IP::forwardPort(int pcPort, int androidPort) {
	string device = getDevice();

	string adbCommand = adb + " -s " + device + " forward tcp:"
			+ to_string(pcPort) + " tcp:" + to_string(androidPort);
	int result = system(adbCommand.c_str());

	if (result == 0) {
		cout << "Port forwarding successful!" << flush;
		return true;
	} else {
		cerr << endl << "Port forwarding failed!";
		return false;
	}
}
