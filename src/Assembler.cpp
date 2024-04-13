#include "Assembler.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#define COMMENT_CHAR ';'
#define LABEL_CHAR ':'

// This shouldn't exist, sorryyyy I'm too lazy
#define GOTOX_INSTRUCTIONS 6
#define GOTO_INSTRUCTIONS 5
#define PUSH_INSTRUCTIONS 6
#define POP_INSTRUCTIONS 6
#define CALL_INSTRUCTIONS 12
#define PC_INSTRUCTIONS 2

namespace {
    std::string binaryInstToString(uint8_t binaryInst) {
        std::string out;
        for(int i = 7; i >= 0; --i) {
            out += (binaryInst & (1 << i) ? '1' : '0');
            if(i == 4) out += ' '; // For readability
        }

        return out;
    }

    OpCode mnemonicToOpCode(const std::string& mnemonic) {
        if(mnemonic == "MVRA") {
            return OpCode::MVRA;
        } else if(mnemonic == "MVAR") {
            return OpCode::MVAR;
        } else if(mnemonic == "LDA") {
            return OpCode::LDA;
        } else if(mnemonic == "STA") {
            return OpCode::STA;
        } else if(mnemonic == "MVAH") {
            return OpCode::MVAH;
        } else if(mnemonic == "MVAL") {
            return OpCode::MVAL;
        } else if(mnemonic == "JUMPZ") {
            return OpCode::JUMPZ;
        } else if(mnemonic == "JUMPN") {
            return OpCode::JUMPN;
        } else if(mnemonic == "ADD") {
            return OpCode::ADD;
        } else if(mnemonic == "SUB") {
            return OpCode::SUB;
        } else if(mnemonic == "MUL") {
            return OpCode::MUL;
        } else if(mnemonic == "LLS") {
            return OpCode::LLS;
        } else if(mnemonic == "LRS") {
            return OpCode::LRS;
        } else if(mnemonic == "AND") {
            return OpCode::AND;
        } else if(mnemonic == "OR") {
            return OpCode::OR;
        } else if(mnemonic == "XOR") {
            return OpCode::XOR;
        } else if(mnemonic == "NOT") {
            return OpCode::NOT;
        } else if(mnemonic == "GOTOZ") {
            return OpCode::GOTOZ;
        } else if(mnemonic == "GOTON") {
            return OpCode::GOTON;
        } else if(mnemonic == "GOTO") {
            return OpCode::GOTO;
        } else if(mnemonic == "PUSH") {
            return OpCode::PUSH;
        } else if(mnemonic == "POP") {
            return OpCode::POP;
        } else if(mnemonic == "CALL") {
            return OpCode::CALL;
        }

        std::cerr << "Error: unrecognized mnemonic '" << mnemonic << "'."
            << std::endl;
        return OpCode::MVRA;
    }

    RegCode regStringToRegCode(const std::string& regString) {
        if(regString == "R0") {
            return RegCode::R0;
        } else if(regString == "R1") {
            return RegCode::R1;
        } else if(regString == "R2") {
            return RegCode::R2;
        } else if(regString == "R3") {
            return RegCode::R3;
        } else if(regString == "R4") {
            return RegCode::R4;
        } else if(regString == "R5") {
            return RegCode::R5;
        } else if(regString == "R6") {
            return RegCode::R6;
        } else if(regString == "R7") {
            return RegCode::R7;
        } else if(regString == "R8") {
            return RegCode::R8;
        } else if(regString == "OUT") {
            return RegCode::OUT;
        } else if(regString == "IN") {
            return RegCode::IN;
        } else if(regString == "MEMA") {
            return RegCode::MEMA;
        } else if(regString == "INSTA") {
            return RegCode::INSTA;
        }  else if(regString == "ZERO") {
            return RegCode::ZERO;
        }  else if(regString == "ONE") {
            return RegCode::ONE;
        }  else if(regString == "MIN1") {
            return RegCode::MIN1;
        }

        std::cerr << "Error: unrecognized register '" << regString << "'."
            << std::endl;
        return RegCode::ZERO;
    }

    uint8_t combineOpCodeOperand(OpCode opCode, uint8_t operand) {
        return (static_cast<uint8_t>(opCode) << 4) | (0x0F & operand);
    }

    uint8_t combineOpCodeOperand(OpCode opCode, RegCode operand) {
        return (static_cast<uint8_t>(opCode) << 4) | (0x0F & static_cast<uint8_t>(operand));
    }
}

Assembler::Assembler(const std::string& inputPath) {
    std::ifstream file(inputPath);

    if (file.is_open()) {
        std::string line;
        int lineNumber = 0;
        int labelTargetNumber = 0;
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;

            std::vector<std::string> inst;
            while (iss >> word) {
                if(word[0] == COMMENT_CHAR) break; // Comment
                if(word[word.size() - 1] == ':') {
                    // Add label, and stop processing line
                    mLabels.emplace(std::pair(
                        word.substr(0, word.size() - 1),
                        labelTargetNumber
                    ));
                    if(!inst.empty()) {
                        std::cerr << "Error line " << lineNumber << ": found instructions before label."
                            << std::endl;
                    }
                    break;
                }
                inst.emplace_back(word);
            }

            if(!inst.empty()) {
                mInstructions.emplace_back(inst);

                // Bad!
                if(inst[0] == "GOTOZ" || inst[0] == "GOTON")
                    labelTargetNumber += GOTOX_INSTRUCTIONS;
                else if(inst[0] == "GOTO")
                    labelTargetNumber += GOTO_INSTRUCTIONS;
                else if(inst[0] == "PUSH")
                    labelTargetNumber += PUSH_INSTRUCTIONS;
                else if(inst[0] == "POP")
                    labelTargetNumber += POP_INSTRUCTIONS;
                else if(inst[0] == "CALL")
                    labelTargetNumber += CALL_INSTRUCTIONS;
                else if(inst[0] == "MVRA" && inst[1] == "PC")
                    labelTargetNumber += PC_INSTRUCTIONS;
                else
                    ++labelTargetNumber;
            }
            ++lineNumber;
        }
    } else {
        std::cerr << "Error: unable to open file: " << inputPath << std::endl;
    }
}

void Assembler::assembleTo(const std::string& outputPath, bool addComments) {
    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: unable to open file for writing: " << outputPath << std::endl;
    }   

    int indexOffset = 0;
    for(int i = 0; i < mInstructions.size(); ++i) {
        std::vector<std::string>& inst = mInstructions[i];
        std::vector<uint8_t> binaryInstructions
            = assembleInstruction(static_cast<uint8_t>(i + indexOffset), inst);
        indexOffset += binaryInstructions.size() - 1;

        for(int i = 0; i < binaryInstructions.size(); ++i) {
            uint8_t binaryInst = binaryInstructions[i];
            outputFile << binaryInstToString(binaryInst);

            if(addComments && i == 0) {
                // Optionally add original instruction as comment of first instruction
                std::string stringInst;
                for(const auto& token : inst) stringInst += token + ' ';
                outputFile << "  ; " << stringInst;
            }
            outputFile << '\n';
        }
    }
}

// May return multiple binary instructions
std::vector<uint8_t> Assembler::assembleInstruction(uint8_t instIndex, 
                                                    const std::vector<std::string>& inst) {
    OpCode op = mnemonicToOpCode(inst[0]);
    bool tooFewOperands = false;

    if(inst.size() > 2) {
        std::cout << "Warning: too many operands for '" << inst[0] << "'."
            << std::endl;
        return {};
    }

    switch(op) {
        case OpCode::MVRA:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            if(inst[1] == "PC") {
                // Helper for getting current PC (useful for function calling, etc)
                // Don't forget to update PC_INSTRUCTIONS
                uint8_t currPC = instIndex + 2; // After both emitted instructions
                return {
                    combineOpCodeOperand(OpCode::MVAH, currPC >> 4),
                    combineOpCodeOperand(OpCode::MVAL, currPC),
                };
            } else {
                return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
            }
        case OpCode::MVAR:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::LDA:
            return { combineOpCodeOperand(op, 0) };
        case OpCode::STA:
            return { combineOpCodeOperand(op, 0) };
        case OpCode::MVAH:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, std::stoi(inst[1])) };
        case OpCode::MVAL:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, std::stoi(inst[1])) };
        case OpCode::JUMPZ:
        case OpCode::JUMPN:
            return { combineOpCodeOperand(op, 0) };
        case OpCode::ADD:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::SUB:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::MUL:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::LLS:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::LRS:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::AND:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::OR:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::XOR:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            return { combineOpCodeOperand(op, regStringToRegCode(inst[1])) };
        case OpCode::NOT:
            return { combineOpCodeOperand(OpCode::XOR, RegCode::MIN1) };
        case OpCode::GOTOZ:
        case OpCode::GOTON:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            if(mLabels.find(inst[1]) == mLabels.end()) {
                std::cerr << "Error: cannot find label '" << inst[1] << "'."
                    << std::endl;
                return {};
            }

            // Don't forget to update GOTOX_INSTRUCTIONS
            return {
                // Temp move ACC to R8
                combineOpCodeOperand(OpCode::MVAR, RegCode::R8),

                // Set ACC to label instruction number
                combineOpCodeOperand(OpCode::MVAH, mLabels.at(inst[1]) >> 4),
                combineOpCodeOperand(OpCode::MVAL, mLabels.at(inst[1])),

                // Move ACC to INSTA
                combineOpCodeOperand(OpCode::MVAR, RegCode::INSTA),

                // Reset ACC
                combineOpCodeOperand(OpCode::MVRA, RegCode::R8),

                // JUMP
                combineOpCodeOperand(op == OpCode::GOTOZ ? OpCode::JUMPZ : OpCode::JUMPN, 0),
            };
        case OpCode::GOTO:
            if(inst.size() < 2) { tooFewOperands = true; break; }
            if(mLabels.find(inst[1]) == mLabels.end()) {
                std::cerr << "Error: cannot find label '" << inst[1] << "'."
                    << std::endl;
                return {};
            }

            // Don't forget to update GOTO_INSTRUCTIONS
            return {
                // Set ACC to label instruction number
                combineOpCodeOperand(OpCode::MVAH, mLabels.at(inst[1]) >> 4),
                combineOpCodeOperand(OpCode::MVAL, mLabels.at(inst[1])),

                // Move ACC to INSTA
                combineOpCodeOperand(OpCode::MVAR, RegCode::INSTA),

                // Clear ACC
                combineOpCodeOperand(OpCode::MVRA, RegCode::ZERO),

                // Jump
                combineOpCodeOperand(OpCode::JUMPZ, 0),
            };
        case OpCode::PUSH:
            if(inst.size() < 2) { tooFewOperands = true; break; }

            // Don't forget to update PUSH_INSTRUCTIONS
            return {
                // Set ACC to R7 (SP) and allocate one byte
                combineOpCodeOperand(OpCode::MVRA, RegCode::R7),
                combineOpCodeOperand(OpCode::SUB, RegCode::ONE),
                combineOpCodeOperand(OpCode::MVAR, RegCode::R7),

                // Set MEMA to new SP
                combineOpCodeOperand(OpCode::MVAR, RegCode::MEMA),

                // Set ACC to register to push
                combineOpCodeOperand(OpCode::MVRA, regStringToRegCode(inst[1])),

                // Store register in M[SP]
                combineOpCodeOperand(OpCode::STA, 0),
            };
        case OpCode::POP:
            if(inst.size() < 2) { tooFewOperands = true; break; }

            // Don't forget to update POP_INSTRUCTIONS
            return {
                // Set MEMA to R7 (SP)
                combineOpCodeOperand(OpCode::MVRA, RegCode::R7),
                combineOpCodeOperand(OpCode::MVAR, RegCode::MEMA),

                // Deallocate one byte from stack
                combineOpCodeOperand(OpCode::ADD, RegCode::ONE),
                combineOpCodeOperand(OpCode::MVAR, RegCode::R7),

                // Set ACC = M[MEMA]
                combineOpCodeOperand(OpCode::LDA, 0),

                // Set specified register to ACC
                combineOpCodeOperand(OpCode::MVAR, regStringToRegCode(inst[1]))
            };
        case OpCode::CALL: {
            if(inst.size() < 2) { tooFewOperands = true; break; }
            if(mLabels.find(inst[1]) == mLabels.end()) {
                std::cerr << "Error: cannot find label '" << inst[1] << "'."
                    << std::endl;
                return {};
            }

            // Return addr is immediately after emitted instructions
            uint8_t returnAddr = instIndex + CALL_INSTRUCTIONS;

            // Don't forget to update CALL_INSTRUCTIONS
            return {
                // Set ACC to R7 (SP) and allocate one byte
                combineOpCodeOperand(OpCode::MVRA, RegCode::R7),
                combineOpCodeOperand(OpCode::SUB, RegCode::ONE),
                combineOpCodeOperand(OpCode::MVAR, RegCode::R7),

                // Set MEMA to new SP
                combineOpCodeOperand(OpCode::MVAR, RegCode::MEMA),

                // Set ACC to return addr
                combineOpCodeOperand(OpCode::MVAH, returnAddr >> 4),
                combineOpCodeOperand(OpCode::MVAL, returnAddr),

                // Store return addr in M[SP]
                combineOpCodeOperand(OpCode::STA, 0),

                // Jump to label
                // Set ACC to label instruction number
                combineOpCodeOperand(OpCode::MVAH, mLabels.at(inst[1]) >> 4),
                combineOpCodeOperand(OpCode::MVAL, mLabels.at(inst[1])),

                // Move ACC to INSTA
                combineOpCodeOperand(OpCode::MVAR, RegCode::INSTA),

                // Clear ACC
                combineOpCodeOperand(OpCode::MVRA, RegCode::ZERO),

                // JUMP
                combineOpCodeOperand(OpCode::JUMPZ, 0)
            };
        }
    }

    if(tooFewOperands) {
        std::cout << "Warning: too few operands for '" << inst[0] << "'."
            << std::endl;
    }
    return {};
}