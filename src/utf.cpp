//
// Created by basva on 26/11/2024.
//

#include "../inc/utf.h"
#include <fstream>
#include <utility>
#include "exception"
#include "unordered_set"
#include "../exc/NotFoundException"
#include <algorithm>

utf::utf() {

}

utf::utf(const std::string& file_path) {
    set_data(file_path);
}

utf::utf(std::vector<uint8_t> input_data) {
    set_data(std::move(input_data));
}

utf::~utf() {
    data.clear();
    vector_data.clear();
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
    populate_vector_data();
}

void utf::set_data(std::vector<uint8_t> data) {
    this->data = std::move(data);
    populate_vector_data();
}

void utf::populate_vector_data() {
    for(size_t i = 0; i < data.size(); i++){
        std::vector<uint8_t> result;
        if((data[i] & 0b10000000) == 0b10000000){           //For regular 1 byte (ASCII) characters
            result.push_back(data[i]);
        } else if ((data[i] & 0b11100000) == 0b11000000){   //For 2 byte characters
            result.push_back(data[i]);
            result.push_back(data[i + 1]);
            i++;
        } else if ((data[i] & 0b11110000) == 0b11100000){   //For 3 byte characters
            result.push_back(data[i]);
            result.push_back(data[i + 1]);
            result.push_back(data[i + 2]);
            i+=2;
        } else if ((data[i] & 0b11111000) == 0b11110000){   //For 4 byte characters
            result.push_back(data[i]);
            result.push_back(data[i + 1]);
            result.push_back(data[i + 2]);
            result.push_back(data[i + 3]);
            i+=3;
        }
        else {
            throw std::runtime_error("Invalid UTF-8 character");
        }
    } //The vector data will be populated with the character index in the original data as the index in the vector and the data translating to the character.
}

std::vector<uint8_t> utf::get_data() {
    return this->data;
}
namespace std {

    template <>
struct hash<std::vector<uint8_t>> {
    std::size_t operator()(const std::vector<uint8_t>& v) const {
    std::size_t seed = 0;
    for (auto& elem : v) {
        seed ^= std::hash<uint8_t>{}(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
    }
};
}

size_t utf::search(std::vector<uint8_t> value) {
    std::unordered_set<std::vector<uint8_t>> data_set;

    for(size_t i = 0; i < data.size(); ++i){
        data_set.insert(vector_data[i]);
    }

    auto it = data_set.find(value);
    if(it == data_set.end()) throw NotFoundException();

    return std::distance(vector_data.begin(), std::find(vector_data.begin(), vector_data.end(), *it));

}

size_t utf::search(uint8_t value) {
    std::vector<uint8_t> value_ = {value};
    return search(value_);
}

//std::vector<uint8_t> utf::replace(uint8_t value, uint8_t new_value) {
//    search(value);
//}