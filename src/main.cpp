#include <cstdlib>
#include <iostream>
#include <utility>
#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "SensorMock.hpp"
#include "RegisterReader.hpp"

void RunContext(boost::asio::io_context* context)
{
    context->run();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
       std::cerr << "Usage: chat_server <port>\n";
       return 1;
    }

    std::shared_ptr<std::mutex> sharedMutex;
    sharedMutex.reset(new std::mutex());
    std::shared_ptr<std::vector<SensorProject::LogRecord>> sharedBuffer;
    sharedBuffer.reset(new std::vector<SensorProject::LogRecord>());
    boost::asio::io_context io_context;

    SensorProject::SensorMock mockedSensor("a5f87451a5f87451a5f87451a5f87451", 1, 2, 2, sharedBuffer, sharedMutex);
    SensorProject::RegisterReader registerReader(sharedBuffer, sharedMutex, "a5f87451a5f87451a5f87451a5f87451", io_context, std::atoi(argv[1]));
    std::thread* sensorThread;
    std::thread* bufferThread;
    std::thread* serverThread;
    sensorThread = new std::thread(std::bind(&SensorProject::SensorMock::SimSensor, &mockedSensor));
    bufferThread = new std::thread(std::bind(&SensorProject::RegisterReader::ReadRegisters, &registerReader));
    serverThread = new std::thread(RunContext, &io_context);
    serverThread->join();
    bufferThread->join();
    sensorThread->join();
    delete serverThread;
    delete bufferThread;
    delete sensorThread;
}
