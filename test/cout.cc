#include "co/cout.h"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << text::red("hello\n");
    std::cout << text::green("hello\n");
    std::cout << text::blue("hello\n");
    std::cout << text::yellow("hello\n");
    std::cout << text::magenta("hello\n");
    std::cout << text::cyan("hello\n");
    std::cout << "hello\n";
    std::cout << text::bold("hello\n");
    std::cout << text::bold("hello\n").red();
    std::cout << text::bold("hello\n").green();
    std::cout << text::bold("hello\n").blue();
    std::cout << text::bold("hello\n").yellow();
    std::cout << text::bold("hello\n").magenta();
    std::cout << text::bold("hello\n").cyan();
    co::print("hello", text::red(" coost "), 23);
    co::print("hello", text::bold(" coost ").green(), 23);
    co::print("hello", text::blue(" coost "), 23);
    return 0;
}
