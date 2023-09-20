#include "config_parser.h"

#include <iostream>

ConfigParser::ConfigParser(const std::string& config_file = "../etc/config.json") {

    // Read Json
    FILE* fp = fopen(config_file.c_str(), "rb");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    // Parse configuration
    const rapidjson::Value& files = doc["files"];
    people_num_ = files.Size();
    for (rapidjson::SizeType i = 0; i < files.Size(); i++) {
        const rapidjson::Value& file = files[i];
        logging_directory_.push_back(file["name"].GetString());
        ids_.push_back(file["id"].GetInt());
        start_frame_.push_back(file["start"].GetInt());
        position_.push_back(std::vector<int>());
        rotation_.push_back(std::vector<int>());
        skip_.push_back(std::vector<int>());
        rewind_.push_back(std::vector<int>());
        for (rapidjson::SizeType j = 0; j < 3; j++) {
            position_[i].push_back(file["position"][j].GetInt());
            rotation_[i].push_back(file["rotation"][j].GetInt());
        }
        int skip_size = file["skip"].Size();
        for (rapidjson::SizeType j = 0; j < skip_size; j++) {
            skip_[i].push_back(file["skip"][j].GetInt());
        }
        int rewind_size = file["rewind"].Size();
        for (rapidjson::SizeType j = 0; j < rewind_size; j++) {
            rewind_[i].push_back(file["rewind"][j].GetInt());
        }
    }

    // Print configuration
    std::cout << "people_num: " << people_num_ << std::endl;
    for (int i = 0; i < people_num_; i++) {
        std::cout << "logging_directory: " << logging_directory_[i] << std::endl;
        std::cout << "  * id: " << ids_[i] << std::endl;
        std::cout << "  * start_frame: " << start_frame_[i] << std::endl;
        std::cout << "  * position: ";
        for (int j = 0; j < 3; j++) {
            std::cout << position_[i][j] << " ";
        }
        std::cout << std::endl;
        std::cout << "  * rotation: ";
        for (int j = 0; j < 3; j++) {
            std::cout << rotation_[i][j] << " ";
        }
        std::cout << std::endl;
        std::cout << "  * skip: ";
        for (int j = 0; j < skip_[i].size(); j++) {
            std::cout << skip_[i][j] << " ";
        }
        std::cout << std::endl;
        std::cout << "  * rewind: ";
        for (int j = 0; j < rewind_[i].size(); j++) {
            std::cout << rewind_[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // parse ip addr and port
    ip_addr_ = doc["ip_addr"].GetString();
    port_ = doc["port"].GetInt();
    delay_ = doc["delay"].GetInt();
    initial_frame_ = doc["init"].GetInt();

}