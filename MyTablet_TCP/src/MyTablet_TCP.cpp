//============================================================================
// Name        : MyTablet_TCP.cpp
// Author      : Yashkumar Vaghani
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Communication/TCPIP.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>

#include "VStreaming.h"
using namespace std;

int main() {
	float window_scale_setting = 1.25;
	V_Streaming streaming(window_scale_setting);

	TCP_IP tcp_ip;
	if (!tcp_ip.forwardPort()) {
		return 0;
	}

	auto server = make_shared<TcpServer_Async>();

	if (server->startListeningOn(5000)) {
		server->async_accept([&server, &streaming]() {
			streaming.startStreaming(*server.get());
		});

		// Run the io_context in a separate thread
		thread ioThread([&server]() {
			server->getContext().run();
		});

		cout << "Server is running. Press Enter to exit." << endl;

		cin.get();
		cin.get();
		streaming.stopStreaming();
		server->getContext().stop();
	} else {
		cerr << "Failed to start server." << endl;
	}

	return 0;
}
