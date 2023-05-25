#include "logger/SkeletonLogger.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

void SkeletonLogger::initialize()
{
    isInitialized_ = true;

    // Create logging directory
    createLoggingDirectory("../log");
    loggingPath_ = "../log/" + getFormattedDateTime();
    createLoggingDirectory(loggingPath_);
}

bool SkeletonLogger::createLoggingDirectory(const std::string& loggingPath)
{
    int ret = -1;
    ret = std::filesystem::create_directory(loggingPath);

    std::cout << "Creating logging directory: " << loggingPath << " : " << ret << std::endl;

    return ret;
}

std::string SkeletonLogger::getFormattedDateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%y-%m-%d-%H-%M");
    return ss.str();
}

void SkeletonLogger::writeLoggingFile(const std::string& frameData)
{
    std::unique_lock<std::mutex> lock(m_);
    if (!isInitialized_)
        initialize();

    std::string filePath = loggingPath_ + "/" + addZeroPadding(frameNum_, 8) + ".json";

    std::ofstream file(filePath);

    frameNum_ += 1;
    file << frameData;
    file.close();
}

std::string SkeletonLogger::readLoggingFile(const std::string& loggingPath, const int& frameNum)
{
    std::string filePath = loggingPath + "/" + addZeroPadding(frameNum, 8) + ".json";
    std::cout << "Reading logging file: " << filePath << std::endl;
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string addZeroPadding(int number, int width)
{
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << number;
    return oss.str();
}
