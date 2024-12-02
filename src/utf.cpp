//
// Created by basva on 26/11/2024.
//

#include "../inc/utf.h"
#include <fstream>
#include "exception"

utf::utf() {

}

utf::utf(const std::string& file_path) {
    set_data(file_path);
}

utf::utf(std::vector<uint8_t> input_data) {
    data = std::move(input_data);
}

utf::~utf() {
    data.clear();
}

void utf::set_data(const std::string& file_path) {
    //Open stream and set cursor to the end of the file
    std::ifstream src(file_path, std::ios::binary | std::ios::ate);

    //Check if file is open
    if(!src.is_open()) {
        throw std::runtime_error("File not found");
    }

    //Get size of the file by seeing where the cursor is
    std::streamsize size = src.tellg();
    std::vector<uint8_t> result(size);
    //Reset the cursor to the beginning of the file
    src.seekg(0, std::ios::beg);

    //Read the file and populate result
    if(!src.read(reinterpret_cast<char*>(result.data()), size)) {
        throw std::runtime_error("Could not read file"); //If we can't read the file, throw an error
    }

    //Move populated result to data
    data = std::move(result);
}

void utf::set_data(std::vector<uint8_t> data) {
    this->data = std::move(data);
}