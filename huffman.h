#pragma once

#include <vector>
#include <deque>
#include <cassert>
#include <functional>
#include <algorithm>
#include <numeric>
#include <queue>
#include <map>
#include <string>
#include <exception>

#include "vertex.h"
#include "writer.h"
#include "reader.h"

class Huffman {
public:
    const static size_t NUMBER_OF_BITS_IN_BYTE;
    const static size_t SYMBOLS_COUNT;
    const static size_t ALPHABET_SIZE;
    const static int FILENAME_END;
    const static int ONE_MORE_FILE;
    const static int ARCHIVE_END;

    Huffman(const std::vector<size_t>& frequencies_of_alphabet);

    Huffman(const char* file_name);

    Vertex* RunHuffman(const std::vector<size_t>& frequencies_of_alphabet) const;

    size_t GetNumberOfSymbols(const std::vector<size_t>& frequencies_of_alphabet) const;

    std::vector<std::vector<bool>> GetNewCodeOfSymbols(Vertex* root) const;

    void NormalizeCodeOfSymbols(const std::vector<size_t>& frequencies_of_alphabet,
                                const std::vector<std::vector<bool>>& new_code_of_symbol,
                                size_t number_of_symbols);

    void GetOrderOfSymbols(Reader& reader, std::deque<bool>& bits_of_file,size_t number_of_symbols);

    void GetCodeOfSymbols(Reader& reader,std::deque<bool>& bits_of_file,size_t number_of_symbols);

    int FindValueOfNextCode(Reader& reader, const std::deque<bool>& bits);

    size_t FindLengthOfNextCode(Reader& reader, const std::deque<bool>& bits);

    int GetValueOfNextLengthBits(Reader& reader, std::deque<bool>& bits, size_t length) const;

    char TransformIntToChar(int value) const;

    void IncrementByOne(std::vector<bool>& vec) const;

    size_t max_symbol_code_size;

    std::vector<int> order_of_symbols;
    std::vector<int> number_of_codes_with_size;
    std::map<std::vector<bool>, int> what_symbol_have_this_code;
    std::vector<std::vector<bool>> code_of_symbol;
};
