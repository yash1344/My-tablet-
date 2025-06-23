/*
 * TcpClient.h
 *
 *  Created on: Mar 8, 2025
 *      Author: vagha
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class TcpServer: public std::enable_shared_from_this<TcpServer> {

private:
	boost::asio::io_context context;
	tcp::socket mSocket;
	tcp::acceptor mAccepter;
	boost::system::error_code error_code;

	char coordinate_buffer_[1024];
	bool isConnected = false;

	void async_read_coordinates();

public:
	TcpServer();
	virtual ~TcpServer();

	bool startListeningOn(int port);
	bool async_accept();

	void startReading();

	std::string read();
	int write(std::string msg);
};

#endif /* TCPSERVER_H_ */
