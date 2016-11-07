all: main.c
	gcc -std=c99 -g -Wall -o sim_bp main.c bimodal.c gshare.c hybrid.c yeh_patt.c btb.c parse.c stats.c -lm

clean:
	$(RM) sim_bp
