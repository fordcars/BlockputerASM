#pragma once

#include <string>
#include <vector>
#include <unordered_map>

enum class OpCode : uint8_t {
    MVRA  = 0b0000,
    MVAR  = 0b0001,
    STA   = 0b0010,
    LDA   = 0b0011,
    JUMPN = 0b0100,
    JUMPZ = 0b0101,
    MVAH  = 0b0110,
    MVAL  = 0b0111,
    MUL   = 0b1000,
    SUB   = 0b1001,
    ADD   = 0b1010,
    LLS   = 0b1011,
    LRS   = 0b1100,
    AND   = 0b1101,
    OR    = 0b1110,
    XOR   = 0b1111,
    
    // Custom OpCodes
    NOT = 100, // ACC = ~ACC

    // All of these modify ACC:
    GOTOZ,     // Goto label if ACC is zero. Modifies R8!
    GOTON,     // Goto label if ACC is negative. Modifies R8!
    GOTO,      // Goto label.
    PUSH,      // Pushes register to stack. Uses R7 as SP!
    POP,       // Pops register from stack. Uses R7 as SP!
    CALL,      // Pushes return addr to stack and gotos label. Uses R7 as SP!
};

enum class RegCode : uint8_t {
    R0    = 0b0000,
    R1    = 0b0001,
    R2    = 0b0010,
    R3    = 0b0011,
    R4    = 0b0100,
    R5    = 0b0101,
    R6    = 0b0110,
    R7    = 0b0111,
    R8    = 0b1000,
    OUT   = 0b1001,
    IN    = 0b1010,
    MEMA  = 0b1011,
    INSTA = 0b1100,
    ZERO  = 0b1101,
    ONE   = 0b1110,
    MIN1  = 0b1111
};

class Assembler {
public:
    Assembler(const std::string& inputPath);
    void assembleTo(const std::string& outputPath, bool addComments);

private:
    std::vector<std::vector<std::string>> mInstructions;
    std::unordered_map<std::string, int> mLabels;

    std::vector<uint8_t> assembleInstruction(uint8_t instIndex, const std::vector<std::string>& inst);
};
