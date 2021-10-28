#include "archiver.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Use \"-h\" to get help\n";
        return 0;
    }

    if (argv[1] == std::string("-h")) {
        std::cout << "Use \"-c archive_name file1 [file2 ...]\" to archive files file1, file2, ... "
                     "and save result to "
                     "file named archive_name\n";

        std::cout
            << "Use \"-d archive_name\" to dearchive files from archive_name to current directory";
        return 0;
    }

    Archiver archiver;

    if (argv[1] == std::string("-c")) {
        archiver.Compress(argc, argv);
        return 0;
    }

    if (argv[1] == std::string("-d")) {
        archiver.Decompress(argv[2]);
        return 0;
    }

    std::cout << "Unknown flags\n";
}
