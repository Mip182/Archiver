#include "reader.h"

Reader::Reader(const char* file_name) : in_() {
    in_ = std::fstream(file_name, std::ios_base::in | std::ios_base::binary);

    if (!in_.is_open()) {
        throw std::runtime_error("error - cannot open file named " +
                                 static_cast<std::string>(file_name));
    }

    ReadNextBits();
}

void Reader::ReadNextBits() {
    char character;

    while (bits_of_file_.size() < NUMBER_OF_BITS_TO_SEE && in_ >> std::noskipws >> character) {
        for (size_t index = 0; index < NUMBER_OF_BITS_IN_BYTE; ++index) {
            bits_of_file_.push_back((character >> index) & 1);
        }
    }
}

void Reader::DeleteUselessBitsAtTheBeginning(size_t count) {

    for (size_t index = 0; index < count; ++index) {
        bits_of_file_.pop_front();
    }

    ReadNextBits();
}

int Reader::GetValueOfNextBits(size_t size) {
    ReadNextBits();

    int value = 0;

    for (size_t index = 0; index < size; ++index) {
        if (bits_of_file_[index]) {
            value |= (1 << index);
        }
    }

    return value;
}
