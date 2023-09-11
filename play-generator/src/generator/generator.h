#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "config/config_parser.h"
#include "generator/keypoint.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

#include <memory>
class Generator {
public:
    Generator(std::shared_ptr<ConfigParser> config_parser);
    virtual ~Generator() = default;

    void generate();
    std::string generate(int frame_num);

private:
    std::string add_zero_padding(int number, int width);
    rapidjson::Value get_body_from_logging_document(rapidjson::Document& logging_document, int idx, rapidjson::MemoryPoolAllocator<>& allocator);
    rapidjson::Value get_body_from_buffer(int idx);
    rapidjson::Document get_template_document(std::string template_file);
    rapidjson::Document get_logging_document(std::string logging_file);

    std::shared_ptr<ConfigParser> config_parser_;
    std::vector<BodyList> people_;
    std::vector<std::string> bodies_;

    const double POSISTION_SCALE = 1.4;
};


#endif // _GENERATOR_H_