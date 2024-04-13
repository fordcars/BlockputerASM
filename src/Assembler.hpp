#pragma once

#include <string>
#include <vector>
#include <unordered_map>

enum class OpCode : uint8_t {
    MVRA  = 0b0000,
    MVAR  = 0b0001,
    LDA   = 0b0011,
    STA   = 0b0010,
    MVAH  = 0b0110,
    MVAL  = 0b0111,
    JUMPZ = 0b0101,
    JUMPN = 0b0100,
    ADD   = 0b1100,
    SUB   = 0b1101,
    MUL   = 0b1111,
    LLS   = 0b1110,
    LRS   = 0b1010,
    AND   = 0b1011,
    OR    = 0b1001,
    XOR   = 0b1000,

    // Custom OpCodes
    NOT = 100, // ACC = NOT ACC
    GOTOZ,     // Goto label if ACC is zero. Modifies R5!
    GOTON,     // Goto label if ACC is negative. Modifies R5!
    GOTO       // Goto label. Erases ACC!
};

enum class RegCode : uint8_t {
    R0    = 0b0000,
    R1    = 0b0001,
    R2    = 0b0010,
    R3    = 0b0011,
    R4    = 0b0100,
    R5    = 0b0101,
    MEMA  = 0b0110,
    INSTA = 0b0111,
    ZERO  = 0b1000,
    ONE   = 0b1001,
    MIN1  = 0b1010,
    IR    = 0b1011,
    PC    = 0b1100
};


class Assembler {
public:
    Assembler(const std::string& inputPath);
    void assembleTo(const std::string& outputPath, bool addComments);

private:
    std::vector<std::vector<std::string>> mInstructions;
    std::unordered_map<std::string, int> mLabels;

    std::vector<uint8_t> assembleInstruction(const std::vector<std::string>& inst);
};
