#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include "include/oc_sim.h"
#include "include/print_reg.h"
static char *log_file = (char*) "ika.log";
static char *sfile;
static void configure(int argc, char **argv);
static void register_segv_handler(void);
static void prom_set(int argc, char **argv);
static void print_conf(void);
int simulate(void);
static void print_elapsed_time(void);
static void print_result(void);
static void print_usage(char*);
FILE *err_fp;
FILE *log_fp;
static void open_log_file(void);
void print_count(void);
uint64_t count[256];
char* outputfile;
char* inputfile;
uint32_t limit;


static struct timeval tv1,tv2;
int main(int argc, char **argv, char **envp) {
	configure(argc, argv);
	register_segv_handler();
	open_log_file();
	prom_set(argc, argv);
	print_conf();

	gettimeofday(&tv1, NULL);
	simulate();
	gettimeofday(&tv2, NULL);

	print_result();
	
	exit(0);
}

static void open_log_file(void) {
#ifdef LOG_FLAG
	log_fp = fopen(log_file, "w");
	if (log_fp==NULL) {
		perror("fopen log_file");
		exit(1);
	}
	fprintf(log_fp, "running %s\n", sfile);
	fprintf(log_fp, "cnt(hex).[pc(hex)]\n");
#endif
}


#define print_option(fmt, ...) \
	warning("\t"fmt"\n", ##__VA_ARGS__)
static void print_usage(char*name) {
	warning("\n");
	warning("USAGE\t: %s $file [$option]\n", name);
	warning("OPTIONS\t:\n");
	print_option("-l [$log_file]\t: output log if LOG_FLAG macro is defined");
	warning("\n");
}
#undef print_option


//inとoutの設定
//argv[1]は命令ファイル　argv[2]は出力ファイル argv[3]は入力ファイル
static void configure(int argc, char **argv) {
	int opt;
	char in[2] = {'\0'};
	err_fp = stderr;
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}
	if (argc == 4){
		outputfile = argv[2];
		inputfile = argv[3];
	}
	else if (argc == 3){
		outputfile = argv[2];
                inputfile = in;
	}	
	else{
		char b[2] = {'\0'};
                outputfile = b;
                inputfile = in;
	}
	sfile = argv[1];
	if (sfile==NULL) {
		warning("Not Found: source file\n");
		exit(1);
	}

	while ((opt = getopt(argc, argv, "l:")) != -1) {
		switch (opt) {
			case 'l' :
				log_file = optarg;
				break;

			default :
				print_usage(argv[0]);
				exit(1);
				break;
		}
	}
}

	

//prom[ROMNUM]にファイルの中身をセット
//prom[(64 * 1024)] // words(32bit)
static void prom_set(int argc,char **argv) {

	if (argc < 2) {
                print_usage(argv[0]);
                exit(1);
        }


FILE *fp;
    char *filename = argv[1];
    char readline[32] = {'\0'};
        limit = 0;


    /* ファイルのオープン */
    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "%sのオープンに失敗しました.\n", filename);
        exit(EXIT_FAILURE);
    }
/*	int64_t num1;
        int64_t num2;
        int64_t num3;
        int64_t num4;
	int64_t num5;
        int64_t num6;
        int64_t num7;
        int64_t num8;
	int i = 1;
        while(i==1){
                 if( fread(&num1, 1, 1, fp ) < 1 ){
                        break;
                }
                if( fread(&num2, 1, 1, fp ) < 1 ){
                        break;
                        }
                if( fread(&num3, 1, 1, fp ) < 1 ){
                        break;
                }
                if( fread(&num4, 1, 1, fp ) < 1 ){
                        break;
                }
                 if( fread(&num5, 1, 1, fp ) < 1 ){
                        break;
                }
                if( fread(&num6, 1, 1, fp ) < 1 ){
                        break;
                        }
                if( fread(&num7, 1, 1, fp ) < 1 ){
                        break;
                }
                if( fread(&num8, 1, 1, fp ) < 1 ){
                        break;
                }
*/
	int N = 100;
	char str[N];
	uint64_t num;
	while(fgets(str, N, fp) != NULL) {
		num = 0;
		for(int i=0;i<64;i++){
			if(str[i]=='1'){
				num = num|(((uint64_t)1)<<(63-i));
			}
		}
//		prom[limit]=((num1<<56)&0xff00000000000000)+((num2<<48)&0xff000000000000)+(num3<<40&0xff0000000000)+(num4<<32&0xff00000000)+(num5<<24&0xff000000)+(num6<<16&0xff0000)+(num7<<8&0xff00)+(num8&0xff);
		prom[limit] = num;
//		print_prom(prom[limit],limit);  
//		print_data64(num);
    		limit+=1;
       }


    /* ファイルのクローズ */
    fclose(fp);


}


void segv_handler(int n) {
	exit(1);
/*	uint32_t ir = prom[pc-1];
	warning("せぐふぉー@%lu.[%x] ir:%08X ", cnt, pc, ir);
	print_ir(ir);
	warning("\n");
	exit(1);
*/
}
static void register_segv_handler(void) {
	struct sigaction sa;
	sa.sa_handler = segv_handler;
	if (sigaction(SIGSEGV, &sa, NULL) != 0) {
		perror("sigaction");
		exit(1);
	}

}

#define print_val(fmt, ...) \
	warning("* "fmt"\n", ##__VA_ARGS__)
static void print_conf(void) {
	warning("\n");
	warning("######################## SIMCHO CONFIGURATION ########################\n\n");
	print_val("source\t: %s", sfile);

#ifdef LOG_FLAG
	print_val("log file\t: %s", log_file);
#else
	print_val("log file\t: no output");
#endif
#ifdef ANALYSE_FLAG
	print_val("analyse\t: on");
#else
	print_val("analyse\t: off (enabled if ANALYSE_FLAG macro is defined)");
#endif
	warning_nl();
	warning("========================== RUNNING PROGRAM ===========================\n");
}
static void print_result(void) {
	warning("[sim's newline]\n");
	warning("========================== SIMULATOR RESULT ==========================\n");
	warning_nl();
	print_elapsed_time();
	print_val("cnt\t: %llu", cnt);
#ifdef ANALYSE_FLAG
	print_val("analyse result");
	print_analysis(stderr);
#endif
	warning_nl();
	warning("######################################################################\n");
	warning_nl();

}

static void print_elapsed_time(void) {
	int elapsed_sec, elapsed_usec;
	elapsed_sec = tv2.tv_sec - tv1.tv_sec;
	elapsed_usec = tv2.tv_usec - tv1.tv_usec;
	if (elapsed_sec == 0) {
		print_val("elapsed_time\t: %u [us]", elapsed_usec);
	} else {
		print_val("elapsed_time\t: %lf [s]", (double)elapsed_sec + elapsed_usec*1e-6);
	}
}

void print_count(void){

        printf("\n使った命令とその回数\n");

        if(count[ADDI]!=0){
                printf("ADDI %llu\n",count[ADDI]);
        }
        if(count[SUBI]!=0){
                printf("SUBI %llu\n",count[SUBI]);
        }
        if(count[ADD]!=0){
                printf("ADD %llu\n",count[ADD]);
        }
        if(count[SUB]!=0){
                printf("SUB %llu\n",count[SUB]);
        }
        if(count[SRAWI]!=0){
                printf("SRAWI %llu\n",count[SRAWI]);
        }
        if(count[SLAWI]!=0){
                printf("SLAWI %llu\n",count[SLAWI]);
        }
        if(count[XOR]!=0){
                printf("XOR %llu\n",count[XOR]);
        }
        if(count[AND]!=0){
                printf("AND %llu\n",count[AND]);
        }
        if(count[FADD]!=0){
                printf("FADD %llu\n",count[FADD]);
        }
        if(count[FSUB]!=0){
                printf("FSUB %llu\n",count[FSUB]);
        }
        if(count[FMUL]!=0){
                printf("FMUL %llu\n",count[FMUL]);
        }
        if(count[FDIV]!=0){
                printf("FDIV %llu\n",count[FDIV]);
        }
        if(count[FTOI]!=0){
                printf("FTOI %llu\n",count[FTOI]);
        }
        if(count[ITOF]!=0){
                printf("ITOF %llu\n",count[ITOF]);
        }
        if(count[FSQRT]!=0){
                printf("FSQRT %llu\n",count[FSQRT]);
        }
        if(count[LOAD]!=0){
                printf("LOAD %llu\n",count[LOAD]);
        }
        if(count[STORE]!=0){
                printf("STORE %llu\n",count[STORE]);
        }
	if(count[LI]!=0){
                printf("LI %llu\n",count[LI]);
        }
        if(count[LIW]!=0){
                printf("LIW %llu\n",count[LIW]);
        }
        if(count[JUMP]!=0){
                printf("JUMP %llu\n",count[JUMP]);
        }
        if(count[BLR]!=0){
                printf("BLR %llu\n",count[BLR]);
        }
        if(count[BL]!=0){
                printf("BL %llu\n",count[BL]);
        }
        if(count[BLRR]!=0){
                printf("BLRR %llu\n",count[BLRR]);
        }
        if(count[CMPD]!=0){
                printf("CMPD %llu\n",count[CMPD]);
        }
        if(count[CMPF]!=0){
                printf("CMPF %llu\n",count[CMPF]);
        }
        if(count[CMPDI]!=0){
                printf("CMPDI %llu\n",count[CMPDI]);
        }
        if(count[BEQ]!=0){
                printf("BEQ %llu\n",count[BEQ]);
        }
        if(count[BLE]!=0){
                printf("BLE %llu\n",count[BLE]);
        }
        if(count[BLT]!=0){
                printf("BLT %llu\n",count[BLT]);
        }
        if(count[BNE]!=0){
                printf("BNE %llu\n",count[BNE]);
        }
        if(count[BGE]!=0){
                printf("BLE %llu\n",count[BGE]);
        }
        if(count[BGT]!=0){
                printf("BLT %llu\n",count[BGT]);
        }
        if(count[INLL]!=0){
                printf("INLL %llu\n",count[INLL]);
        }
        if(count[INLH]!=0){
                printf("INLH %llu\n",count[INLH]);
        }
        if(count[INUL]!=0){
                printf("INUL %llu\n",count[INUL]);
        }
        if(count[INUH]!=0){
                printf("INUH %llu\n",count[INUH]);
        }
        if(count[OUTLL]!=0){
                printf("OUTLL %llu\n",count[OUTLL]);
        }
        if(count[OUTLH]!=0){
                printf("OUTLH %llu\n",count[OUTLH]);
        }
        if(count[OUTUL]!=0){
                printf("OUTULL %llu\n",count[OUTUL]);
        }
        if(count[OUTUH]!=0){
                printf("OUTUH %llu\n",count[OUTUH]);
        }
        if(count[NOP]!=0){
                printf("NOP %llu\n",count[NOP]);
        }
        if(count[END]!=0){
                printf("END %llu\n",count[END]);
        }


}


#undef print_val
