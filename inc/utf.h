//
// Created by basva on 26/11/2024.
//

#ifndef UTF_UTF_H
#define UTF_UTF_H
#include <vector>
#include <cstdio>
#include <cstdint>
#include <string_view>
#include <unordered_map>

class utf {
public:
    utf();

    utf(std::vector<uint8_t> input_data);

    utf(const std::string &file_path);

    ~utf();

    void set_data(const std::string& file_path);
    void set_data(std::vector<uint8_t> data);
    void populate_vector_data();

    std::vector<uint8_t> get_data();

    std::vector<size_t> search(uint8_t value);
    std::vector<size_t> search(const std::vector<uint8_t>& value);
    std::vector<size_t> search(const std::vector<std::vector<uint8_t>> &value);

    void replace(const std::vector<uint8_t>& value, const std::vector<uint8_t>& new_value);

    void delete_value(uint8_t value);
    void delete_value(const std::vector<uint8_t>& value);

    void print(); //Prints all the data in vector_data
    void print(const std::vector<size_t>& indices); //Prints indices inputted as parameter
    static void print(const std::vector<std::vector<uint8_t>>& values); //Prints values inputted as parameter

private:
    std::vector<uint8_t> data;
    std::vector<std::vector<uint8_t>> vector_data;


};


#endif //UTF_UTF_H