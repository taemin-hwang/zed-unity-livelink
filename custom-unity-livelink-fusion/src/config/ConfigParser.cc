#include <cstdio>
#include <iostream>

#include "config/ConfigParser.h"

using namespace rapidjson;

ConfigParser::ConfigParser(std::string path) {
    FILE* fp = fopen(path.c_str(), "rb");
    if(fp == 0) {
        std::cout << "file not exist : default param will be used" << std::endl;
        return;
    }
    char readbuffer[65536];
    FileReadStream is(fp, readbuffer, sizeof(readbuffer));

    Document document;
    document.ParseStream(is);

    if(document.HasMember("addr") && document["addr"].IsString()) {
        addr_ = document["addr"].GetString();
    }
    if(document.HasMember("port") && document["port"].IsInt()) {
        port_ = document["port"].GetInt();
    }
    if(document.HasMember("viewer") && document["viewer"].IsString()) {
        is_enable_viewer_ = document["viewer"].GetString();
    }
    if(document.HasMember("resolution") && document["resolution"].IsString()) {
        resolution_ = document["resolution"].GetString();
    }
    if(document.HasMember("fps") && document["fps"].IsInt()) {
        fps_ = document["fps"].GetInt();
    }
    if(document.HasMember("accuracy") && document["accuracy"].IsInt()) {
        accuracy_ = document["accuracy"].GetInt();
    }

    fclose(fp);
}