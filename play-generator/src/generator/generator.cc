#include "generator/generator.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

Generator::Generator(std::shared_ptr<ConfigParser> config_parser) : config_parser_(config_parser){\
    int people_num = config_parser_->get_people_num();
    bodies_.resize(people_num);
    for (int i = 0; i < people_num; i++) {
        bodies_[i] = "";
    }
    people_.resize(people_num);
    for (int i = 0; i < people_num; i++) {
        people_[i] = BodyList();
    }
}

std::string Generator::generate(int frame_num) {
    int people_num = config_parser_->get_people_num();
    rapidjson::Document template_document;
    auto& template_allocator = template_document.GetAllocator();
    template_document.CopyFrom(get_template_document("../etc/template.json"), template_allocator);

    // get logging document
    std::cout << "get logging document ";

    for (int i = 0; i <= frame_num % 20; i++ ) {
        printf(".");
    }
    printf("\n");

    // set nb_object
    template_document["bodies"]["nb_object"] = people_num;
    // std::cout << "people_num: " << people_num << std::endl;

    for (int i = 0; i < people_num; i++) {
        auto current_frame = frame_num + config_parser_->get_start_frame()[i];

        std::string logging_file = config_parser_->get_logging_directory()[i] + "/" + add_zero_padding(current_frame, 8) + ".json";
        auto logging_document = get_logging_document(logging_file);

        int body_list_size = logging_document["bodies"]["body_list"].Size();
        if (body_list_size == 1) {
            auto temporary_value = get_body_from_logging_document(logging_document, i, template_allocator);

            // convert to string
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            temporary_value.Accept(writer);
            bodies_[i] = buffer.GetString();

            // push bodies to template
            template_document["bodies"]["body_list"].PushBack(temporary_value, template_allocator);

        } else if (bodies_[i].size() > 0) {
            auto temporary_value = get_body_from_buffer(i);

            // push bodies to template
            template_document["bodies"]["body_list"].PushBack(temporary_value, template_allocator);
        }
    }

    // write template document
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    template_document.Accept(writer);

    // Write the JSON data to a file
    std::ofstream outputFile("../output/" + add_zero_padding(frame_num, 8) + ".json");
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the output file." << std::endl;
    }

    std::string ret = buffer.GetString();

    outputFile << ret;
    outputFile.close();

    return ret;
}

void Generator::generate() {
    std::cout << "Generator::generate()" << std::endl;

    int frame_num = 0;

    // get template document
    std::cout << "get template document" << std::endl;

    while(frame_num < 1000) {
        this->generate(frame_num);
        frame_num++;
    }
}

rapidjson::Value Generator::get_body_from_logging_document(rapidjson::Document& logging_document, int idx, rapidjson::MemoryPoolAllocator<>& allocator) {
    auto id = config_parser_->get_ids()[idx] - 1;
    auto position = config_parser_->get_position()[idx];

    rapidjson::Value temporary_value(logging_document["bodies"]["body_list"][0], allocator);

    // change id
    temporary_value["id"] = id;

    // add keypoint offset
    int keypoint_size = temporary_value["keypoint"].Size(); // 38 keypoint

    for (int i = 0 ; i < keypoint_size; i++) {
        temporary_value["keypoint"][i]["x"].SetDouble(temporary_value["keypoint"][i]["x"].GetDouble() + position[0]*1.2);
        temporary_value["keypoint"][i]["y"].SetDouble(temporary_value["keypoint"][i]["y"].GetDouble() + position[1]*1.2);
        temporary_value["keypoint"][i]["z"].SetDouble(temporary_value["keypoint"][i]["z"].GetDouble() + position[2]*1.2);
    }

    // update people
    Body body;
    for (int i = 0; i < keypoint_size; i++) {
        body.keypoint[i].position[0] = temporary_value["keypoint"][i]["x"].GetDouble();
        body.keypoint[i].position[1] = temporary_value["keypoint"][i]["y"].GetDouble();
        body.keypoint[i].position[2] = temporary_value["keypoint"][i]["z"].GetDouble();
    }
    people_[id].push(body); // id starts from 1

    // get average
    auto average_body = people_[id].get_average(); // id starts from 1

    // update temporary_value
    for (int i = 0; i < keypoint_size; i++) {
        temporary_value["keypoint"][i]["x"].SetDouble(average_body.keypoint[i].position[0]);
        temporary_value["keypoint"][i]["y"].SetDouble(average_body.keypoint[i].position[1]);
        temporary_value["keypoint"][i]["z"].SetDouble(average_body.keypoint[i].position[2]);
    }

    if(!temporary_value.IsObject()) {
        std::cout << "Parsing error for reading document" << std::endl;
    }

    return temporary_value;
}

rapidjson::Value Generator::get_body_from_buffer(int idx) {
    auto id = config_parser_->get_ids()[idx] - 1;
    rapidjson::Value temporary_value(rapidjson::kObjectType);

    // convert string to json
    rapidjson::Document body_document;
    body_document.Parse(bodies_[id].c_str());

    if(body_document.HasParseError()) {
        std::string debug;
        std::cout << "Parsing error for reading buffer" << std::endl;
        std::cout << bodies_[id] << std::endl;
        std::cin >> debug;

        return temporary_value;
    }

    // convert document to value
    temporary_value.CopyFrom(body_document, body_document.GetAllocator());

    // get average
    auto average_body = people_[id].get_average(); // id starts from 1

    // update temporary_value
    int keypoint_size = temporary_value["keypoint"].Size(); // 38 keypoint
    for (int i = 0; i < keypoint_size; i++) {
        temporary_value["keypoint"][i]["x"].SetDouble(average_body.keypoint[i].position[0]);
        temporary_value["keypoint"][i]["y"].SetDouble(average_body.keypoint[i].position[1]);
        temporary_value["keypoint"][i]["z"].SetDouble(average_body.keypoint[i].position[2]);
    }

    return temporary_value;
}

// get template document
rapidjson::Document Generator::get_template_document(std::string template_file = "../etc/template.json") {
    rapidjson::Document doc;
    FILE* fp = fopen(template_file.c_str(), "rb");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    doc.ParseStream(is);
    fclose(fp);
    return doc;
}

// get logging document
rapidjson::Document Generator::get_logging_document(std::string logging_file) {
    rapidjson::Document doc;
    FILE* fp = fopen(logging_file.c_str(), "rb");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    doc.ParseStream(is);
    fclose(fp);
    return doc;
}

std::string Generator::add_zero_padding(int number, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << number;
    return oss.str();
}