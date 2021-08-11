/************************************************************************
 * @ObjectName: main.cpp
 * @Description:
 * @Author: xzf
 * @Date: 2021/8/11
 ***********************************************************************/
#include <iostream>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#include <p2p/client.h>
#include <vsnc_utils/utils.h>
#include <vsnc_utils/memory.h>


static int creatUdpSocket()
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		std::cout << "UDP::Socket() failed." << std::endl;
		return -1;
	}
	return sock;
}

static void worker(bool& run)
{
	char c = '\0';
	do {
		std::cin >> c;
	} while ('q' != c);
	run = false;
}


int main(int argc, char* argv[])
{
	//≥ı ºªØWSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) return 0;

	auto udp_sock = creatUdpSocket();
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4004);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	if (bind(udp_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
		std::cout << "UDP::bind() failed." << std::endl;
		return 0;
	}

	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4002);
	inet_pton(AF_INET, "192.168.3.229", &sin.sin_addr);
	int len = sizeof(sin);

	size_t Max_Len = 1504;
	char* rBuf = (char*)malloc(Max_Len);
	vsnc::p2p::Client client(42, "52.130.75.26", 10000, 0);
	bool run = true;
	std::thread quit(&worker, std::ref(run));
	vsnc::utils::BasicMemory<char> mem(rBuf, Max_Len);
	int64_t ts = 0;
	while (run) {
		vsnc::utils::__sleep_seconds(1);
		auto state = client.GetState();
		switch (state)
		{
		case vsnc::p2p::vsnc_p2p_state::OFFLINE:
			std::cout << "OFFLINE" << std::endl;
			break;
		case vsnc::p2p::vsnc_p2p_state::FREE:
			client.Connect(41);
			std::cout << "FREE" << std::endl;
			break;
		case vsnc::p2p::vsnc_p2p_state::REQUESTING:
			std::cout << "REQUESTING" << std::endl;
			break;
		case vsnc::p2p::vsnc_p2p_state::CONNECTING:
			std::cout << "CONNECTING" << std::endl;
			break;
		case vsnc::p2p::vsnc_p2p_state::CONNECTED:
			std::cout << "CONNECTED" << std::endl;
			while (run) {
				auto _size = client.Receive(mem, ts,200);
				//std::cout << _size << std::endl;
				if (_size < 0) {
					std::cout << "recvfrom close" << std::endl;
					break;
				}
				else if (_size == 0) {
					std::cout << "client shutdown..." << std::endl;
					break;
				}
				else {
					auto data_len = sendto(udp_sock, mem.Data(), _size, 0, reinterpret_cast<sockaddr*>(&sin), len);
					//std::cout << " len: " << data_len << std::endl;
					if (data_len < 0) {
						break;
					}
				}
			}
			break;
		default:
			std::cout << "UNKNOWN STATE" << std::endl;
			break;
		}
	}
	quit.join();
	WSACleanup();
	return 0;
}
