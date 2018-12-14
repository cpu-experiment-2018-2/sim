obj = gcc -c -g -O3 -fPIC -lm -D"SILENT=1"
app = gcc -O3 -lm -D"SILENT=1"
# obj = gcc -c -g -O2 -fPIC -lm 
# app = gcc -O2 -lm 
# FPUフラグを立てるとFPU実装が動く
# obj = gcc -c -g -O3 -fPIC -lm -D"SILENT=1 FPU=1"
# app = gcc -O3 -lm -D"SILENT=1 FPU=1"



.PHONY: [sim]

all: sim simcho.o simulate.o ftools.o fpu.o fdiv.o

simulate.o: simulate.c
	$(obj) simulate.c

simcho.o: simcho.c
	$(obj) simcho.c

ftools.o: ./fpu/ftools.c
	$(obj) ./fpu/ftools.c
fpu.o: ./fpu/fpu.c
	$(obj) ./fpu/fpu.c

fdiv.o: ./fpu/fdiv.c
	$(obj) ./fpu/fdiv.c

sim: simcho.o simulate.o ftools.o fpu.o fdiv.o
	$(app) simcho.o simulate.o fpu.o fdiv.o ftools.o -o sim 

silent: clean
	$(obj) simcho.c -D"SILENT=1"
	$(obj) simulate.c -D"SILENT=1"
	$(obj) ./fpu/ftools.c -D"SILENT=1"
	$(obj) ./fpu/fpu.c -D"SILENT=1"
	$(app) simcho.o simulate.o ftools.o -o sim

clean: 
	rm -rf ./*.o
	rm -rf ./sim
