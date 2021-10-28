#include "writer.h"

Writer::Writer(const std::string& file_name) : out_() {
    this->out_ =
        std::ofstream(file_name, std::ios_base::trunc | std::ios_base::binary | std::ios_base::out);
}

Writer::Writer(char* file_name) : out_() {
    this->out_ =
        std::ofstream(file_name, std::ios_base::trunc | std::ios_base::binary | std::ios_base::out);
}

void Writer::WriteCharacter(char character) {
    this->out_ << character;
}

void Writer::PushOneNumber() {
    size_t size_of_number = std::min(NUMBER_OF_BITS_IN_BYTE, bits_to_push_.size());

    char character = 0;

    for (size_t index = 0; index < size_of_number; ++index) {
        if (bits_to_push_.front()) {
            character |= (1 << index);
        }

        bits_to_push_.pop_front();
    }

    out_ << character;
}

void Writer::PushTillCan() {
    while (bits_to_push_.size() >= NUMBER_OF_BITS_IN_BYTE) {
        PushOneNumber();
    }
}

void Writer::PushTillEnd() {
    while (!bits_to_push_.empty()) {
        PushOneNumber();
    }
}
