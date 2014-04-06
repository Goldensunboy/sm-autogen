# Andrew Wilder

PROGNAME = sm-autogen
CSRC = $(wildcard *.c)
ASRC = $(wildcard *.s)
OBJ = $(CSRC:%.c=%.o) $(ASRC:%.s=%.o)
AFLAGS = -felf64
CFLAGS = -std=gnu99 -O3
LFLAGS =
AS = nasm
CC = gcc
LD = gcc

all : $(OBJ)
	@echo "[LD] $(PROGNAME)"
	@$(LD) $(LFLAGS) $^ -o $(PROGNAME)

%.o : %.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s
	@echo "[AS] $<"
	@$(AS) $(AFLAGS) $< -o $@

clean :
	@echo "Removing all binaries"
	@rm -f $(PROGNAME) $(OBJ)
