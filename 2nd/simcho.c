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
/* static char *log_file = (char*) "ika.log"; */
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
uint32_t count[256];
char* outputfile;
uint64_t limit;


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
	log_fp = fopen("log.log", "w");
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
static void configure(int argc, char **argv) {
	int opt;
	err_fp = stderr;
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}
	if (argc == 3){
		outputfile = argv[2];
	}	
	else{
		char b[2] = {'\0'};
                outputfile = b;
	}
	sfile = argv[1];
	if (sfile==NULL) {
		warning("Not Found: source file\n");
		exit(1);
	}

	while ((opt = getopt(argc, argv, "l:")) != -1) {
		switch (opt) {
			case 'l' :
				/* log_file = ""; */
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
	int64_t num1;
        int64_t num2;
        int64_t num3;
        int64_t num4;
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
		 prom[limit] = (num1<<24&0xff000000)+(num2<<16&0xff0000)+(num3<<8&0xff00)+(num4&0xff);
//		print_prom(prom[limit],limit);
        	limit++;
        }


    /* ファイルのクローズ */
    fclose(fp);


}

/*
static void prom_set(int argc, char **argv) {
	int fd, ret;
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}
	sfile = argv[1];
	fd = open(sfile, O_RDONLY);
	if (fd<0) {
		perror("open @ prom_set");
		exit(1);
	}
	ret = read(fd, prom, ROMNUM*4);
	if (ret<0) {
		perror("read");
		exit(1);
	}
	close(fd);
}
*/

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
	print_count();
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
                printf("ADDI %d\n",count[ADDI]);
        }
        if(count[SUBI]!=0){
                printf("SUBI %d\n",count[SUBI]);
        }
        if(count[ADD]!=0){
                printf("ADD %d\n",count[ADD]);
        }
        if(count[SUB]!=0){
                printf("SUB %d\n",count[SUB]);
        }
        if(count[FADD]!=0){
                printf("FADD %d\n",count[FADD]);
        }
        if(count[FSUB]!=0){
                printf("FSUB %d\n",count[FSUB]);
        }
        if(count[FMUL]!=0){
                printf("FMUL %d\n",count[FMUL]);
        }
        if(count[FDIV]!=0){
                printf("FDIV %d\n",count[FDIV]);
        }
        if(count[SRAWI]!=0){
                printf("SRAWI %d\n",count[SRAWI]);
        }
        if(count[SLAWI]!=0){
                printf("SLAWI %d\n",count[SLAWI]);
        }
        if(count[LOAD]!=0){
                printf("LOAD %d\n",count[LOAD]);
        }
        if(count[STORE]!=0){
                printf("STORE %d\n",count[STORE]);
        }
	if(count[LI]!=0){
                printf("LI %d\n",count[LI]);
        }
        if(count[LIS]!=0){
                printf("LIS %d\n",count[LIS]);
        }
        if(count[JUMP]!=0){
                printf("JUMP %d\n",count[JUMP]);
        }
        if(count[BLR]!=0){
                printf("BLR %d\n",count[BLR]);
        }
        if(count[BL]!=0){
                printf("BL %d\n",count[BL]);
        }
        if(count[BLRR]!=0){
                printf("BLRR %d\n",count[BLRR]);
        }
        if(count[BEQ]!=0){
                printf("BEQ %d\n",count[BEQ]);
        }
        if(count[BLE]!=0){
                printf("BLE %d\n",count[BLE]);
        }
        if(count[BLT]!=0){
                printf("BLT %d\n",count[BLT]);
        }
        if(count[CMPD]!=0){
                printf("CMPD %d\n",count[CMPD]);
        }
        if(count[CMPF]!=0){
                printf("CMPF %d\n",count[CMPF]);
        }
        if(count[CMPDI]!=0){
                printf("CMPDI %d\n",count[CMPDI]);
        }
        if(count[INLL]!=0){
                printf("INLL %d\n",count[INLL]);
        }
        if(count[INLH]!=0){
                printf("INLH %d\n",count[INLH]);
        }
        if(count[INUL]!=0){
                printf("INUL %d\n",count[INUL]);
        }
        if(count[INUH]!=0){
                printf("INUH %d\n",count[INUH]);
        }
        if(count[OUTLL]!=0){
                printf("OUTLL %d\n",count[OUTLL]);
        }
        if(count[OUTLH]!=0){
                printf("OUTLH %d\n",count[OUTLH]);
        }
        if(count[OUTUL]!=0){
                printf("OUTULL %d\n",count[OUTUL]);
        }
        if(count[OUTUH]!=0){
                printf("OUTUH %d\n",count[OUTUH]);
        }
        if(count[NOP]!=0){
                printf("NOP %d\n",count[NOP]);
        }
        if(count[XOR]!=0){
                printf("XOR %d\n",count[XOR]);
        }
        if(count[AND]!=0){
                printf("AND %d\n",count[AND]);
        }
        if(count[OR]!=0){
                printf("OR %d\n",count[OR]);
        }
        if(count[FTOI]!=0){
                printf("FTOI%d\n",count[FTOI]);
        }
        if(count[ITOF]!=0){
                printf("ITOF %d\n",count[ITOF]);
        }
        if(count[FSQRT]!=0){
                printf("FSQRT%d\n",count[FSQRT]);
        }
        if(count[FLOOR]!=0){
                printf("FLOOR %d\n",count[FLOOR]);
        }

}


#undef print_val
