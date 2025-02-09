#include <iostream>
#include "inc/utf.h"

int main() {
    //This main is for the user, some example instructions are given.

    utf utf1("../test.txt");

    std::string data = "不会";
    utf::print(utf1.search(data));
    utf1.print();
    utf1.replace("不会", "aaa");
    utf1.print();

    utf utf2("../test2.txt");
    utf::print(utf2.search("Never"));
    utf2.replace("Never", "Always");
    utf2.delete_value("Always");
    utf2.insert_value("Always", 0);
    utf2.print();

    return 0;
}
