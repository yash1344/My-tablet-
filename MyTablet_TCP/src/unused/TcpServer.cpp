/*
 * TcpClient.cpp
 *
 *  Created on: Mar 8, 2025
 *      Author: vagha
 */

#include "TcpServer.h"

using namespace std;

TcpServer::~TcpServer() {

}

bool TcpServer::startListeningOn(int port) {
	try {
		mAccepter = tcp::acceptor(context, tcp::endpoint(tcp::v4(), port));
		cout << endl << "Listening to " << mAccepter.local_endpoint() << "..."
				<< flush;
		mAccepter.accept(mSocket);
	} catch (const boost::system::system_error &e) {
		return false;
	}
	return true;
}

std::string TcpServer::read() {
	size_t length = mSocket.read_some(boost::asio::buffer(coordinate_buffer_));
	return string(coordinate_buffer_, length);
}

void TcpServer::async_read_coordinates() {
	auto self(shared_from_this());
	boost::asio::async_read(mSocket, boost::asio::buffer(coordinate_buffer_),
			[this, self](const boost::system::error_code &error,
					std::size_t bytes_transferred) {
				if (!error) {
					// Process coordinates
					std::string coordinates(coordinate_buffer_,
							bytes_transferred);
					std::cout << "Received coordinates: " << coordinates
							<< std::endl;

					// Continue reading
					async_read_coordinates();
				} else {
					std::cerr << "Error reading coordinates: "
							<< error.message() << std::endl;
				}
			}
	);
}

void TcpServer::startReading() {
	//start reading coordinates
	async_read_coordinates();
}

int TcpServer::write(std::string msg) {
	return boost::asio::write(mSocket, boost::asio::buffer(msg));
}

TcpServer::TcpServer() :
		mSocket(context), mAccepter(context) {
}

bool TcpServer::async_accept() {
	auto self(shared_from_this());
	mAccepter.async_accept(mSocket,
			[this, self](const boost::system::error_code &error) {
				if (!error) {
					// Start the session
					return true;
				} else {
					return false;
				}
				// Continue accepting new connections
//				async_accept();
			}
	);
	return false;
}
