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
#include <iostream>

utf::utf() = default;

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
    uint8_t buffer[4];
    std::vector<std::vector<uint8_t>> result;   //Stores all the results of the character separation

    for(size_t i = 0; i < data.size(); i++){
        std::vector<uint8_t> char_result;   //Stores the final vector with the character seperated in bytes (intermediate step)

        //Initialize the buffer with the first 4 bytes based on i
        buffer[0] = (uint8_t) data[i];
        buffer[1] = (uint8_t) data[i + 1];
        buffer[2] = (uint8_t) data[i + 2];
        buffer[3] = (uint8_t) data[i + 3];

        if((buffer[0] & 0b10000000) == 0b00000000){           //For regular 1 byte (ASCII) characters
            char_result.push_back(buffer[0]);
        } else if ((buffer[0] & 0b11100000) == 0b11000000){   //For 2 byte characters
            char_result.push_back(buffer[0]);
            char_result.push_back(buffer[1]);
            i++;
        } else if ((buffer[0] & 0b11110000) == 0b11100000){   //For 3 byte characters
            char_result.push_back(buffer[0]);
            char_result.push_back(buffer[1]);
            char_result.push_back(buffer[2]);
            i+=2;
        } else if ((buffer[0] & 0b11111000) == 0b11110000){   //For 4 byte characters
            char_result.push_back(buffer[0]);
            char_result.push_back(buffer[1]);
            char_result.push_back(buffer[2]);
            char_result.push_back(buffer[3]);
            i+=3;
        }
        else {
            throw std::runtime_error("Invalid UTF-8 character");
        }
        result.push_back(char_result);
    } //The vector data will be populated with the character index in the original data as the index in the vector and the data translating to the character.
    vector_data = std::move(result);
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

//This function takes one char of one byte and searches for it in the vector_data
std::vector<size_t> utf::search(uint8_t value) {
    return search(std::vector<uint8_t>{value});
}

//This function takes one char of multiple bytes and searches for it in the vector_data
std::vector<size_t> utf::search(const std::vector<uint8_t>& value) {
    std::vector<size_t> indices;  //Stores the indices of the characters in value

    for(size_t i = 0; i < vector_data.size(); i++){
        if(vector_data[i] == value){
            indices.push_back(i);
        }
    }

    if(indices.empty()){
        throw NotFoundException();
    }
    return indices;
}

std::vector<size_t> utf::search(const std::vector<std::vector<uint8_t>>& value) {
    std::vector<size_t> result; //Stores the indices of the value in vector_data
    std::vector<std::vector<size_t>> indices;  //Stores the indices of the characters in value

    for(const auto& current_datapoint : value){
        indices.push_back(search(current_datapoint));
    }

    //Cycle over all indexes of the characters at index i in value
    for(size_t j = 0; j < indices[0].size() - 1; j++){
        //Cycle over all characters at index i
        for (int i = 0; i < indices.size() - 1; i++){
            std::vector<uint8_t> current_char = vector_data[indices[i][j]]; //Get the character at index i
            if(std::find(indices[i + 1].begin(), indices[i + 1].end(), indices[i][j] + 1) != indices[i + 1].end()){ //If the next character is in the next vector
                result.push_back(indices[i][j]); //Add the index to the result
            } //Find the next character in the next vector
        }
    }
    return result;
}

//void utf::replace(const std::vector<uint8_t>& value, const std::vector<uint8_t>& new_value) {
//    try {
//        size_t index = search(value);
//        vector_data[index] = new_value;
//    } catch (...){
//        throw std::exception();
//    }
//}

//void utf::delete_value(uint8_t value) {
//    std::vector<uint8_t> value_ {value};
//    delete_value(value_);
//}

//void utf::delete_value(const std::vector<uint8_t>& value) {
//    vector_data.erase(vector_data.begin() + search(value));
//}

void utf::print() {
    print(vector_data);
}

void utf::print(const std::vector<size_t>& indices) {
    for(auto index : indices){
        std::cout << index << std::endl;
    }
}

void utf::print(const std::vector<std::vector<uint8_t>>& values) {
    for(auto value : values){
        std::string str(value.begin(), value.end());
        std::cout << str;
    }
}