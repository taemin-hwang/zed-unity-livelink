#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include <vector>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

class ConfigParser {
public:
    ConfigParser(const std::string& config_file);
    virtual ~ConfigParser() = default;

    inline int get_people_num() const { return people_num_; }
    inline std::vector<std::string> get_logging_directory() const { return logging_directory_; }
    inline std::vector<std::vector<int>> get_position() const { return position_; }
    inline std::vector<std::vector<int>> get_rotation() const { return rotation_; }
    inline std::vector<int> get_ids() const { return ids_; }
    inline std::vector<int> get_start_frame() const { return start_frame_; }

private:
    int people_num_;
    std::vector<std::string> logging_directory_;
    std::vector<std::vector<int>> position_;
    std::vector<std::vector<int>> rotation_;
    std::vector<int> ids_;
    std::vector<int> start_frame_;
};


#endif // _CONFIG_PARSER_H_