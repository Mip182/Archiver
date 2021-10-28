#include "archiver.h"

const size_t Archiver::NUMBER_OF_BITS_IN_BYTE = 8;
const size_t Archiver::SYMBOLS_COUNT = (1 << 9);
const size_t Archiver::ALPHABET_SIZE = 9;
const size_t Archiver::FILENAME_END = 256;
const size_t Archiver::ONE_MORE_FILE = 257;
const size_t Archiver::ARCHIVE_END = 258;

void Archiver::Decompress(const char* file_name) const {
    Huffman huffman(file_name);
}

void Archiver::Compress(int argc, char* argv[]) const {
    if (argc <= 3) {
        throw std::runtime_error("error - too few arguments");
    }

    Writer writer(argv[2]);

    std::deque<bool>& bits_to_push_in_archive = writer.bits_to_push_;

    for (size_t index = 3; index < static_cast<size_t>(argc); ++index) {
        char* next_file_name = argv[index];

        Reader reader_to_count_frequencies(next_file_name);

        std::string next_file_name_str = static_cast<std::string>(next_file_name);
        std::vector<int> file_name = TransformStringToNumbers(next_file_name_str);

        CompressNextFile(next_file_name, writer, bits_to_push_in_archive,
                         reader_to_count_frequencies, next_file_name_str, file_name, index, argc);
    }

    writer.PushTillEnd();
}

void Archiver::CompressNextFile(char* next_file_name, Writer& writer,
                                std::deque<bool>& bits_to_push_in_archive,
                                Reader& reader_to_count_frequencies,
                                const std::string& next_file_name_str,
                                const std::vector<int>& file_name, size_t index, int argc) const {
    std::vector<size_t> frequencies_of_symbols =
        GetFrequenciesOfSymbols(reader_to_count_frequencies);

    for (auto symbol : file_name) {
        frequencies_of_symbols[symbol]++;
    }

    size_t number_of_symbols = GetNumberOfSymbols(frequencies_of_symbols);

    for (size_t counter = 0; counter < ALPHABET_SIZE; ++counter) {
        if ((number_of_symbols >> counter) & 1) {
            bits_to_push_in_archive.push_back(true);
        } else {
            bits_to_push_in_archive.push_back(false);
        }

        writer.PushTillCan();
    }

    Huffman huffman(frequencies_of_symbols);

    for (size_t next_index = 0; next_index < number_of_symbols; next_index++) {
        PushNumberToDequeBool(writer, bits_to_push_in_archive,
                              huffman.order_of_symbols[next_index]);
    }

    for (size_t current_index = 1; current_index < huffman.number_of_codes_with_size.size();
         current_index++) {
        PushNumberToDequeBool(writer, bits_to_push_in_archive,
                              huffman.number_of_codes_with_size[current_index]);
    }

    for (auto value : file_name) {
        AppearVectorBoolToDequeBool(writer, bits_to_push_in_archive, huffman.code_of_symbol[value]);
    }

    AppearVectorBoolToDequeBool(writer, bits_to_push_in_archive,
                                huffman.code_of_symbol[FILENAME_END]);

    Reader reader(next_file_name);

    while (!reader.bits_of_file_.empty()) {
        reader.ReadNextBits();

        int value = reader.GetValueOfNextBits(NUMBER_OF_BITS_IN_BYTE);

        AppearVectorBoolToDequeBool(writer, bits_to_push_in_archive, huffman.code_of_symbol[value]);

        reader.DeleteUselessBitsAtTheBeginning(NUMBER_OF_BITS_IN_BYTE);
    }

    AppearVectorBoolToDequeBool(
        writer, bits_to_push_in_archive,
        huffman.code_of_symbol[(index + 1 == static_cast<size_t>(argc) ? ARCHIVE_END
                                                                       : ONE_MORE_FILE)]);

    writer.PushTillCan();
}

std::vector<size_t> Archiver::GetFrequenciesOfSymbols(Reader& reader_to_count_frequencies) const {
    std::vector<size_t> frequencies_of_symbols(SYMBOLS_COUNT);

    frequencies_of_symbols[FILENAME_END] = 1;
    frequencies_of_symbols[ONE_MORE_FILE] = 1;
    frequencies_of_symbols[ARCHIVE_END] = 1;

    while (!reader_to_count_frequencies.bits_of_file_.empty()) {
        reader_to_count_frequencies.ReadNextBits();

        int symbol = reader_to_count_frequencies.GetValueOfNextBits(NUMBER_OF_BITS_IN_BYTE);

        frequencies_of_symbols[symbol]++;

        reader_to_count_frequencies.DeleteUselessBitsAtTheBeginning(NUMBER_OF_BITS_IN_BYTE);
    }
    return frequencies_of_symbols;
}

size_t Archiver::GetNumberOfSymbols(const std::vector<size_t>& frequencies_of_symbols) const {
    size_t number_of_symbols = 0;

    for (auto frequency : frequencies_of_symbols) {
        if (frequency) {
            number_of_symbols++;
        }
    }

    return number_of_symbols;
}

void Archiver::PushNumberToDequeBool(Writer& writer, std::deque<bool>& vec, int number) const {
    for (size_t index = 0; index < ALPHABET_SIZE; ++index) {
        vec.push_back((number >> index) & 1);
    }

    writer.PushTillCan();
}

int Archiver::GetIntFromChar(char character) const {
    int value = 0;

    for (size_t index = 0; index < NUMBER_OF_BITS_IN_BYTE; ++index) {
        if ((character >> index) & 1) {
            value |= (1 << index);
        }
    }

    return value;
}

std::vector<int> Archiver::TransformStringToNumbers(const std::string& str) const {
    std::vector<int> numbers;

    for (auto character : str) {
        numbers.push_back(static_cast<unsigned char>(character));
    }

    return numbers;
}

void Archiver::AppearVectorBoolToDequeBool(Writer& writer, std::deque<bool>& bits,
                                           const std::vector<bool>& to_push) const {
    for (auto bit : to_push) {
        bits.push_back(bit);
    }

    writer.PushTillCan();
}
