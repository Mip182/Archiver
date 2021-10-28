#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <exception>

#include "huffman.h"

class Archiver {
public:
    const static size_t NUMBER_OF_BITS_IN_BYTE;
    const static size_t SYMBOLS_COUNT;
    const static size_t ALPHABET_SIZE;
    const static size_t FILENAME_END;
    const static size_t ONE_MORE_FILE;
    const static size_t ARCHIVE_END;

    void Decompress(const char* file_name) const;

    void Compress(int argc, char* argv[]) const;

    void CompressNextFile(char* next_file_name, Writer& writer,
                          std::deque<bool>& bits_to_push_in_archive,
                          Reader& reader_to_count_frequencies,
                          const std::string& next_file_name_str, const std::vector<int>& file_name,
                          size_t index, int argc) const;

    std::vector<size_t> GetFrequenciesOfSymbols(Reader& reader_to_count_frequencies) const;

    size_t GetNumberOfSymbols(const std::vector<size_t>& frequencies_of_symbols) const;

    void PushNumberToDequeBool(Writer& writer, std::deque<bool>& vec, int number) const;

    int GetIntFromChar(char character) const;

    std::vector<int> TransformStringToNumbers(const std::string& str) const;

    void AppearVectorBoolToDequeBool(Writer& writer, std::deque<bool>& bits,
                                     const std::vector<bool>& to_push) const;
};
