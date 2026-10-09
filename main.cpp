#include <iostream>
#include <string>
#include <vector>

#include "inc/utf.h"
#include "exc/NotFoundException"

// CMake sets this to the absolute path of the samples/ folder, so the demo works
// from any working directory (including CLion's default one).
#ifndef UTF_SAMPLES_DIR
#define UTF_SAMPLES_DIR "samples"
#endif

// Shows every operation of the library on one file:
// search -> replace -> delete -> insert. Indices count characters, not bytes.
static void demo(const std::string& path, const std::string& word, const std::string& replacement) {
    std::cout << "=== " << path << " ===\n";

    utf text(path);
    std::cout << "Original:\n";
    text.print();

    // Search: where does the word occur? (character indices)
    std::cout << "\nSearch \"" << word << "\" found at character index:\n";
    utf::print(text.search(word));

    // Replace: every occurrence, even when the new text has a different length.
    text.replace(word, replacement);
    std::cout << "\nAfter replacing \"" << word << "\" with \"" << replacement << "\":\n";
    text.print();

    // Delete: remove every occurrence of the replacement again.
    text.delete_value(replacement);
    std::cout << "\nAfter deleting \"" << replacement << "\":\n";
    text.print();

    // Insert: put a marker at the very start.
    text.insert_value(">> ", 0);
    std::cout << "\nAfter inserting \">> \" at index 0:\n";
    text.print();
    std::cout << "\n";
}

// Usage:
//   ./utf                              runs the demo on the three sample files
//   ./utf <file> <search> <replacement>  runs the demo on your own file
int main(int argc, char* argv[]) {
    try {
        if (argc == 4) {
            demo(argv[1], argv[2], argv[3]);
        } else if (argc == 1) {
            const std::string dir = UTF_SAMPLES_DIR;
            demo(dir + "/english.txt", "character", "symbol");
            demo(dir + "/chinese.txt", "字符", "符号");
            demo(dir + "/korean.txt", "문자", "글자");
        } else {
            std::cerr << "Usage: " << argv[0] << " [<file> <search> <replacement>]\n";
            return 2;
        }
    } catch (const NotFoundException& e) {
        std::cerr << "Not found: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}