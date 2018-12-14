
#ifndef COMMONHEAD
#define COMMONHEAD
#define INSTNUM 64	// 6bit 63
#define REGNUM 32
#define ROMNUM (64 * 1024) // words(32bit)
//メモリのこと？
#define RAMNUM (1024 * 1024)

#define ADDI  0b000000
#define SUBI  0b000001
#define ADD  0b000010
#define SUB  0b000011
#define SRAWI 0b000100
#define SLAWI 0b000101


#define FADD  0b001000
#define FSUB  0b001001
#define FMUL  0b001010
#define FDIV  0b001011
#define FTOI  0b001100
#define ITOF  0b001101
#define FSQRT  0b001110
#define FLOOR  0b001111

#define LOAD  0b010000
#define STORE  0b010001
#define LI  0b010010
#define LIS  0b010011
#define AND 0b010100
#define OR 0b010101
#define XOR 0b010111


#define JUMP   0b011000
#define BLR	0b011001
#define BL	0b011010
#define BLRR	0b011011
#define CMPD  0b011100
#define CMPF  0b011101
#define CMPDI 0b011110


#define BEQ  0b100000
#define BLE  0b100001
#define BLT  0b100010
#define BNE  0b100011
#define BGE  0b100100
#define BGT  0b100101


#define INLL  0b101000
#define INLH  0b101001
#define INUL  0b101010
#define INUH  0b101011
#define OUTLL  0b101100
#define OUTLH  0b101101
#define OUTUL  0b101110
#define OUTUH  0b101111
#define NOP 0b111000
#define END 0b111001

#endif
