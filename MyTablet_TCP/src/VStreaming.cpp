/*
 * VStreaming.cpp
 *
 *  Created on: Mar 14, 2025
 *      Author: vagha
 */

#include "VStreaming.h"

#include <winsock2.h>
using namespace std;

V_Streaming::V_Streaming(float window_scale_setting) {
	this->screenCapture = CaptureScreen(window_scale_setting);
	this->cursor = MyCursor(window_scale_setting);
	cursor.upadteCursor();
	this->rect = MyRectangle();
	this->isStreaming = false;
}

V_Streaming::~V_Streaming() {
	stopStreaming();
}

void V_Streaming::startStreaming(TcpServer_Async &server) {
	rect.setBoundery(GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN));
	rect.setRect(cursor.x, cursor.y, 1204, 540);

	isStreaming = true;
	capture_thread = thread(
			[this]() {
				this->screenCapture.initialize();
				while (isStreaming) {
					try {
						jpeg_queue.push_back(
								screenCapture.captureCustomScreen_jpeg(rect,
										50));
					} catch (const exception &e) {
						cerr << "capture_thread: ";
						cerr << e.what();
						isStreaming = false;
					}
					this->cursor.upadteCursor();
					rect.setRect(cursor.x, cursor.y, 1204, 500);
					this_thread::sleep_for(chrono::milliseconds(16));
				}
			});

	send_thread = thread([this, &server]() {
		ostringstream oss;
		JPEG image_data;
		std::vector<uint8_t> combined_data;  // Reusable buffer

		while (!jpeg_queue.empty() || isStreaming) {
			if (jpeg_queue.empty()) {
				this_thread::sleep_for(std::chrono::milliseconds(5));
				continue;
			}

			image_data = jpeg_queue.front();
			jpeg_queue.pop_front();

			// Clear the combined_data vector
			combined_data.clear();

			oss.str("");        // Clear the ostringstream content
			oss.clear();        // Clear any error flags
			oss << "imgS:{" << image_data.size() << "}";
//			oss << "imgS:{" << static_cast<int>(image_data.size()) << "}";
			std::string header = oss.str();
//			cout << endl << header;

			// Reserve enough memory once, if possible, or at least for this iteration
			combined_data.clear();
			combined_data.reserve(header.size() + image_data.size());

			// Append header bytes
			combined_data.insert(combined_data.end(), header.begin(), header.end());
			combined_data.insert(combined_data.end(), image_data.begin(),
					image_data.end());

			try {
				server.async_write(boost::asio::buffer(combined_data));
			} catch (const exception &e) {
				cerr << "send_frame_thread: ";
				cerr << e.what();
			}
		}
	});

	receive_thread = thread([this, &server]() {
		server.read_until([this](string data) {
			cout << endl << "read: " << data;
		});
//		server.async_read_continuous([this](string data) {
//			cout << "read: " << data << flush;
//		});
	});

}

bool V_Streaming::isIsStreaming() const {
	return isStreaming;
}

void V_Streaming::stopStreaming() {
	isStreaming = false;
	if (capture_thread.joinable()) {
		capture_thread.join();
	}
	if (send_thread.joinable()) {
		send_thread.join();
	}
	if (receive_thread.joinable()) {
		receive_thread.join();
	}
//	jpeg_queue.clear();
}
