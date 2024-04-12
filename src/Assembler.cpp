#include "Assembler.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#define COMMENT_CHAR ';'
#define LABEL_CHAR ':'

Assembler::Assembler(const std::string& inputPath) {
    std::ifstream file(inputPath);

    if (file.is_open()) {
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;

            std::vector<std::string> inst;
            while (iss >> word) {
                if(word[0] == COMMENT_CHAR) break; // Comment
                if(word[word.size() - 1] == ':') {
                    // Add label, and stop processing line
                    mLabels.emplace(std::pair(
                        word.substr(0, word.size() - 2),
                        static_cast<int>(mInstructions.size())
                    ));
                    if(!inst.empty()) {
                        std::cerr << "** Error line " << lineNumber << ", found instructions before label."
                            << std::endl;
                    }
                    break;
                }
                inst.emplace_back(word);
            }

            if(!inst.empty()) mInstructions.emplace_back(inst);
            ++lineNumber;
        }
    } else {
        std::cerr << "Unable to open file: " << inputPath << std::endl;
    }
}

void Assembler::assembleTo(const std::string& outputPath) {
    for(const auto& inst : mInstructions) {
        for(int i = 0; i < inst.size(); ++i) {
            const std::string& token = inst[i];
            std::cout << token << " ";
        }
    }
}