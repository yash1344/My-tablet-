/*
 * VStreaming.h
 *
 *  Created on: Mar 14, 2025
 *      Author: vagha
 */

#ifndef VSTREAMING_H_
#define VSTREAMING_H_
#include <thread>
#include <deque>
#include <sstream>

#include "Communication/TcpServerAsync.h"
#include "Screen/CaptureScreen.h"
#include "Screen/Utility/MyCursor.h"

using JPEG = vector<BYTE>;

class V_Streaming {
private:
	bool isStreaming;
	CaptureScreen screenCapture;
	MyCursor cursor;
	MyRectangle rect;

	std::deque<JPEG> jpeg_queue;

	std::thread capture_thread;
	std::thread send_thread;
	std::thread receive_thread;

public:
	V_Streaming(float window_scale_setting = 1);
	virtual ~V_Streaming();

	void startStreaming(TcpServer_Async &server);
	void stopStreaming();
	bool isIsStreaming() const;
};

#endif /* VSTREAMING_H_ */
