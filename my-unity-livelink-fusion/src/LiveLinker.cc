#include "LiveLinker.h"
#include <utility>

LiveLinker::LiveLinker() {
    config_parser_ = std::make_shared<ConfigParser>("../etc/config.json");
    logger_ = std::make_unique<SkeletonLogger>();

    is_enable_viewer_ = config_parser_->IsViewerOn();
}

bool LiveLinker::Initialize(int argc, char **argv)
{
    std::cout << "LiveLinker::Initialize()" << std::endl;

    argc_ = argc;
    argv_ = argv;

    if (argc_ != 2)
    {
        std::cout << "Need a Localization file in input" << std::endl;
        return EXIT_FAILURE;
    }

    std::string json_config_filename = "";
    try
    {
        json_config_filename = std::string(argv_[1]);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto configurations = sl::readFusionConfigurationFile(json_config_filename, COORDINATE_SYSTEM, UNIT);

    if (configurations.empty())
    {
        std::cout << "Empty configuration File." << std::endl;
        return EXIT_FAILURE;
    }

    // initialize the ZED camera
    for (int i = 0; i < configurations.size(); ++i) {
        SenderRunner obj(config_parser_);
        clients_.emplace_back(std::move(obj));
    }

    int id_ = 0;
    for (auto conf : configurations)
    {
        // if the ZED camera should run locally, then start a thread to handle it
        if (conf.communication_parameters.getType() == sl::CommunicationParameters::COMM_TYPE::INTRA_PROCESS)
        {
            std::cout << "Try to open ZED " << conf.serial_number << ".." << std::flush;
            auto state = clients_[id_++].open(conf.input_type, BODY_FORMAT);
            if (state)
                std::cout << ". ready !" << std::endl;
        }
        else
            std::cout << "Try to open ZED " << conf.serial_number << " over a local network" << std::endl;
    }

    // start camera threads
    for (auto &it : clients_)
        it.start();

    // Now that the ZED camera are running, we need to initialize the fusion module
    sl::InitFusionParameters init_params;
    init_params.coordinate_units = UNIT;
    init_params.coordinate_system = COORDINATE_SYSTEM;
    init_params.verbose = true;

    // initialize it
    fusion_.init(init_params);

    // subscribe to every cameras of the setup to internally gather their data
    for (auto &it : configurations)
    {
        sl::CameraIdentifier uuid(it.serial_number);
        // to subscribe to a camera you must give its serial number, the way to communicate with it (shared memory or local network), and its world pose in the setup.
        auto state = fusion_.subscribe(uuid, it.communication_parameters, it.pose);
        if (state != sl::FUSION_ERROR_CODE::SUCCESS)
            std::cout << "Unable to subscribe to " << std::to_string(uuid.sn) << " . " << state << std::endl;
        else
            cameras_.push_back(uuid);
    }

    // check that at least one camera is connected
    if (cameras_.empty())
    {
        std::cout << "no connections " << std::endl;
        return EXIT_FAILURE;
    }

    // as this sample shows how to fuse body detection from the multi camera setup
    // we enable the Body Tracking module with its options
    sl::BodyTrackingFusionParameters body_fusion_init_params;
    body_fusion_init_params.enable_tracking = true;
    body_fusion_init_params.enable_body_fitting = true;
    fusion_.enableBodyTracking(body_fusion_init_params);

    // initialize the viewer
    if (is_enable_viewer_)
    {
        viewer_.init(argc_, argv_);
    }

    is_initialized_ = true;

    return EXIT_SUCCESS;
}

/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// -------------------------------- MAIN LOOP ---------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
bool LiveLinker::Start()
{
    if (!is_initialized_) {
        std::cout << "LiveLinker::Start() failed. Not initialized." << std::endl;
        return false;
    }

    std::cout << "LiveLinker::Start()" << std::endl;
    // define fusion behavior
    sl::BodyTrackingFusionRuntimeParameters body_tracking_runtime_parameters;
    // be sure that the detection skeleton is complete enough
    body_tracking_runtime_parameters.skeleton_minimum_allowed_keypoints = 7;

    // we can also want to retrieve skeleton seen by multiple camera, in this case at least half of them
    body_tracking_runtime_parameters.skeleton_minimum_allowed_camera = cameras_.size() / 2.;

    // make skeleton smooth
    body_tracking_runtime_parameters.skeleton_smoothing = 0.05;

    // fusion outputs
    sl::Bodies fused_bodies;
    std::map<sl::CameraIdentifier, sl::Bodies> camera_raw_data;
    sl::FusionMetrics metrics;

    bool run = true;

    // ----------------------------------
    // UDP to Unity----------------------
    // ----------------------------------
    std::string servAddress = config_parser_->GetAddress();
    int servPort = config_parser_->GetPort();
    UDPSocket sock;

    sock.setMulticastTTL(1);

    // servAddress = "230.0.0.1";
    // servPort = 20001;

    std::cout << "Sending fused data at " << servAddress << ":" << servPort << std::endl;

    // run the fusion as long as the viewer is available.
    while (run) {
        // run the fusion process (which gather data from all camera, sync them and process them)
        if (fusion_.process() == sl::FUSION_ERROR_CODE::SUCCESS) {
            // Retrieve fused body
            fusion_.retrieveBodies(fused_bodies, body_tracking_runtime_parameters);
            // for debug, you can retrieve the data send by each camera
            for (auto &id : cameras_)
                fusion_.retrieveBodies(camera_raw_data[id], body_tracking_runtime_parameters, id);
            // get metrics about the fusion process for monitoring purposes
            fusion_.getProcessMetrics(metrics);

            if (is_enable_viewer_) {
                // update the 3D view
                viewer_.updateBodies(fused_bodies, camera_raw_data, metrics);
            }
            if (fused_bodies.is_new) {
                try {
                    // ----------------------------------
                    // UDP to Unity----------------------
                    // ----------------------------------
                    std::string data_to_send = getJson(metrics, fused_bodies, fused_bodies.body_format).dump();
                    sock.sendTo(data_to_send.data(), data_to_send.size(), servAddress, servPort);
                }
                catch (SocketException &e) {

                    cerr << e.what() << endl;
                }
            }
        }

        if (is_enable_viewer_) {
            run = viewer_.isAvailable();
        }
        sl::sleep_ms(10);
    }

    if (is_enable_viewer_) {
        viewer_.exit();
    }

    for (auto &it : clients_)
        it.stop();

    fusion_.close();

    return EXIT_SUCCESS;
}

void LiveLinker::Stop()
{
    std::cout << "LiveLinker::Stop()" << std::endl;
}

/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------- DATA FORMATTING ------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------

// If the sender encounter NaN values, it sends 0 instead.
nlohmann::json LiveLinker::bodyDataToJsonMeter(sl::BodyData body)
{
    nlohmann::json res;

    res["id"] = body.id;
    res["tracking_state"] = body.tracking_state;
    res["action_state"] = body.action_state;

    res["position"] = nlohmann::json::object();
    res["position"]["x"] = isnan(body.position.x) ? 0 : body.position.x;
    res["position"]["y"] = isnan(body.position.y) ? 0 : body.position.y;
    res["position"]["z"] = isnan(body.position.z) ? 0 : body.position.z;

    res["velocity"] = nlohmann::json::object();
    res["velocity"]["x"] = isnan(body.velocity.x) ? 0 : body.velocity.x;
    res["velocity"]["y"] = isnan(body.velocity.y) ? 0 : body.velocity.y;
    res["velocity"]["z"] = isnan(body.velocity.z) ? 0 : body.velocity.z;

    res["confidence"] = isnan(body.confidence) ? 0 : body.confidence;

    res["bounding_box"] = nlohmann::json::array();
    for (auto &i : body.bounding_box)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x;
        e["y"] = isnan(i.y) ? 0 : i.y;
        e["z"] = isnan(i.z) ? 0 : i.z;
        res["bounding_box"].push_back(e);
    }

    res["dimensions"] = nlohmann::json::object();
    res["dimensions"]["x"] = isnan(body.dimensions.x) ? 0 : body.dimensions.x;
    res["dimensions"]["y"] = isnan(body.dimensions.y) ? 0 : body.dimensions.y;
    res["dimensions"]["z"] = isnan(body.dimensions.z) ? 0 : body.dimensions.z;

    res["keypoint"] = nlohmann::json::array();
    for (auto &i : body.keypoint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x;
        e["y"] = isnan(i.y) ? 0 : i.y;
        e["z"] = isnan(i.z) ? 0 : i.z;
        res["keypoint"].push_back(e);
    }

    res["keypoint_confidence"] = nlohmann::json::array();
    for (auto &i : body.keypoint_confidence)
    {
        res["keypoint_confidence"].push_back(isnan(i) ? 0 : i);
    }
    res["local_position_per_joint"] = nlohmann::json::array();
    for (auto &i : body.local_position_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x;
        e["y"] = isnan(i.y) ? 0 : i.y;
        e["z"] = isnan(i.z) ? 0 : i.z;
        res["local_position_per_joint"].push_back(e);
    }
    res["local_orientation_per_joint"] = nlohmann::json::array();
    for (auto &i : body.local_orientation_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x;
        e["y"] = isnan(i.y) ? 0 : i.y;
        e["z"] = isnan(i.z) ? 0 : i.z;
        e["w"] = isnan(i.w) ? 1 : i.w;
        res["local_orientation_per_joint"].push_back(e);
        if (isnan(i.x) || isnan(i.y) || isnan(i.z) || isnan(i.w))
        {
            std::cout << "Nan value in quat." << std::endl;
        }
    }
    res["global_root_orientation"] = nlohmann::json::object();
    res["global_root_orientation"]["x"] = isnan(body.global_root_orientation.x) ? 0 : body.global_root_orientation.x;
    res["global_root_orientation"]["y"] = isnan(body.global_root_orientation.y) ? 0 : body.global_root_orientation.y;
    res["global_root_orientation"]["z"] = isnan(body.global_root_orientation.z) ? 0 : body.global_root_orientation.z;
    res["global_root_orientation"]["w"] = isnan(body.global_root_orientation.w) ? 0 : body.global_root_orientation.w;
    return res;
}

// Create the json sent to the clients
nlohmann::json LiveLinker::getJson(sl::FusionMetrics metrics, sl::Bodies &bodies, sl::BODY_FORMAT body_format)
{
    nlohmann::json j;

    nlohmann::json bodyData;
    nlohmann::json fusionMetricsData;
    nlohmann::json singleMetricsData;

    fusionMetricsData["mean_camera_fused"] = metrics.mean_camera_fused;
    fusionMetricsData["mean_stdev_between_camera"] = metrics.mean_stdev_between_camera;

    for (auto &cam : cameras_)
    {

        singleMetricsData["sn"] = cam.sn;
        singleMetricsData["received_fps"] = metrics.camera_individual_stats[cam.sn].received_fps;
        singleMetricsData["received_latency"] = metrics.camera_individual_stats[cam.sn].received_latency;
        singleMetricsData["synced_latency"] = metrics.camera_individual_stats[cam.sn].synced_latency;
        singleMetricsData["is_present"] = metrics.camera_individual_stats[cam.sn].is_present;
        singleMetricsData["ratio_detection"] = metrics.camera_individual_stats[cam.sn].ratio_detection;
        singleMetricsData["delta_ts"] = metrics.camera_individual_stats[cam.sn].delta_ts;

        fusionMetricsData["camera_individual_stats"].push_back(singleMetricsData);
    }

    bodyData["body_format"] = body_format;

    bodyData["is_new"] = (int)bodies.is_new;
    bodyData["is_tracked"] = (int)bodies.is_tracked;
    bodyData["timestamp"] = bodies.timestamp.data_ns;

    bodyData["nb_object"] = bodies.body_list.size();
    bodyData["body_list"] = nlohmann::json::array();

    for (auto &body : bodies.body_list)
    {
        bodyData["body_list"].push_back(bodyDataToJsonMeter(body));
        if (body.local_orientation_per_joint.size() == 0)
        {
            std::cout << "Detected body[" << body.id << "] has empty jointsOrientation, with tracking state : " << body.tracking_state << std::endl;
        }
    }

    j["fusionMetrics"] = fusionMetricsData;
    j["bodies"] = bodyData;

    return j;
}

/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------- MISC & MONO-CAM ------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------

void LiveLinker::print(string msg_prefix, sl::ERROR_CODE err_code, string msg_suffix)
{
    cout << "[Sample]";
    if (err_code != sl::ERROR_CODE::SUCCESS)
        cout << "[Error]";
    cout << " " << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS)
    {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

// If the sender encounter NaN values, it sends 0 instead.
nlohmann::json LiveLinker::bodyDataToJson(sl::BodyData body)
{
    nlohmann::json res;

    res["id"] = body.id;
    // res["unique_object_id"] = body.unique_object_id.get();
    res["tracking_state"] = body.tracking_state;
    res["action_state"] = body.action_state;
    res["position"] = nlohmann::json::object();
    res["position"]["x"] = isnan(body.position.x) ? 0 : body.position.x / 1000;
    res["position"]["y"] = isnan(body.position.y) ? 0 : body.position.y / 1000;
    res["position"]["z"] = isnan(body.position.z) ? 0 : body.position.z / 1000;
    res["velocity"] = nlohmann::json::object();
    res["velocity"]["x"] = isnan(body.velocity.x) ? 0 : body.velocity.x / 1000;
    res["velocity"]["y"] = isnan(body.velocity.y) ? 0 : body.velocity.y / 1000;
    res["velocity"]["z"] = isnan(body.velocity.z) ? 0 : body.velocity.z / 1000;
    // res["position_covariance"] = nlohmann::json::array();
    // for (auto& i : body.position_covariance)
    //  {
    //     res["position_covariance"].push_back(i);
    // }
    // res["bounding_box_2d"] = nlohmann::json::array();
    // for (auto& i : body.bounding_box_2d)
    //  {
    //     nlohmann::json e;
    //     e["x"] = i.x;
    //     e["y"] = i.y;
    //     res["bounding_box_2d"].push_back(e);
    // }
    res["confidence"] = isnan(body.confidence) ? 0 : body.confidence;
    res["bounding_box"] = nlohmann::json::array();
    for (auto &i : body.bounding_box)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["bounding_box"].push_back(e);
    }
    res["dimensions"] = nlohmann::json::object();
    res["dimensions"]["x"] = isnan(body.dimensions.x) ? 0 : body.dimensions.x / 1000;
    res["dimensions"]["y"] = isnan(body.dimensions.y) ? 0 : body.dimensions.y / 1000;
    res["dimensions"]["z"] = isnan(body.dimensions.z) ? 0 : body.dimensions.z / 1000;
    // res["keypoint_2d"] = nlohmann::json::array();
    // for (auto& i : body.keypoint_2d)
    //  {
    //     nlohmann::json e;
    //     e["x"] = i.x;
    //     e["y"] = i.y;
    //     res["keypoint_2d"].push_back(e);
    // }
    res["keypoint"] = nlohmann::json::array();
    for (auto &i : body.keypoint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["keypoint"].push_back(e);
    }
    std::cout << "id [" << body.id << "] hips: " << body.keypoint[0] << std::endl;
    // res["head_bounding_box_2d"] = nlohmann::json::array();
    // for (auto& i : body.head_bounding_box_2d)
    //  {
    //     nlohmann::json e;
    //     e["x"] = i.x;
    //     e["y"] = i.y;
    //     res["head_bounding_box_2d"].push_back(e);
    // }
    // res["head_bounding_box"] = nlohmann::json::array();
    // for (auto& i : body.head_bounding_box)
    //   {
    //     nlohmann::json e;
    //     e["x"] = i.x;
    //     e["y"] = i.y;
    //     e["z"] = i.z;
    //     res["head_bounding_box"].push_back(e);
    // }
    // res["head_position"] = nlohmann::json::object();
    // res["head_position"]["x"] = body.head_position.x;
    // res["head_position"]["y"] = body.head_position.y;
    // res["head_position"]["z"] = body.head_position.z;
    res["keypoint_confidence"] = nlohmann::json::array();
    for (auto &i : body.keypoint_confidence)
    {
        res["keypoint_confidence"].push_back(isnan(i) ? 0 : i);
    }
    res["local_position_per_joint"] = nlohmann::json::array();
    for (auto &i : body.local_position_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["local_position_per_joint"].push_back(e);
    }
    res["local_orientation_per_joint"] = nlohmann::json::array();
    for (auto &i : body.local_orientation_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 42 : i.x;
        e["y"] = isnan(i.y) ? 42 : i.y;
        e["z"] = isnan(i.z) ? 42 : i.z;
        e["w"] = isnan(i.w) ? 42 : i.w;
        res["local_orientation_per_joint"].push_back(e);
    }
    res["global_root_orientation"] = nlohmann::json::object();
    res["global_root_orientation"]["x"] = isnan(body.global_root_orientation.x) ? 0 : body.global_root_orientation.x;
    res["global_root_orientation"]["y"] = isnan(body.global_root_orientation.y) ? 0 : body.global_root_orientation.y;
    res["global_root_orientation"]["z"] = isnan(body.global_root_orientation.z) ? 0 : body.global_root_orientation.z;
    res["global_root_orientation"]["w"] = isnan(body.global_root_orientation.w) ? 0 : body.global_root_orientation.w;
    return res;
}