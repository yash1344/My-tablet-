/*
 * TcpServerAsyncAsync.cpp
 *
 *  Created on: Mar 11, 2025
 *      Author: vagha
 */

#include "TcpServerAsync.h"

using namespace std;

TcpServer_Async::TcpServer_Async() :
		mSocket(context), mAccepter(context) {
}

TcpServer_Async::~TcpServer_Async() {
	if (mSocket.is_open()) {
		mSocket.close();
	}
}

bool TcpServer_Async::startListeningOn(int port) {
	try {
		mAccepter = tcp::acceptor(context, tcp::endpoint(tcp::v4(), port));
		cout << endl << "Listening on " << mAccepter.local_endpoint() << "..."
				<< endl;

	} catch (const boost::system::system_error &e) {
		cerr << "Error starting server: " << e.what() << endl;
		return false;
	}
	return true;
}

void TcpServer_Async::async_accept(function<void(void)> onConnected) {
	auto self(shared_from_this());
	mAccepter.async_accept(
			[this, self, onConnected](const boost::system::error_code &error,
					tcp::socket socket) {
				if (!error) {
					cout << "Client connected!" << endl;
					mSocket = std::move(socket);
					isConnected = true;

					onConnected();
				} else {
					cerr << "Error accepting connection: " << error.message()
							<< endl;
				}
			}
	);
}

void TcpServer_Async::async_read_continuous(function<void(string)> onRead) {
	auto self(shared_from_this());
	boost::asio::async_read_until(mSocket, buffer_, delimiter_,
			[this, self, onRead](const boost::system::error_code &error,
					std::size_t bytes_transferred) {
				if (!error) {
					cout << endl << "here...";
					// Extract the message up to the delimiter
//					std::string message { buffers_begin(buffer_.data()),
//							buffers_begin(buffer_.data()) + bytes_transferred
//									- delimiter_.length() };
//					onRead(message);
					buffer_.consume(bytes_transferred); // Remove processed data from the buffer

					//for continouse reading
					async_read_continuous(onRead);
				} else {
					std::cerr << "Error: " << error.message() << std::endl;
				}
			}
	);
}

void TcpServer_Async::async_read(function<void(string)> onRead) {
	auto self(shared_from_this());
	boost::asio::async_read_until(mSocket, buffer_, delimiter_,
			[this, self, onRead](const boost::system::error_code &error,
					std::size_t bytes_transferred) {
				if (!error) {
					cout << endl << "here...";
					// Extract the message up to the delimiter
//					std::string message { buffers_begin(buffer_.data()),
//							buffers_begin(buffer_.data()) + bytes_transferred
//									- delimiter_.length() };
//					onRead(message);
					buffer_.consume(bytes_transferred); // Remove processed data from the buffer
				} else {
					std::cerr << "Error: " << error.message() << std::endl;
				}
			}
	);
}

void TcpServer_Async::read_until(function<void(string)> onRead) {
	boost::asio::streambuf buffer;

	try {
		while (mSocket.is_open()) {
			// Blocking read until delimiter is found
			size_t bytes_read = boost::asio::read_until(mSocket, buffer,
					delimiter_);
			cout << endl << "Size: " << bytes_read;

			// Extract the message (including proper buffer handling)
			std::string message;
			message.resize(bytes_read);
			buffer.sgetn(&message[0], bytes_read); // Directly consume from buffer

			// Remove the delimiter if present at the end
			if (!message.empty() && message.back() == delimiter_) {
				message.pop_back();
			}

			// Call the callback
			onRead(message);
		}
	} catch (const boost::system::system_error &e) {
		if (e.code() != boost::asio::error::eof) {
			std::cerr << "Read error: " << e.what() << std::endl;
		} else {
			std::cout << "Client disconnected" << std::endl;
		}
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void TcpServer_Async::async_write(const std::string &msg) {
	if (!isConnected) {
		cerr << "No client connected!" << endl;
		return;
	}

	auto self(shared_from_this());
	boost::asio::async_write(mSocket, boost::asio::buffer(msg),
			[this, self](const boost::system::error_code &error,
					std::size_t bytes_transferred) {
				if (!error) {
					cout << "Sent " << bytes_transferred << " bytes." << endl;
				} else {
					cerr << "Error writing data: " << error.message() << endl;
					isConnected = false;
					mSocket.close();
				}
			}
	);
}

void TcpServer_Async::async_write(boost::asio::const_buffer buffer) {
	if (!isConnected) {
		cerr << "No client connected!" << endl;
		return;
	}

	auto self(shared_from_this());
	boost::asio::async_write(mSocket, buffer,
			[this, self](const boost::system::error_code &error,
					std::size_t bytes_transferred) {
				if (!error) {
					cout << "Sent " << bytes_transferred << " bytes." << endl;

					// Continue Writing
				} else {
					cerr << "Error writing data: " << error.message() << endl;
					isConnected = false;
					mSocket.close();
				}
			}
	);
}

void TcpServer_Async::start_reading(function<void(string)> onEachRead) {
	while (isConnected) {

	}
}
