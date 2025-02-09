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
    explicit utf(std::vector<uint8_t> input_data);
    explicit utf(const std::string &file_path);

    ~utf();

    void set_data(const std::string& file_path);
    void set_data(std::vector<uint8_t> data);
    void populate_vector_data();
    static std::vector<std::vector<uint8_t>> convert_chars_to_vector(std::vector<uint8_t> data);

    std::vector<size_t> search(const std::string&);

    void replace(const std::string& value, const std::string& new_value);
    void delete_value(const std::string& value);
    void insert_value(const std::string& value, size_t index);

    void print(); //Prints all the data in vector_data
    static void print(const std::vector<size_t>& indices); //Prints indices inputted as parameter
    static void print(const std::vector<std::vector<uint8_t>>& values); //Prints values inputted as parameter


private:
    std::vector<uint8_t> data;
    std::vector<std::vector<uint8_t>> vector_data;

    std::vector<size_t> search(const std::vector<uint8_t>& value);
    std::vector<size_t> search(const std::vector<std::vector<uint8_t>> &value);

    void replace(std::vector<std::vector<uint8_t>> value, std::vector<std::vector<uint8_t>> new_value);
    void delete_value(const std::vector<std::vector<uint8_t>> &value);
    void insert_value(const std::vector<std::vector<uint8_t>> &value, size_t index);
    static std::vector<size_t> update_indices(const std::vector<size_t>& indices, size_t change_length, size_t start_index);
};


#endif //UTF_UTF_H