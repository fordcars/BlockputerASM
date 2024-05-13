# BlockputerASM
See [Blockputer](https://github.com/fordcars/Blockputer).

## Building:
```
mkdir build
cd build
cmake ..
make # Or build in Microsoft Visual Studio, etc.
```

## Example
```asm
; Iterate from 0 to R0
; R0: loop count
; R1: current iteration (i)

MVAL 5
MVAR R0        ; Stop loop when ACC == R0
MVRA ZERO      ; Clear ACC
loop:
    ; This is a loop
    MVRA R1    ; Move R1 to ACC (i)
    ADD ONE    ; Add 1 to ACC
    MVRA R1    ; Move ACC back to R1 (i)
    SUB R0     ; ACC -= R0 (our loop count)
    GOTON loop ; GOTO loop if ACC < R0
```
