#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <string>
#include <exception>

class Reader {
public:
    const size_t NUMBER_OF_BITS_TO_SEE = 50;
    const size_t NUMBER_OF_BITS_IN_BYTE = 8;

    Reader(const char* file_name);

    void ReadNextBits();

    void DeleteUselessBitsAtTheBeginning(size_t count);

    int GetValueOfNextBits(size_t size);

    std::fstream in_;
    std::deque<bool> bits_of_file_;
};
