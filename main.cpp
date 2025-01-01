#include <iostream>
#include "inc/utf.h"

int main() {
    utf utf1("../test.txt");
    std::vector<std::vector<uint8_t>> data = {{(uint8_t) 'n'}, {(uint8_t) 'u'}};
    utf1.print(utf1.search(data));
    utf1.print();
    return 0;
}
