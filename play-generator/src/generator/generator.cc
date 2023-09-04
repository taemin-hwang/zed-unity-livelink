#include "generator/generator.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

void Generator::generate() {
    std::cout << "Generator::generate()" << std::endl;

    int frame_num = 0;
    int people_num = config_parser_->get_people_num();

    // get template document
    std::cout << "get template document" << std::endl;

    while(frame_num < 200) {
        rapidjson::Document template_document;
        auto& template_allocator = template_document.GetAllocator();
        template_document.CopyFrom(get_template_document("../etc/template.json"), template_allocator);

        // print template document
        std::cout << "template_document: " << std::endl;
        std::cout << "  * bodies: " << std::endl;
        std::cout << "    * body_format: " << template_document["bodies"]["body_format"].GetInt() << std::endl;
        std::cout << "    * body_list (size) : " << template_document["bodies"]["body_list"].Size() << std::endl;
        std::cout << "  * nb_object: " << template_document["bodies"]["nb_object"].GetInt() << std::endl;

        // get logging document
        std::cout << "get logging document" << std::endl;

        // set nb_object
        template_document["bodies"]["nb_object"] = people_num;
        std::cout << "people_num: " << people_num << std::endl;

        for (int i = 0; i < people_num; i++) {
            auto current_frame = frame_num + config_parser_->get_start_frame()[i];

            std::string logging_file = config_parser_->get_logging_directory()[i] + "/" + add_zero_padding(current_frame, 8) + ".json";

            auto logging_document = get_logging_document(logging_file);
            auto id = config_parser_->get_ids()[i];

            std::cout << "read logging document: " << logging_file << std::endl;
            std::cout << "  * bodies: " << std::endl;
            std::cout << "    * body_format: " << logging_document["bodies"]["body_format"].GetInt() << std::endl;
            std::cout << "    * body_list (size) : " << logging_document["bodies"]["body_list"].Size() << std::endl;
            for (int i = 0; i < logging_document["bodies"]["body_list"].Size(); i++) {
                std::cout << "      *[" << i << "] id: " << logging_document["bodies"]["body_list"][i]["id"].GetInt() << std::endl;
                std::cout << "      *[" << i << "] confidence: " << logging_document["bodies"]["body_list"][i]["confidence"].GetDouble() << std::endl;
            }

            int body_list_size = logging_document["bodies"]["body_list"].Size();

            if (body_list_size  == 1) {
                rapidjson::Value temporary_value(logging_document["bodies"]["body_list"][0], template_allocator);

                // change id
                temporary_value["id"] = id;
                // push bodies to template
                template_document["bodies"]["body_list"].PushBack(temporary_value, template_allocator);
            }
        }

        // print template document
        std::cout << "template_document: " << std::endl;
        std::cout << "  * bodies: " << std::endl;
        std::cout << "    * body_format: " << template_document["bodies"]["body_format"].GetInt() << std::endl;
        std::cout << "    * body_list (size) : " << template_document["bodies"]["body_list"].Size() << std::endl;
        for (int i = 0; i < template_document["bodies"]["body_list"].Size(); i++) {
            std::cout << "      *[" << i << "] id: " << template_document["bodies"]["body_list"][i]["id"].GetInt() << std::endl;
            std::cout << "      *[" << i << "] confidence: " << template_document["bodies"]["body_list"][i]["confidence"].GetDouble() << std::endl;
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
        outputFile << buffer.GetString();
        outputFile.close();

        frame_num++;
    }
}

// get template document
rapidjson::Document Generator::get_template_document(std::string template_file = "./etc/template.json") {
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