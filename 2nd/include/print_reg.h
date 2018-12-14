#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "common.h"
void print_opcode(int32_t ir) {

  switch (get_opcode(ir)) {
  case AND:
    printf("AND ");
    printf("%x\n", ir);
    break;
  case OR:
    printf("OR ");
    printf("%x\n", ir);
    break;
  case XOR:
    printf("XOR ");
    printf("%x\n", ir);
    break;
  case ADDI:
    printf("ADDI ");
    printf("%x\n", ir);
    break;
  case SUBI:
    printf("SUBI ");
    printf("%x\n", ir);
    break;
  case ADD:
    printf("ADD ");
    printf("%x\n", ir);
    break;
  case SUB:
    printf("SUB ");
    printf("%x\n", ir);
    break;
  case FADD:
    printf("FADD ");
    printf("%x\n", ir);
    break;
  case FSUB:
    printf("FSUB ");
    printf("%x\n", ir);
    break;
  case FMUL:
    printf("FMUL ");
    printf("%x\n", ir);
    break;
  case FDIV:
    printf("FDIV ");
    printf("%x\n", ir);
    break;
  case SRAWI:
    printf("SRAWI ");
    printf("%x\n", ir);
    break;
  case SLAWI:
    printf("SLAWI ");
    printf("%x\n", ir);
    break;
  //メモリから代入 ram
  case LOAD:
    printf("LOAD ");
    printf("%x\n", ir);
    break;
  //メモリに代入
  case STORE:
    printf("STORE ");
    printf("%x\n", ir);
    break;
  case LI:
    printf("LI ");
    printf("%x\n", ir);
    break;
  case LIS:
    printf("LIS ");
    printf("%x\n", ir);
    break;
  case JUMP:
    printf("JUMP ");
    printf("%x\n", ir);
    break;
  case BLR:
    printf("BLR ");
    printf("%x\n", ir);
    break;
  case BL:
    printf("BL ");
    printf("%x\n", ir);
    break;
  case BLRR:
    printf("BLRR ");
    printf("%x\n", ir);
    break;
  case BEQ:
    printf("BEQ ");
    printf("%x\n", ir);
    break;
  case BLE:
    printf("BLE ");
    printf("%x\n", ir);
    break;
  case BLT:
    printf("BLT ");
    printf("%x\n", ir);
    break;
  case BNE:
    printf("BNE ");
    printf("%x\n", ir);
    break;
  case BGE:
    printf("BGE ");
    printf("%x\n", ir);
    break;
  case BGT:
    printf("BGT ");
    printf("%x\n", ir);
    break;

  case CMPD:
    printf("CMPD ");
    printf("%x\n", ir);
    break;
  case CMPF:
    printf("CMPF ");
    printf("%x\n", ir);
    break;
  case CMPDI:
    printf("CMPDI ");
    printf("%x\n", ir);
    break;
  case INLL:
    printf("INLL ");
    printf("%x\n", ir);
    break;
  case INLH:
    printf("INLH ");
    printf("%x\n", ir);
    break;
  case INUL:
    printf("INUL ");
    printf("%x\n", ir);
    break;
  case INUH:
    printf("INUH ");
    printf("%x\n", ir);
    break;
  case OUTLL:
    printf("OUTLL ");
    printf("%x\n", ir);
    break;
  case OUTLH:
    printf("OUTLH ");
    printf("%x\n", ir);
    break;
  case OUTUL:
    printf("OUTUL ");
    printf("%x\n", ir);
    break;
  case OUTUH:
    printf("OUTUH ");
    printf("%x\n", ir);
    break;
  case NOP:
    printf("NOP ");
    printf("%x\n", ir);
    break;
  default:
    printf("don't exist ");
    printf("%x\n", ir);
    break;
  }
}

void print_cdr(int cdr) {

  if (cdr == eq) {
    printf("eq\n");
  } else if (cdr == le) {
    printf("le\n");
  } else {
    printf("フラグなし\n");
  }
}

void print_prom(int32_t ir, int n) {

  int opcode = get_opcode(ir);
  printf("%d: ", n);
  switch (opcode) {
  case ADDI:
    printf("ADDI ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case SUBI:
    printf("SUBI ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case ADD:
    printf("ADD ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case SUB:
    printf("SUB ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case FADD:
    printf("FADD ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case FSUB:
    printf("FSUB ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case FMUL:
    printf("FMUL ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case FDIV:
    printf("FDIV ");
    printf("r%d r%d r%d\n", get_rti(ir), get_rai(ir), get_rbi(ir));
    break;
  case SRAWI:
    printf("SRAWI ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case SLAWI:
    printf("SLAWI ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case LOAD:
    printf("LOAD ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case STORE:
    printf("STORE ");
    printf("r%d r%d %d\n", get_rti(ir), get_rai(ir), get_si(ir));
    break;
  case LI:
    printf("LI ");
    printf("r%d %d\n", get_rti(ir), get_si(ir));
    break;
  case LIS:
    printf("LIS ");
    printf("r%d %d\n", get_rti(ir), get_si(ir));
    break;
  case JUMP:
    printf("JUMP ");
    printf("%d\n", get_li(ir));
    break;
  case BLR:
    printf("BLR\n");
    break;
  case BL:
    printf("BL ");
    printf("%d\n", get_li(ir));
    break;
  case BLRR:
    printf("BLRR ");
    printf("r%d\n", get_rti(ir));
    break;
  case BEQ:
    printf("BEQ ");
    printf("%d\n", get_li(ir));
    break;
  case BLE:
    printf("BLE ");
    printf("%d\n", get_li(ir));
    break;
  case BLT:
    printf("BLT ");
    printf("%d\n", get_li(ir));
    break;
  case CMPD:
    printf("CMPD ");
    printf("r%d r%d \n", get_rti(ir), get_rai(ir));
    break;
  case CMPF:
    printf("CMPF ");
    printf("r%d r%d \n", get_rti(ir), get_rai(ir));
    break;
  case CMPDI:
    printf("CMPDI ");
    printf("r%d %d \n", get_rti(ir), get_si(ir));
    break;
  case INLL:
    printf("INLL ");
    printf("r%d\n", get_rti(ir));
    break;
  case INLH:
    printf("INLH ");
    printf("r%d\n", get_rti(ir));
    break;
  case INUL:
    printf("INUL ");
    printf("r%d\n", get_rti(ir));
    break;
  case INUH:
    printf("INUH ");
    printf("r%d\n", get_rti(ir));
    break;
  case OUTLL:
    printf("OUTLL ");
    printf("r%d\n", get_rti(ir));
    break;
  case OUTLH:
    printf("OUTLH ");
    printf("r%d\n", get_rti(ir));
    break;
  case OUTUL:
    printf("OUTUL ");
    printf("r%d\n", get_rti(ir));
    break;
  case OUTUH:
    printf("OUTUH ");
    printf("r%d\n", get_rti(ir));
    break;
  case NOP:
    printf("NOP \n");
    break;
  default:
    printf("not defined\n");
  }
}
