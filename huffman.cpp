#include "huffman.h"

const size_t Huffman::NUMBER_OF_BITS_IN_BYTE = 8;
const size_t Huffman::SYMBOLS_COUNT = (1 << 9);
const size_t Huffman::ALPHABET_SIZE = 9;
const int Huffman::FILENAME_END = 256;
const int Huffman::ONE_MORE_FILE = 257;
const int Huffman::ARCHIVE_END = 258;

Huffman::Huffman(const std::vector<size_t>& frequencies_of_alphabet) {
    code_of_symbol = std::vector<std::vector<bool>>(SYMBOLS_COUNT);

    Vertex* root = RunHuffman(frequencies_of_alphabet);

    std::vector<std::vector<bool>> new_code_of_symbol = GetNewCodeOfSymbols(root);

    size_t number_of_symbols = GetNumberOfSymbols(frequencies_of_alphabet);

    order_of_symbols = std::vector<int>(SYMBOLS_COUNT);

    std::iota(order_of_symbols.begin(), order_of_symbols.end(), 0);

    std::sort(order_of_symbols.begin(), order_of_symbols.end(), [&](int a, int b) {
        if (frequencies_of_alphabet[b] == 0) {
            return frequencies_of_alphabet[a] != 0;
        }

        if (frequencies_of_alphabet[a] == 0) {
            return false;
        }

        if (new_code_of_symbol[a].size() != new_code_of_symbol[b].size()) {
            return new_code_of_symbol[a].size() < new_code_of_symbol[b].size();
        }

        return new_code_of_symbol[a] < new_code_of_symbol[b];
    });

    NormalizeCodeOfSymbols(frequencies_of_alphabet, new_code_of_symbol, number_of_symbols);
}

Huffman::Huffman(const char* file_name) {
    Reader reader(file_name);

    std::deque<bool>& bits_of_file = reader.bits_of_file_;

    while (NUMBER_OF_BITS_IN_BYTE < bits_of_file.size()) {
        size_t number_of_symbols = GetValueOfNextLengthBits(reader, bits_of_file, ALPHABET_SIZE);
        reader.DeleteUselessBitsAtTheBeginning(ALPHABET_SIZE);

        GetOrderOfSymbols(reader, bits_of_file, number_of_symbols);

        GetCodeOfSymbols(reader, bits_of_file, number_of_symbols);

        std::string next_file_name;

        while (true) {
            size_t next_length = FindLengthOfNextCode(reader, bits_of_file);
            int next_value = FindValueOfNextCode(reader, bits_of_file);

            if (next_value == FILENAME_END) {
                break;
            }

            next_file_name += TransformIntToChar(next_value);

            reader.DeleteUselessBitsAtTheBeginning(next_length);
        }

        {
            size_t next_length = FindLengthOfNextCode(reader, bits_of_file);
            int next_value = FindValueOfNextCode(reader, bits_of_file);

            if (next_value != FILENAME_END) {
                throw std::runtime_error("error - wrong data in archive file");
            }

            reader.DeleteUselessBitsAtTheBeginning(next_length);
        }

        Writer writer(next_file_name);

        while (true) {
            size_t next_length = FindLengthOfNextCode(reader, bits_of_file);
            int next_value = FindValueOfNextCode(reader, bits_of_file);

            if (next_value == ARCHIVE_END || next_value == ONE_MORE_FILE) {
                break;
            }

            writer.WriteCharacter(TransformIntToChar(next_value));

            reader.DeleteUselessBitsAtTheBeginning(next_length);
        }

        size_t next_length = FindLengthOfNextCode(reader, bits_of_file);
        int next_value = FindValueOfNextCode(reader, bits_of_file);

        if (next_value != ARCHIVE_END && next_value != ONE_MORE_FILE) {
            throw std::runtime_error("error - wrong data in archive file");
        }

        if (next_length + NUMBER_OF_BITS_IN_BYTE >= bits_of_file.size()) {
            if (next_value != ARCHIVE_END) {
                throw std::runtime_error("error - wrong data in archive file");
            }
        } else {
            if (next_value != ONE_MORE_FILE) {
                throw std::runtime_error("error - wrong data in archive file");
            }
        }

        reader.DeleteUselessBitsAtTheBeginning(next_length);
    }

    if (NUMBER_OF_BITS_IN_BYTE < bits_of_file.size()) {
        throw std::runtime_error("error - wrong data in archive file");
    }
}

Vertex* Huffman::RunHuffman(const std::vector<size_t>& frequencies_of_alphabet) const {
    auto comparator_for_vertexes = [](const Vertex* a, const Vertex* b) {
        return a->frequency_of_vertex > b->frequency_of_vertex;
    };

    std::priority_queue<Vertex*, std::vector<Vertex*>, decltype(comparator_for_vertexes)> queue(
        comparator_for_vertexes);

    for (size_t index = 0; index < SYMBOLS_COUNT; ++index) {
        if (frequencies_of_alphabet[index] == 0) {
            continue;
        }

        Vertex* ha = new Vertex();
        ha->frequency_of_vertex = frequencies_of_alphabet[index];
        ha->symbol_of_vertex = index;

        queue.push(ha);
    }

    while (queue.size() > 1) {
        Vertex* left_child = queue.top();
        queue.pop();

        Vertex* right_child = queue.top();
        queue.pop();

        Vertex* parent = new Vertex();
        parent->frequency_of_vertex =
            left_child->frequency_of_vertex + right_child->frequency_of_vertex;
        parent->left_child = left_child;
        parent->right_child = right_child;

        queue.push(parent);
    }

    assert(queue.size() == 1);

    return queue.top();
}

size_t Huffman::GetNumberOfSymbols(const std::vector<size_t>& frequencies_of_alphabet) const {
    size_t number_of_symbols = 0;
    for (auto frequency : frequencies_of_alphabet) {
        if (frequency > 0) {
            number_of_symbols++;
        }
    }

    return number_of_symbols;
}

std::vector<std::vector<bool>> Huffman::GetNewCodeOfSymbols(Vertex* root) const {
    std::vector<std::vector<bool>> new_code_of_symbol(SYMBOLS_COUNT);

    std::function<void(Vertex*, std::vector<bool>&)> dfs = [&](Vertex* current,
                                                               std::vector<bool>& current_code) {
        if (current->symbol_of_vertex != -1) {
            new_code_of_symbol[current->symbol_of_vertex] = current_code;
            return;
        }

        if (current->left_child != nullptr) {
            current_code.push_back(false);
            dfs(current->left_child, current_code);
            current_code.pop_back();
        }

        if (current->right_child != nullptr) {
            current_code.push_back(false);
            dfs(current->right_child, current_code);
            current_code.pop_back();
        }
    };

    std::vector<bool> current_code;
    dfs(root, current_code);

    return new_code_of_symbol;
}

void Huffman::NormalizeCodeOfSymbols(const std::vector<size_t>& frequencies_of_alphabet,
                                     const std::vector<std::vector<bool>>& new_code_of_symbol,
                                     size_t number_of_symbols) {
    std::vector<bool> now_code(new_code_of_symbol[order_of_symbols[0]].size());

    code_of_symbol[order_of_symbols[0]] = now_code;

    for (size_t index = 1; index < number_of_symbols; ++index) {
        IncrementByOne(now_code);

        while (now_code.size() < new_code_of_symbol[order_of_symbols[index]].size()) {
            now_code.push_back(false);
        }

        code_of_symbol[order_of_symbols[index]] = now_code;
    }

    number_of_codes_with_size = std::vector<int>(now_code.size() + 1);

    for (size_t symbol = 0; symbol < SYMBOLS_COUNT; ++symbol) {
        if (frequencies_of_alphabet[symbol] == 0) {
            continue;
        }

        number_of_codes_with_size[new_code_of_symbol[symbol].size()]++;
    }
}

void Huffman::GetOrderOfSymbols(Reader& reader, std::deque<bool>& bits_of_file,
                                size_t number_of_symbols) {
    order_of_symbols = std::vector<int>(number_of_symbols);

    for (size_t index = 0; index < number_of_symbols; ++index) {
        order_of_symbols[index] = GetValueOfNextLengthBits(reader, bits_of_file, ALPHABET_SIZE);
        reader.DeleteUselessBitsAtTheBeginning(ALPHABET_SIZE);
    }
}

void Huffman::GetCodeOfSymbols(Reader& reader, std::deque<bool>& bits_of_file,
                               size_t number_of_symbols) {
    size_t total_number_of_symbols = 0;
    number_of_codes_with_size.clear();
    number_of_codes_with_size.push_back(0);

    while (!bits_of_file.empty() && total_number_of_symbols < number_of_symbols) {
        number_of_codes_with_size.push_back(
            GetValueOfNextLengthBits(reader, bits_of_file, ALPHABET_SIZE));
        reader.DeleteUselessBitsAtTheBeginning(ALPHABET_SIZE);

        total_number_of_symbols += number_of_codes_with_size.back();
    }

    if (total_number_of_symbols < number_of_symbols) {
        throw std::runtime_error("error - too few arguments in archive file");
    }

    if (total_number_of_symbols > number_of_symbols) {
        throw std::runtime_error("error - too much arguments in archive file");
    }

    size_t current_size_of_file = 1;

    while (number_of_codes_with_size[current_size_of_file] == 0) {
        current_size_of_file++;
    }

    what_symbol_have_this_code.clear();

    std::vector<bool> now_code(current_size_of_file);

    code_of_symbol = std::vector<std::vector<bool>>(SYMBOLS_COUNT);

    size_t current_symbol = 0;

    max_symbol_code_size = number_of_codes_with_size.size() - 1;

    if (order_of_symbols.empty()) {
        throw std::runtime_error("error - wrong data in archive file");
    }

    code_of_symbol[order_of_symbols[current_symbol++]] = now_code;
    number_of_codes_with_size[current_size_of_file]--;

    while (current_symbol < number_of_symbols) {
        IncrementByOne(now_code);

        while (now_code.size() <= max_symbol_code_size &&
               number_of_codes_with_size[current_size_of_file] == 0) {
            now_code.push_back(false);
            current_size_of_file++;
        }

        if (now_code.size() > max_symbol_code_size) {
            throw std::runtime_error("error - wrong data in archive file");
        }

        code_of_symbol[order_of_symbols[current_symbol++]] = now_code;
        number_of_codes_with_size[current_size_of_file]--;
    }

    for (size_t index = 0; index < number_of_symbols; ++index) {
        what_symbol_have_this_code[code_of_symbol[order_of_symbols[index]]] =
            order_of_symbols[index];
    }
}

int Huffman::FindValueOfNextCode(Reader& reader, const std::deque<bool>& bits) {
    reader.ReadNextBits();

    std::vector<bool> current_code;

    for (size_t length = 1; length <= bits.size() && length <= max_symbol_code_size; length++) {
        current_code.push_back(bits[length - 1]);

        if (what_symbol_have_this_code.find(current_code) != what_symbol_have_this_code.end()) {
            return what_symbol_have_this_code[current_code];
        }
    }

    throw std::runtime_error("error - wrong data in archive file");
}

size_t Huffman::FindLengthOfNextCode(Reader& reader, const std::deque<bool>& bits) {
    reader.ReadNextBits();

    std::vector<bool> current_code;

    for (size_t length = 1; length <= bits.size() && length <= max_symbol_code_size; length++) {
        current_code.push_back(bits[length - 1]);

        if (what_symbol_have_this_code.find(current_code) != what_symbol_have_this_code.end()) {
            return length;
        }
    }

    throw std::runtime_error("error - wrong data in archive file");
}

int Huffman::GetValueOfNextLengthBits(Reader& reader, std::deque<bool>& bits, size_t length) const {
    reader.ReadNextBits();

    if (length <= bits.size()) {
        int value = 0;
        int power_of_two = 1;

        for (size_t delta_index = 0; delta_index < length; ++delta_index) {
            if (bits[delta_index]) {
                value += power_of_two;
            }
            power_of_two *= 2;
        }

        return value;
    }

    throw std::runtime_error("error - wrong data in archive file");
}

char Huffman::TransformIntToChar(int value) const {
    if (value == ONE_MORE_FILE || value == ARCHIVE_END || value == FILENAME_END) {
        throw std::runtime_error("error - wrong data in archive file");
    }

    return static_cast<char>(value);
}

void Huffman::IncrementByOne(std::vector<bool>& vec) const {
    for (size_t index = 0; index < vec.size(); index++) {
        if (vec[vec.size() - 1 - index]) {
            vec[vec.size() - 1 - index] = false;
        } else {
            vec[vec.size() - 1 - index] = true;
            return;
        }
    }

    throw std::runtime_error("error - wrong data in archive file");
}
