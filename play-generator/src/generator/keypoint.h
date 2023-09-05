#ifndef KEYPOINT_H_
#define KEYPOINT_H_

#include <queue>
#include <string>
#include <iostream>

struct Keypoint {
    std::vector<double> position;
    std::vector<double> rotation;

    Keypoint() { position.resize(3); rotation.resize(4); }
    void set_position(std::vector<double> position) { this->position = position; }
    void set_rotation(std::vector<double> rotation) { this->rotation = rotation; }
};

struct Body {
    std::vector<Keypoint> keypoint;

    Body() { keypoint.resize(38); }
    void set_keypoint(std::vector<Keypoint> keypoint) { this->keypoint = keypoint; }
};

struct BodyList {
    std::deque<Body> body;

    BodyList() { }
    void operator=(BodyList body_list) {
        this->body = body_list.body;
    }

    void push(Body body) {
        auto body_list_size = this->body.size();
        if (body_list_size >= BUFFER_SIZE) {
            this->body.pop_front();
        }
        this->body.push_back(body);

        if (this->body.size() > BUFFER_SIZE) {
            std::cout << "BodyList size (" << this->body.size() << ") is exceed to " << BUFFER_SIZE << std::endl;
        }

    }

    Body get_average() {
        Body ret;

        auto body_list_size = this->body.size();
        if (body_list_size == 0) {
            std::cout << "body_list_size == 0" << std::endl;
            return ret;
        }

        for (int i = 0; i < body_list_size; i++) {
            for (int j = 0; j < this->body[i].keypoint.size(); j++) {
                for (int k = 0; k < this->body[i].keypoint[j].position.size(); k++) {
                    ret.keypoint[j].position[k] += this->body[i].keypoint[j].position[k];
                }
                for (int k = 0; k < this->body[i].keypoint[j].rotation.size(); k++) {
                    ret.keypoint[j].rotation[k] += this->body[i].keypoint[j].rotation[k];
                }
            }
        }
        for (int i = 0; i < ret.keypoint.size(); i++) {
            for (int j = 0; j < ret.keypoint[i].position.size(); j++) {
                ret.keypoint[i].position[j] /= body_list_size;
            }
            for (int j = 0; j < ret.keypoint[i].rotation.size(); j++) {
                ret.keypoint[i].rotation[j] /= body_list_size;
            }
        }
        return ret;
    }

private:
    const int BUFFER_SIZE = 10;
};


#endif