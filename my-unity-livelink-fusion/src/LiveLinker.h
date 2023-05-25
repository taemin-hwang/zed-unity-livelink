// Referenced open source code from Stereolabs (https://www.stereolabs.com)

#ifndef _LIVE_LINKER_H_
#define _LIVE_LINKER_H_

// OpenGL Viewer(!=0) or no viewer (0)
#define DISPLAY_OGL 0

// ZED include
#include "client/SenderRunner.h"
#include "viewer/GLViewer.h"
#include "transfer/PracticalSocket.h"
#include "transfer/json.h"
#include "logger/SkeletonLogger.h"
#include "config/ConfigParser.h"
#include <sl/Camera.hpp>
#include <iostream>
#include <memory>

// Defines the Coordinate system and unit used in this sample
static const sl::COORDINATE_SYSTEM COORDINATE_SYSTEM = sl::COORDINATE_SYSTEM::LEFT_HANDED_Y_UP;
static const sl::UNIT UNIT = sl::UNIT::METER;
static const sl::BODY_TRACKING_MODEL BODY_MODEL = sl::BODY_TRACKING_MODEL::HUMAN_BODY_ACCURATE;
static const sl::BODY_FORMAT BODY_FORMAT = sl::BODY_FORMAT::BODY_70;

class LiveLinker {
public:
    LiveLinker();
    virtual ~LiveLinker() = default;

    bool Initialize(int argc, char **argv);
    bool Start();
    void Stop();

private:
    nlohmann::json getJson(sl::FusionMetrics metrics, sl::Bodies& bodies, sl::BODY_FORMAT body_format);
    nlohmann::json bodyDataToJson(sl::BodyData body);
    nlohmann::json bodyDataToJsonMeter(sl::BodyData body);
    void print(string msg_prefix, sl::ERROR_CODE err_code = sl::ERROR_CODE::SUCCESS, string msg_suffix = "");

private:
    std::vector<sl::CameraIdentifier> cameras_;
    std::vector<SenderRunner> clients_;
    GLViewer viewer_;
    std::shared_ptr<ConfigParser> config_parser_;
    std::unique_ptr<SkeletonLogger> logger_;
    sl::Fusion fusion_;
    bool is_initialized_ = false;
    std::string ip_addr = "";
    int port = 0;
    bool is_enable_viewer_ = false;

    int argc_;
    char **argv_;
};

#endif // _LIVE_LINKER_H_