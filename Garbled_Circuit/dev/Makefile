CC=gcc
CFLAGS:= -O3 -Wpedantic -g

HASH_SRC:=lib/hash/hash.c
HASH_INCLUDE:=-I lib/hash -lcrypto

SRC:=src
LIB:=$(HASH_INCLUDE)

MAIN_MPC:=src/main.c
MAIN_BENCHMARK_TIME:=test/main_perf.c
MPC_OBJS:=auxiliary_functions.o cmp_steps.o gate_functions.o randombytes.o oblivious_transfer.o paillier.o twisted_edwards_curves.o
LIB_OBJS:=hash.o

BUILD:=bin/build
BIN:=bin

folders:
	@echo -e "\n### Creating build folders\n"
	mkdir -p $(BUILD)

hash.o: folders
	 @echo -e "\n### Compiling $@ (wrapper around openssl SHA512 implementation)\n"
	 $(CC) $(CFLAGS) -c $(HASH_SRC) $(HASH_INCLUDE) -o $(BUILD)/$@

%.o: $(SRC)/%.c | folders
	@echo -e "\n### Compiling $@\n"
	$(CC) $(CFLAGS) -c $< $(LIB) -o $(BUILD)/$@

comparison: $(MPC_OBJS) $(LIB_OBJS) | folders
	@echo -e "\n### Compiling the comparison\n"
	$(CC) $(CFLAGS) $(MAIN_MPC) $(addprefix $(BUILD)/, $^) $(INCLUDE) $(LIB) -o $(BIN)/$@ -lgmp

bench-time: $(MPC_OBJS) $(LIB_OBJS) | folders
	@echo -e "\n### Compiling the comparison\n"
	$(CC) $(CFLAGS) $(MAIN_BENCHMARK_TIME) $(addprefix $(BUILD)/, $^) $(INCLUDE) $(LIB) -o $(BIN)/$@ -lgmp

clean:
	rm -f vgcore.*
	rm -rf ./bin
