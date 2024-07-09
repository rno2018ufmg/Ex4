#include "SensorMock.hpp"
#include <random>
#include <iostream>

SensorProject::SensorMock::SensorMock(std::string_view sensorID, size_t sensorPause, double sensorRange, double sensorOffset, std::shared_ptr<std::vector<LogRecord>> sharedBuffer, std::shared_ptr<std::mutex> sharedMutex) :
    m_CurrentLog(sensorID), m_SensorPause(sensorPause), m_SensorAmplitude(sensorRange), m_SensorOffset(sensorOffset), m_SharedBuffer(sharedBuffer), m_SharedMutex(sharedMutex)
{
    m_Logger.reset(new std::fstream("phonebook.dat", std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app));
    if (!m_Logger->is_open())
    {
        exit(5);
    }
    if(m_CurrentLog.sensor_id.length() != 32)
        exit(1);
}

void SensorProject::SensorMock::SimSensor()
{
    while(true)
    {
        auto recordStart = std::chrono::steady_clock::now();
        GenRandomLog();
        RecordLog();
        auto recordEnd = std::chrono::steady_clock::now();
        int64_t timeEllapsed = std::chrono::duration_cast<std::chrono::milliseconds>(recordEnd - recordStart).count();
        while((double)timeEllapsed < (double)(m_SensorPause * 1e3))
        {
            recordEnd = std::chrono::steady_clock::now();
            timeEllapsed = std::chrono::duration_cast<std::chrono::milliseconds>(recordEnd - recordStart).count();
        }
    }
}

void SensorProject::SensorMock::GenRandomLog()
{
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    m_CurrentLog.timestamp = std::chrono::system_clock::now();
    m_CurrentLog.value = (dist(mt) * m_SensorAmplitude) + m_SensorOffset;
}

void SensorProject::SensorMock::RecordLog()
{
    m_SharedMutex->lock();
    m_SharedBuffer->push_back(m_CurrentLog);
    m_SharedMutex->unlock();
    //m_Logger->seekp(0, std::ios_base::end);
    m_Logger->write((char *)m_CurrentLog.sensor_id.c_str(), 32);
    m_Logger->write((char *)&m_CurrentLog.timestamp, sizeof(std::chrono::system_clock::time_point));
    m_Logger->write((char *)&m_CurrentLog.value, sizeof(double));
}