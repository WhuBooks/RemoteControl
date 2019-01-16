
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "CWebSocket.h"

typedef websocketpp::client<websocketpp::config::asio_client> ws_client;

namespace kagula
{

	class connection_metadata {
	public:
		typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

		connection_metadata(websocketpp::connection_hdl hdl, std::string uri)
			: m_hdl(hdl)
			, m_status("Connecting")
			, m_uri(uri)
			, m_server("N/A")
		{}

		void on_open(ws_client *client, websocketpp::connection_hdl hdl) {
			m_status = "Open";

			ws_client::connection_ptr con = client->get_con_from_hdl(hdl);
			m_server = con->get_response_header("Server");
		}

		// if connection failed, the function will be invoke.
		void on_fail(ws_client *client, websocketpp::connection_hdl hdl) {
			m_status = "Failed";

			ws_client::connection_ptr con = client->get_con_from_hdl(hdl);
			m_server = con->get_response_header("Server");
			m_error_reason = con->get_ec().message();
		}

		void on_close(ws_client *client, websocketpp::connection_hdl hdl) {
			m_status = "Closed";
			ws_client::connection_ptr con = client->get_con_from_hdl(hdl);
			std::stringstream s;
			s << "close code: " << con->get_remote_close_code() << " ("
				<< websocketpp::close::status::get_string(con->get_remote_close_code())
				<< "), close reason: " << con->get_remote_close_reason();
			m_error_reason = s.str();
		}

		void on_message(websocketpp::connection_hdl, ws_client::message_ptr msg) {
			std::vector<std::string> temp;
			if (msg->get_opcode() == websocketpp::frame::opcode::text) {
				temp.push_back(/*"<< " +*/ msg->get_payload());
			}
			else {
				temp.push_back(/*"<< " +*/ websocketpp::utility::to_hex(msg->get_payload()));
			}
			m_messages = temp;
		}

		websocketpp::connection_hdl get_hdl() const {
			return m_hdl;
		}

		std::string get_status() const {
			return m_status;
		}

		std::string get_uri() const {
			return m_uri;
		}

		std::vector<std::string> get_message() const {
			return m_messages;
		}

		void record_sent_message(std::string message) {
			m_messages.push_back(">> " + message);
		}

		friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data);
	private:
		websocketpp::connection_hdl m_hdl;
		std::string m_status;
		std::string m_uri;
		std::string m_server;
		std::string m_error_reason;
		std::vector<std::string> m_messages;
	};

	std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
		out << "> URI: " << data.m_uri << "\n"
			<< "> Status: " << data.m_status << "\n"
			<< "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
			<< "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n";
		out << "> Messages Processed: (" << data.m_messages.size() << ") \n";

		//std::vector<std::string>::const_iterator it;
		//for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
		//	out << *it << "\n";
		//}

		return out;
	}

	ws_client g_wsEndPoint;
	connection_metadata::ptr g_wsClientConnection;

	websocketpp::lib::shared_ptr<websocketpp::lib::thread> g_threadWS;

	websocket_endpoint::websocket_endpoint() {
		g_wsEndPoint.clear_access_channels(websocketpp::log::alevel::all);
		g_wsEndPoint.clear_error_channels(websocketpp::log::elevel::all);

		g_wsEndPoint.init_asio();
		g_wsEndPoint.start_perpetual();

		g_threadWS = websocketpp::lib::make_shared<websocketpp::lib::thread>(&ws_client::run, &g_wsEndPoint);
	}

	websocket_endpoint::~websocket_endpoint() {
		g_wsEndPoint.stop_perpetual();

		if (g_wsClientConnection->get_status() == "Open") {
			// Only close open connections
			websocketpp::lib::error_code ec;
			g_wsEndPoint.close(g_wsClientConnection->get_hdl(), websocketpp::close::status::going_away, "", ec);
			if (ec) {
				std::cout << "> Error closing ws connection " << g_wsClientConnection->get_uri() << " :" << ec.message() << std::endl;
			}
		}

		g_threadWS->join();
	}

	int websocket_endpoint::connect(std::string const & uri) {
		websocketpp::lib::error_code ec;

		ws_client::connection_ptr pConnection = g_wsEndPoint.get_connection(uri, ec);

		if (ec) {
			std::cout << "> Connect initialization error: " << ec.message() << std::endl;
			return -1;
		}

		g_wsClientConnection = websocketpp::lib::make_shared<connection_metadata>(pConnection->get_handle(), uri);

		pConnection->set_open_handler(websocketpp::lib::bind(
			&connection_metadata::on_open,
			g_wsClientConnection,
			&g_wsEndPoint,
			websocketpp::lib::placeholders::_1
		));
		pConnection->set_fail_handler(websocketpp::lib::bind(
			&connection_metadata::on_fail,
			g_wsClientConnection,
			&g_wsEndPoint,
			websocketpp::lib::placeholders::_1
		));
		pConnection->set_close_handler(websocketpp::lib::bind(
			&connection_metadata::on_close,
			g_wsClientConnection,
			&g_wsEndPoint,
			websocketpp::lib::placeholders::_1
		));
		pConnection->set_message_handler(websocketpp::lib::bind(
			&connection_metadata::on_message,
			g_wsClientConnection,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		g_wsEndPoint.connect(pConnection);

		return 0;
	}

	void close(websocketpp::close::status::value code, std::string reason) {
		websocketpp::lib::error_code ec;

		g_wsEndPoint.close(g_wsClientConnection->get_hdl(), code, reason, ec);
		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}

	void websocket_endpoint::close()
	{
		if (g_wsClientConnection->get_status() == "Open")
		{
			int close_code = websocketpp::close::status::normal;
			kagula::close(close_code, "");
		}
	}

	void websocket_endpoint::send(std::string message) {
		websocketpp::lib::error_code ec;

		g_wsEndPoint.send(g_wsClientConnection->get_hdl(), message, websocketpp::frame::opcode::text, ec);
		if (ec) {
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			return;
		}

		g_wsClientConnection->record_sent_message(message);
	}

	void websocket_endpoint::show(std::vector<std::string> &msg)
	{
		//std::cout << *g_wsClientConnection << std::endl;
		msg = g_wsClientConnection->get_message();
	}
}
