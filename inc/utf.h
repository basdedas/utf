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

    size_t search(uint8_t value);
    size_t search(std::vector<uint8_t> value);

    std::vector<uint8_t> replace(uint8_t value, uint8_t new_value);
    std::vector<uint8_t> replace(std::vector<uint8_t> value, std::vector<uint8_t> new_value);

    void delete_value(uint8_t value);
    void delete_value(std::vector<uint8_t> value);

    void print();

private:
    std::vector<uint8_t> data;
    std::vector<std::vector<uint8_t>> vector_data;
    //We use an unordered_map because it has an average lookup and insertion speed of O(1) as opposed to (ordered) map which has O(log n)
};


#endif //UTF_UTF_H