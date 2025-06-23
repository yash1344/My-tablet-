/*
 * TcpServerAsync.h
 *
 *  Created on: Mar 11, 2025
 *      Author: vagha
 */

#ifndef TCPSERVERASYNC_H_
#define TCPSERVERASYNC_H_

#include <winsock2.h>
#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using boost::asio::ip::tcp;
using boost::asio::streambuf;
using std::function;
using std::string;

class TcpServer_Async: public std::enable_shared_from_this<TcpServer_Async> {
private:
	boost::asio::io_context context;
	tcp::socket mSocket;
	tcp::acceptor mAccepter;
	char coordinate_buffer_[20];
	streambuf buffer_;
	char delimiter_ = '\n';
	bool isConnected = false;

public:
	TcpServer_Async();
	virtual ~TcpServer_Async();

	bool startListeningOn(int port);
	void async_accept(function<void(void)> onConnected);
	void async_read_continuous(function<void(string)> onRead);
	void async_read(function<void(string)> onRead);
	void read_until(function<void(string)> onRead);
	void async_write(const std::string &message);
	void async_write(boost::asio::const_buffer buffer);

	void start_reading(function<void(string)> onEachRead);

	boost::asio::io_context& getContext() {
		return context;
	}
};

#endif /* TCPSERVERASYNC_H_ */
