#pragma once
#include<vector>
namespace kagula
{
	class websocket_endpoint {
	public:
		websocket_endpoint();
		~websocket_endpoint();

		int connect(std::string const & uri);
		void close();

		void send(std::string message);
		void show(std::vector<std::string> &msg);
	};
}

