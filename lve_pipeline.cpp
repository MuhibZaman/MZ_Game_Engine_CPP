#include "lve_pipeline.hpp"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lve {
    LvePipeline::LvePipeline(const std::string& vertFilepath, const std::string& fragFilepath){
        createGraphicsPipeline(vertFilepath, fragFilepath);
    }

    //To read our shader files, we are using an input file stream object
    std::vector<char> LvePipeline::readFile(const std::string& filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary}; //ate means to seek to the end immediately, binary means to read file as binary

        if(!file.is_open()) {
            //Check path and permissions
            throw std::runtime_error("failed to open file: " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg()); //tellg returns position 
        //character buffer
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void LvePipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath){
        auto vertCode = readFile(vertFilepath);
        auto fragCode = readFile(fragFilepath);

        std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
        std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';
    }
} //namespace lve