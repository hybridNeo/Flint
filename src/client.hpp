#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#define MEM_LISTNER_PORT 10114
#define INTRO_PORT 10115
#define UPDATE_PORT 10116
#define DEBUG 0     
#define PUTFILE_PORT 10117
#define FTP_SERVER_PORT 10118
#define MONITOR_SERVER_PORT 10119

using boost::asio::ip::udp;

std::vector<std::string> where_to(std::string request);
