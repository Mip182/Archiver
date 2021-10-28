#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <string>

class Writer {
public:
    const size_t NUMBER_OF_BITS_IN_BYTE = 8;

    Writer(const std::string& file_name);

    Writer(char* file_name);

    void WriteCharacter(char character);

    void PushOneNumber();

    void PushTillCan();

    void PushTillEnd();

    std::ofstream out_;
    std::deque<bool> bits_to_push_;
};
