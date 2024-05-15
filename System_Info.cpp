#include <iostream>

#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_category.hpp>
#include <boost/asio.hpp>

void SystemInfo() {
   boost::asio::io_service io_service;
   boost::asio::ip::tcp::resolver resolver(io_service);
   boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
   boost::system::error_code ec;
   boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, ec);


   if (!ec) {
      std::cout << "hostname:     " << iter->host_name() << '\n';

      while (iter != boost::asio::ip::tcp::resolver::iterator()) {
         boost::asio::ip::tcp::endpoint endpoint = *iter++;
         if (endpoint.protocol() == boost::asio::ip::tcp::v4()) {
            std::string ip_address = endpoint.address().to_string();
            std::cout << "IPv4 address: " << ip_address << std::endl;
            break; // for testing only the use the first IPv4-address
            }
         }
      }
   else {
      std::cerr << "unknown hostname\n";
      }
   }
