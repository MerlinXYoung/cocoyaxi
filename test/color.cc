#include "co/color.h"

#include <iostream>

#include "co/print.h"

int main(int argc, char** argv) {
    std::cout << co::color::red("hello\n");
    std::cout << co::color::green("hello\n");
    std::cout << co::color::blue("hello\n");
    std::cout << co::color::yellow("hello\n");
    std::cout << co::color::magenta("hello\n");
    std::cout << co::color::cyan("hello\n");
    std::cout << "hello\n";
    std::cout << co::color::bold("hello\n");
    std::cout << co::color::bold("hello\n").red();
    std::cout << co::color::bold("hello\n").green();
    std::cout << co::color::bold("hello\n").blue();
    std::cout << co::color::bold("hello\n").yellow();
    std::cout << co::color::bold("hello\n").magenta();
    std::cout << co::color::bold("hello\n").cyan();
    co::print("hello", co::color::red(" coost "), 23);
    co::print("hello", co::color::bold(" coost ").green(), 23);
    co::print("hello", co::color::blue(" coost "), 23);
    return 0;
}
