#include "../inc/utf.h"
#include <fstream>
#include <utility>
#include "../exc/NotFoundException"
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
    try {
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
    } catch (...){
        throw std::runtime_error("Error occurred in set_data()");
    }

}

void utf::set_data(std::vector<uint8_t> data_) {
    this->data = std::move(data_);
    populate_vector_data();
}

void utf::populate_vector_data() {
    vector_data = convert_chars_to_vector(data);
}

std::vector<std::vector<uint8_t>> utf::convert_chars_to_vector(std::vector<uint8_t> data) {
    try{
        uint8_t buffer[4];
        std::vector<std::vector<uint8_t>> result;   //Stores all the results of the character separation

        for(size_t i = 0; i < data.size(); i++){
            std::vector<uint8_t> char_result;   //Stores the final vector with the character separated in bytes (intermediate step)

            //Initialize the buffer with the first 4 bytes based on i
            buffer[0] = (uint8_t) data[i];
            buffer[1] = (uint8_t) data[i + 1];
            buffer[2] = (uint8_t) data[i + 2];
            buffer[3] = (uint8_t) data[i + 3];

            if(buffer[0] == 13){    //Delete / ignore cursor reset
                continue;
            }

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
            } else {
                throw std::runtime_error("Invalid UTF-8 character");
            }
            result.push_back(char_result);
        } //The vector data will be populated with the character index in the original data as the index in the vector and the data translating to the character.
        return result;
    } catch (...) {
        throw std::runtime_error("Error occurred in convert_chars_to_vector()");
    }

}


//This function takes one char of multiple bytes and searches for it in the vector_data
std::vector<size_t> utf::search(const std::vector<uint8_t>& value) {
    try{
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
    } catch (...){
        throw std::runtime_error("Error occurred in search() (one character)");
    }

}

//This function takes a series of characters and searches for it in vector_data
std::vector<size_t> utf::search(const std::vector<std::vector<uint8_t>>& value) {
    try{
        std::vector<size_t> result;                     //Stores the indices of the value in vector_data
        std::vector<size_t> indices_first_character;     //Stores the indices of the first character in value

        //Finds all the indices in the text for each of the characters in value
        indices_first_character = search(value[0]);

        size_t value_size = value.size();
        //For max amount of matches
        for(unsigned long j : indices_first_character) {
            //For characters in value
            for(size_t k = 0; k < value_size; k++){
                //If character at the relative index k in vector_data corresponds to the character at index k of the to be searched for string
                if(vector_data[j + k] == value[k]) {
                    //If all checked then add to result
                    if(k == value_size - 1) result.push_back(j);
                    continue;
                } else break;
            }
        }

        //If all checked and no result, throw not found exception.
        if(result.empty()) throw NotFoundException();

        return result;
    } catch (const NotFoundException& e) {
        throw;
    } catch (...) {
        throw std::runtime_error("Error occurred in search() (string)");
    }
}

std::vector<size_t> utf::search(const std::string& value) {
    return search(convert_chars_to_vector(std::vector<uint8_t>(value.begin(), value.end())));
}

void utf::print() {
    print(vector_data);
}

void utf::print(const std::vector<size_t>& indices) {
    for(const auto& index : indices){
        std::cout << index << std::endl;
    }
}

void utf::print(const std::vector<std::vector<uint8_t>>& values) {
    for(const auto& value : values){
        std::string str(value.begin(), value.end());
        std::cout << str;
    }
}

void utf::replace(const std::vector<std::vector<uint8_t>>& value, std::vector<std::vector<uint8_t>> new_value) {
    try {
        //All indices where the to be replaces value is
        std::vector<size_t> indices = search(value);

        //For all those indices
        for(size_t i = 0; i < indices.size(); i++){
            auto index = indices[i];                                                       //Current index
            vector_data.erase(vector_data.begin() + index, vector_data.begin() + index + value.size()); //Remove value
            vector_data.insert(vector_data.begin() + index, new_value.begin(), new_value.end());        //Insert value
            indices = update_indices(indices, new_value.size() - value.size(), index);                  //Correct indices
        }
    } catch (...){
        throw std::exception();
    }
}

void utf::replace(const std::string &value, const std::string &new_value) {
    replace(convert_chars_to_vector(std::vector<uint8_t>(value.begin(), value.end())), convert_chars_to_vector(std::vector<uint8_t>(new_value.begin(), new_value.end())));
}

void utf::delete_value(const std::vector<std::vector<uint8_t>> &value) {
    try {
        std::vector<size_t> indices = search(value);

        for(int i = 0; i < indices.size(); i++){
            auto index = indices[i];
            vector_data.erase(vector_data.begin() + index, vector_data.begin() + index + value.size());
            indices = update_indices(indices, -value.size(), index);
        }
    } catch (...){
        throw std::exception();
    }
}

void utf::delete_value(const std::string &value) {
    delete_value(convert_chars_to_vector(std::vector<uint8_t>(value.begin(), value.end())));
}

void utf::insert_value(const std::vector<std::vector<uint8_t>> &value, size_t index) {
    vector_data.insert(vector_data.begin() + index, value.begin(), value.end());
}

void utf::insert_value(const std::string &value, size_t index) {
    insert_value(convert_chars_to_vector(std::vector<uint8_t>(value.begin(), value.end())), index);
}

std::vector<size_t> utf::update_indices(const std::vector<size_t>& indices, size_t change_length, size_t start_index) {
    std::vector<size_t> result;
    for (auto index : indices) {
        if (index >= start_index) {
            result.push_back(index + change_length);
        } else {
            result.push_back(index);
        }
    }
    return result;
}