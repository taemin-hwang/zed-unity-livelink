#include "logger/SkeletonLogger.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

void SkeletonLogger::initialize()
{
    isInitialized_ = true;

    // Create logging directory
    createLoggingDirectory("log");
    createLoggingDirectory("log/"+getFormattedDateTime());
}

bool SkeletonLogger::createLoggingDirectory(const std::string& loggingPath)
{
    loggingPath_ = loggingPath;
    return std::filesystem::create_directory(loggingPath);
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
    if (!isInitialized_)
        initialize();

    std::string filePath = loggingPath_ + "/" + std::to_string(frameNum_) + ".json";

    std::ofstream file(filePath);

    frameNum_ += 1;
    file << frameData;
    file.close();
}

std::string SkeletonLogger::readLoggingFile(const std::string& loggingPath, const int& frameNum)
{
    std::string filePath = loggingPath + "/" + std::to_string(frameNum) + ".json";
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}