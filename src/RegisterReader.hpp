#pragma once
#include "SensorMock.hpp"
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace SensorProject
{
    class RegisterReader
    {
    public:
        RegisterReader(std::shared_ptr<std::vector<LogRecord>> sharedBuffer, std::shared_ptr<std::mutex> sharedMutex, std::string_view sensorID, boost::asio::io_context& ioContext, uint16_t port);
        ~RegisterReader() = default;

        void ReadRegisters();
    private:

        void SerializeRegisters();
        
        class Session : public std::enable_shared_from_this<Session> {
        public:
            Session(tcp::socket socket);

            void Start();

        private:
            void Read();
            void Write(std::size_t length);

            tcp::socket socket_;
            enum { max_length = 1024 };
            char data_[max_length];
        };

        std::shared_ptr<std::vector<LogRecord>> m_SharedBuffer;
        std::shared_ptr<std::mutex> m_SharedMutex;
        std::string m_SensorID;
        std::vector<LogRecord> m_PrivateBuffer;
        tcp::acceptor m_Acceptor;
    };
}