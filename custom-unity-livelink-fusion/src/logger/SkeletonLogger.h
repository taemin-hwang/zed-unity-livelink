#ifndef _SKELETON_LOGGER_H_
#define _SKELETON_LOGGER_H_

#include <mutex>
#include <string>

std::string addZeroPadding(int number, int width);
class SkeletonLogger {
public:
    SkeletonLogger() = default;
    virtual ~SkeletonLogger() = default;

    void initialize();                                                                // Initialize the frame logger
    void writeLoggingFile(const std::string& frameData);                              // Write logging file
    std::string readLoggingFile(const std::string& filePath, const int& frameNum);    // Read logging file

private:
    bool createLoggingDirectory(const std::string& loggingPath);    // Create logging directory
    std::string getFormattedDateTime();                             // Get formatted date and time

private:
    bool isInitialized_ = false;
    std::string loggingPath_;
    std::string loggingFile_;
    int frameNum_ = 0;
    std::mutex m_;
};

#endif // _SKELETON_LOGGER_H_