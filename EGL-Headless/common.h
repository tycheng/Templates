#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Config {
    std::string inputFile;
    std::vector<glm::vec2> rotations;
    std::vector<std::string> outputFiles;
};

void parse_config(std::string file, std::vector<Config>& configs) {
    std::ifstream in(file);
    assert(in.good());

    std::string str;
    float longitude, latitude;

    while (!in.eof()) {
        char c;
        in >> c;
        switch (c) {
            case 'M':
                in >> str;
                configs.push_back({
                    .inputFile = str
                });
                // std::cout << "inputFile: " << str << std::endl;
                break;
            case 'C':
                in >> longitude >> latitude;
                longitude = glm::radians(longitude);
                latitude = glm::radians(latitude);
                configs.back().rotations.push_back(glm::vec2(longitude, latitude));
                // std::cout << "rotation: " << longitude << ", " << latitude << std::endl;
                break;
            case 'F':
                in >> str;
                if (str.length() > 0) {
                    configs.back().outputFiles.push_back(str);
                    // std::cout << "outputFile: " << str << std::endl;
                    str = "";
                }
                break;
        }
    }

    for (auto config : configs) {
        assert(config.rotations.size() == config.outputFiles.size());
    }
}


#endif /* end of include guard: COMMON_H */
