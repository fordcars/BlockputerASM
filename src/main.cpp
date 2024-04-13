#include "getopt.h"
#include <iostream>

#include "Assembler.hpp"

void printHelp(const std::string& progName) {
    std::cout
        << "Usage:\n"
        << "    " << progName << " OPTIONS\n"
        << "Options:\n"
        << "    -i        input ASM file\n"
        << "    -o        output 'binary' file\n"
        << "    -c        add comments to output file"
        << std::endl;
}

int main(int argc, char** argv) {
    int c;
    std::string inputPath;
    std::string outputPath;
    bool addComments = false;

    while((c = getopt(argc, argv, "hi:o:c")) != -1) {
        switch(c) {
            case 'i':
            {
                inputPath = optarg;
                break;
            }
            case 'o':
            {
                outputPath = optarg;
                break;
            }
            case 'c':
                addComments = true;
                break;
            case '?':
            case 'h':
            default:
                printHelp(argv[0]);
                return 0;

            case -1:
                break;
        }
    }

    if(!inputPath.empty() && !outputPath.empty()) {
        Assembler assembler(inputPath);
        assembler.assembleTo(outputPath, addComments);
    }
    
    return 0;
}
