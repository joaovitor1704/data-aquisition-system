#include <iostream>
#include <boost/asio.hpp>
#include <iomanip>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>

using boost::asio::ip::tcp;


#pragma pack(push, 1)
struct sensorData {
    char sensor_id[32]; // supondo um ID de sensor de até 32 caracteres
    std::time_t timestamp; // timestamp UNIX
    double value; // valor da leitura
};
#pragma pack(pop)

std::time_t string_to_time_t(const std::string& time_string) {
    std::tm tm = {};
    std::istringstream ss(time_string);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::mktime(&tm);
}

std::string time_t_to_string(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    std::ostringstream ss;
    ss << std::put_time(tm, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

void write_sensor_data(const sensorData& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&data), sizeof(sensorData));
        file.close();
    }
}

std::string read_sensor_data(const std::string& filename, const std::string& sensor_id, int n) {
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        std::string sensorDataString;
        sensorData data;
        std::vector<sensorData> sensorDataList;
        while (file.read(reinterpret_cast<char*>(&data), sizeof(sensorData))) {
            if (std::string(data.sensor_id) == sensor_id) {
            sensorDataList.push_back(data);
            }
        }
        
        int startIndex = std::max(0, static_cast<int>(sensorDataList.size()) - n);
        sensorDataString += std::to_string(n);
        for (int i = startIndex; i < sensorDataList.size(); i++) {
            sensorDataString += ";" + time_t_to_string(sensorDataList[i].timestamp) + "|" + 
                std::to_string(sensorDataList[i].value) + "\\r\\n";
        }
        file.close();
        if(sensorDataString.size() > 1){
            return sensorDataString;
        }
        else {
            return "ERROR|SENSOR_NOT_FOUND\\r\\n";
        }
    }
    return "";
}


class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void start()
  {
    read_message();
  }

private:
  void read_message()
  {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, "\r\n",
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::istream is(&buffer_);
            std::string message(std::istreambuf_iterator<char>(is), {});
            std::cout << "Received: " << message << std::endl;
            write_message(message);

            std::vector<std::string> split_message
            std::istringstream iss(message);
            std::string element;

            while(std::getline(iss, element, '|'){

            }

            = split(message, "|");
          }
        });
  }

  void write_message(const std::string& message)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [this, self, message](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            read_message();
          }
        });
  }

  tcp::socket socket_;
  boost::asio::streambuf buffer_;
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    accept();
  }

private:
  void accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket))->start();
          }

          accept();
        });
  }

  tcp::acceptor acceptor_;
};




int main(int argc, char* argv[]) { 
    sensorData sensor1;
    strcpy(sensor1.sensor_id, "sensor7");
    time_t now = std::time(nullptr);
    sensor1.timestamp = now;
    double value = 25.0;
    sensor1.value = value;
    write_sensor_data(sensor1, "sensor_data.dat");
    std::string sensor = read_sensor_data("sensor_data.dat", "546hbvhh", 5);
    std::cout << sensor;


    /*if (argc != 2)
    {
      std::cerr << "Usage: chat_server <port>\n";
      return 1;
    }*/
    boost::asio::io_context io_context;
    server s(io_context, 9000 /*std::atoi(argv[1])*/ );
    io_context.run();


    return 0;
}