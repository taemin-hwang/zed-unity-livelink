#ifndef _SKELETON_LOGGER_H_
#define _SKELETON_LOGGER_H_

#include <string>

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
};

#endif // _SKELETON_LOGGER_H_