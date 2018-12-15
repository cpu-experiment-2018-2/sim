#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "fpu/ftools.h"
#include "fpu/fpu.h"
#include "fpu/fdiv.h"
#include "include/oc_sim.h"
#include "include/common.h"
#include "include/print_reg.h"
unsigned int tmp_un;

//一つ一つに命令が入る
uint32_t prom[ROMNUM];
uint32_t promjmp[ROMNUM][ROMNUM] = {0};
uint32_t promcmpd[ROMNUM][3] = {0};
//メモリ
uint32_t ram[RAMNUM];
float tmp;
/*
汎用レジスタ
%r0 ゼロレジスタ
%r1 スタックの先頭
%r2 スタックフレーム
%r3~%r14 int
%r15~%r26 float
%r31 リンクレジスタ
*/
int32_t reg[REGNUM];
uint32_t freg[REGNUM];
uint32_t count[256] = {0};
int cdr;
uint32_t pc;
//命令
uint32_t ir;
int32_t lr;
//何命令行ったか？
uint64_t cnt;
uint64_t limit;

FILE *fpout;
struct timeval start;

void print_count(void);

void print_jmp(uint32_t promjmp[ROMNUM][ROMNUM]) {
  printf("========= jump result ==========\n\n");
  for (int i = 0; i < limit; i++) {
    for (int j = 0; j < limit; j++) {
      if (promjmp[i][j] != 0) {
        print_prom(prom[i], i);
        printf("%d回実行\n", promjmp[i][j]);
      }
    }
  }
  printf("\n\n");
}

void print_cmpd(uint32_t promcmpd[ROMNUM][3]) {
  printf("========= cmpd result ==========\n\n");
  for (int i = 0; i < limit; i++) {
    if (promcmpd[i][0] != 0 || promcmpd[i][1] != 0 || promcmpd[i][2] != 0) {
      print_prom(prom[i], i);
      printf("ge %d回 eq %d回 le %d回\n", promcmpd[i][ge], promcmpd[i][eq],
             promcmpd[i][le]);
    }
  }
  printf("\n\n");
}

void print_jmpd(uint32_t promjmp[ROMNUM][ROMNUM],
                uint32_t promcmpd[ROMNUM][3]) {
  printf("========= jmpd result ==========\n\n");
  for (int i = 0; i < limit; i++) {
    for (int j = 0; j < limit; j++) {
      if (promjmp[i][j] != 0) {
        print_prom(prom[i], i);
        printf("%d回実行\n", promjmp[i][j]);
      }
    }
    if (promcmpd[i][0] != 0 || promcmpd[i][1] != 0 || promcmpd[i][2] != 0) {
      print_prom(prom[i], i);
      printf("ge %d回 eq %d回 le %d回\n", promcmpd[i][ge], promcmpd[i][eq],
             promcmpd[i][le]);
    }
  }
  printf("\n\n");
}
double elapsed_time() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return (double)(now.tv_sec - start.tv_sec) +
         (double)(now.tv_usec - start.tv_usec) / 1000000.0;
}
//レジスタの規定
static inline void init(void) {
  if (*outputfile == '\0') {
    fpout = fopen("d.txt", "a");
  } else {
    fpout = fopen(outputfile, "a");
  }
  // register init
  reg[0] = 0;
  reg[1] = 0;
  reg[2] = 0;
  ram[0] = 0;
  lnk = 0;
  cdr = 0;
  // heap init なにこれ?
  pc = 0;
  gettimeofday(&start, NULL);
  /*
          for (pc = 1; pc*4 <= reg[2]; pc++) {
                  ram[pc-1] = prom[pc];
          }
  */
}

static inline int exec_op(uint32_t ir);

//命令がある限りpcを進めて命令を読んで実行
int simulate(void) {
  init();
  do {

    ir = prom[pc];

#ifndef SILENT
    if (cnt == 0)
      printf("初期状態\n");

#endif


#ifdef LOG_FLAG
    _print_ir(ir, log_fp);
#endif
#ifdef ANALYSE_FLAG
    analyse(ir);
#endif
    cnt++;
    pc++;
    if (!(cnt % 10000000)) {
      fprintf(stderr,".");
      if (!(cnt % 1000000000)) {
        fprintf(stderr,"time %.3f [sec],operation_count = %ld hp = %d \n",
                elapsed_time(), cnt, ram[0]);
      }
    }

    float f;



    /* printf("pc %d\n", pc); */
    /* print_opcode(ir); */
    /* for (int i = 0; i < 10; i++) { */
    /*   printf("reg[%d] int %d float %f\n", i + 3, reg[i + 3], */
    /*          *(float *)(&reg[i + 3])); */
    /* } */
    /* for (int i = 0; i < 10; i++) { */
    /*   printf("ram[%d] int %d float %f\n", i, ram[i], */
    /*          *(float *)(&ram[i])); */
    /* } */



//ゼロレジスタを表示
#ifndef SILENT
    printf("pc %d\n", pc);
    printf("実行命令 ");
    print_opcode(ir);
    //命令実行後ではなく命令実行時の現在のレジスタ状態を表示
    printf("ゼロレジスタ %d\n", reg[0]);
    printf("スタックの先頭　%d\n", reg[1]);
    printf("スタックフレーム %d\n", reg[2]);
    printf("リンクレジスタ %d\n", reg[31]);
    printf("コンディションレジスタ ");
    print_cdr(cdr);
    //整数レジスタのint版とfloat版を表示
    for (int i = 0; i < 10; i++) {
      printf("reg[%d] int %d float %f\n", i, reg[i],
             *(float *)(&reg[i]));
    }
//      printf("_GRA %d\n %d", _GRA, get_rai ); 

    for (int i = 0; i < 10; i++) {
      printf("ram[%d] int %d float %f\n", i, ram[i],
             *(float *)(&ram[i]));
    }
#endif
    if (ir == 0) {
      printf("終了\n");
      break;
    }
  } while (exec_op(ir) == 0);
  print_jmpd(promjmp, promcmpd);
  printf("total order is %ld\n" , cnt);
  return 0;
}

static inline int exec_op(uint32_t ir) {
  uint8_t opcode, funct;
  union {
    uint32_t i;
    float f;
  } a, b, out;
  char c;
  float ra = 0.0;
  float rb = 0.0;
  float rt = 0.0;
  float resultf = 0.0;
  int32_t si;
  opcode = get_opcode(ir);
  int p;

  switch (opcode) {
  case AND:
    _GRT = (_GRA & _GRB);
    count[opcode] += 1;
    /* printf("AND %d %d %d\n",get_rti(ir),get_rai(ir),get_rbi(ir)); */
    break;
  case OR:
    _GRT = (_GRA | _GRB);
    count[opcode] += 1;
    break;
  case XOR:
    _GRT = (_GRA ^ _GRB);
    count[opcode] += 1;
    break;
  case ADDI:
si = _SI;
    if (((si >> 15) & 0x1) == 1) {
      tmp_un = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 15) & 0x1) == 0) {
      tmp_un = (0xffff & si);
    }
    si = *(int*)(&tmp_un); 

    _GRT = _GRA + si;
    count[opcode] += 1;
    break;
  case SUBI:
si = _SI;
    if (((si >> 15) & 0x1) == 1) {
      tmp_un = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 15) & 0x1) == 0) {
      tmp_un = (0xffff & si);
    }
    si = *(int*)(&tmp_un); 

    _GRT = _GRA - si;
    count[opcode] += 1;
    break;
  case ADD:
    _GRT = _GRA + _GRB;
    count[opcode] += 1;
    break;
  case SUB:
    _GRT = _GRA - _GRB;
    count[opcode] += 1;
    break;
  case FADD:
#ifndef FPU
    ra = *(float*)(&(_GRA));
    rb = *(float*)(&(_GRB));
    resultf = ra+rb;
    _GRT = *(int*)(&(resultf));
#else
    _GRT = fadd(_GRA, _GRB);
#endif
    count[opcode] += 1;
    break;
  case FSUB:
#ifndef FPU
    ra = *(float*)(&(_GRA));
    rb = *(float*)(&(_GRB));
    resultf = ra-rb;
    _GRT = *(int*)(&(resultf));
#else
    _GRT = fsub(_GRA, _GRB);
#endif
    count[opcode] += 1;
    break;
  case FMUL:

#ifndef FPU
            ra = *(float*)(&(_GRA));
            rb = *(float*)(&(_GRB));
            resultf = ra*rb;
            _GRT = *(int*)(&(resultf));
#else
    _GRT = fmul(_GRA, _GRB);
#endif
    count[opcode] += 1;
    break;
  case FDIV:
#ifndef FPU
            ra = *(float*)(&(_GRA));
            rb = *(float*)(&(_GRB));
            resultf = ra/rb;
            _GRT = *(int*)(&resultf);
#else
    _GRT = fdiv(_GRA, _GRB);
#endif
    count[opcode] += 1;
    break;
  case ITOF:
    tmp = (float)_GRA;
    _GRT = *(int *)(&tmp);
    count[opcode] += 1;
    break;
  case FTOI:
    _GRT = (int)(*(float *)(&_GRA));
    count[opcode] += 1;
    break;
  case FSQRT:
    tmp = sqrtf(*(float *)&_GRA);
    _GRT = *(int *)(&tmp);
    count[opcode] += 1;
    break;
  case FLOOR:
    tmp = floor(*(float *)&_GRA);
    _GRT = *(int *)(&tmp);
    count[opcode] += 1;
    break;

  case SRAWI:
    //_GRAはint32_tなので多分これで大丈夫
    _GRT = _GRA >> _SI;
    count[opcode] += 1;
    break;
  case SLAWI:
    _GRT = _GRA << _SI;
    count[opcode] += 1;
    break;
  //メモリから代入 ram
  case LOAD:
    /* if(get_rti(ir) == 31){ */
    /*     printf("load %d %d %d %d %d\n",pc,_GRA,_SI, _GRA+_SI,ram[(_GRA + _SI)]); */
    /* } */
    si = _SI;
    if (((si >> 15) & 0x1) == 1) {
      tmp_un = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 15) & 0x1) == 0) {
      tmp_un = (0xffff & si);
    }
    si = *(int*)(&tmp_un); 

    /* printf("hoge load %d %d %d %d %d\n",pc,_GRA,_SI, _GRA+_SI,ram[(_GRA + _SI)]);  */
    /* printf("fuga load %d %d %d %d %d\n",pc,_GRA,si, _GRA+si,ram[(_GRA + si)]);  */

    _GRT = ram[(_GRA + si)];
    count[opcode] += 1;
    break;
  //メモリに代入
  case STORE:
    si = _SI;
    if (((si >> 15) & 0x1) == 1) {
      tmp_un = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 15) & 0x1) == 0) {
      tmp_un = (0xffff & si);
    }
    si = *(int*)(&tmp_un); 
    /* printf("piyo store %d %d %d %d %d %d\n",pc,_GRA,_SI, _GRA+_SI,ram[(_GRA + _SI)], _GRT);  */
    /* printf("nyan store %d %d %d %d %d %d\n",pc,_GRA,si, _GRA+si,ram[(_GRA + si)],_GRT);  */

    ram[((_GRA + si))] = _GRT;
    count[opcode] += 1;
    break;
  case LI:
    si = _SI;
    if (((si >> 31) & 0x1) == 1) {
      _GRT = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 31) & 0x1) == 0) {
      _GRT = (0xffff & si);
    }
    count[opcode] += 1;
    break;
  case LIS:
    //_GRT = (_SI<<16) | (_GRT & ((1<<16)-1)) ;
    //
    // printf("LIS %d\n",get_rti(ir));
    _GRT = ((_SI & 0xffff) << 16) | (_GRT & ((1 << 16)-1));
    count[opcode] += 1;
    break;
  case JUMP:
    if (pc-1 == get_li(ir)) {
            return 1;
    }
    promjmp[pc - 1][get_li(ir)] += 1;
    pc = get_li(ir);

    count[opcode] += 1;
    break;
  case BLR:
    /* if (pc == lnk) */
    /*   return 1; */
    /* printf("pc %d %d \n",pc,lnk); */
    promjmp[pc - 1][lnk] += 1;
    pc = lnk;
    count[opcode] += 1;
    break;
  case BL:
    lnk = pc;
    /* printf("save  %d at %d \n",lnk,_LI); */
    promjmp[pc - 1][_LI] += 1;
    pc = _LI;
    count[opcode] += 1;
    break;
  case BLRR:
    lnk = pc;
    promjmp[pc - 1][_GRT] += 1;
    pc = _GRT;
    count[opcode] += 1;
    break;
  case BEQ:
    if (cdr == eq) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;
  case BLE:
    if (cdr == eq || cdr == le) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;
  case BLT:
    if (cdr == le) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;
  case BNE:
    if (cdr != eq) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;
  case BGE:
    if (cdr != le) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;
  case BGT:
    if (cdr != le && cdr != eq) {
      promjmp[pc - 1][_LI] += 1;
      pc = _LI;
    }
    count[opcode] += 1;
    break;

  case CMPD:
    if (_GRA == _GRB) {
      cdr = eq;
      promcmpd[pc - 1][eq] += 1;
    } else if (_GRA < _GRB) {
      cdr = le;
      promcmpd[pc - 1][le] += 1;
    } else {
      cdr = 0;
      promcmpd[pc - 1][ge] += 1;
    }
    count[opcode] += 1;
    break;
  case CMPF:
    rt = *(float *)(&(_GRA));
    ra = *(float *)(&(_GRB));
    if (rt == ra) {
      cdr = eq;
      promcmpd[pc - 1][eq] += 1;
    } else if (rt < ra) {
      cdr = le;
      promcmpd[pc - 1][le] += 1;
    } else {
      cdr = 0;
      promcmpd[pc - 1][ge] += 1;
    }
    count[opcode] += 1;
    break;
  case CMPDI:
    si = _SI;
    if (((si >> 15) & 0x1) == 1) {
      tmp_un = (0xffff0000 | (si & 0xffff));
    } else if (((si >> 15) & 0x1) == 0) {
      tmp_un = (0xffff & si);
    }
    si = *(int*)(&tmp_un); 
    /* printf("%d %d %d\n",_GRA,_SI,si); */
    if (_GRA == si) {
      cdr = eq;
      promcmpd[pc - 1][eq] += 1;
    } else if (_GRA < si) {
 //      printf("LT %d %d\n",_GRT,_SI);
      cdr = le;
      promcmpd[pc - 1][le] += 1;
    } else {
//      printf("ELSE %d %d\n",_GRT,_SI);
      cdr = 0;
      promcmpd[pc - 1][ge] += 1;
    }
    count[opcode] += 1;
    break;
  case INLL:
    scanf("%d", &p);
    _GRT = (_GRT & 0xffffff00) | (p & 0xff);
    count[opcode] += 1;
    break;
  case INLH:
    scanf("%d", &p);
    _GRT = (_GRT & 0xffff00ff) | (p << 8 & 0xff00);
    count[opcode] += 1;
    break;
  case INUL:
    scanf("%d", &p);
    _GRT = (_GRT & 0xff00ffff) | (p << 16 & 0xff0000);
    count[opcode] += 1;
    break;
  case INUH:
    scanf("%d", &p);
    _GRT = (_GRT & 0x00ffffff) | (p << 24 & 0xff000000);
    count[opcode] += 1;
    break;
  case OUTLL:
    fprintf(fpout, "%d\n", (_GRT >> 0) & 0xff);
    count[opcode] += 1;
    break;
  case OUTLH:
    fprintf(fpout, "%d\n", (_GRT >> 8) & 0xff);
    count[opcode] += 1;
    break;
  case OUTUL:
    fprintf(fpout, "%d\n", (_GRT >> 16) & 0xff);
    count[opcode] += 1;
    break;
  case OUTUH:
    fprintf(fpout, "%d\n", (_GRT >> 24) & 0xff);
    count[opcode] += 1;
    break;
  case END:
    /* for (int i = 0; i < 30; i++) { */
    /*   printf("reg[%d] int %d float %f\n", i , reg[i], */
    /*          *(float *)(&reg[i ])); */
    /* } */
    /*  */
    /* for (int i = 0; i < 400; i++) { */
    /*   printf("ram[%d] int %d float %f\n", i, ram[i], */
    /*          *(float *)(&ram[i])); */
    /* } */
    printf("終了\n");
  ;

    count[opcode] += 1;
    return 1;
  case NOP:
    count[opcode] += 1;
    break;
  default:
    printf("invalid op %d\n",opcode);
    return 1; // break;
  }

  return 0;
}
