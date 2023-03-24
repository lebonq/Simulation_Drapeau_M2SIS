
CC  = gcc
STD = -std=c17

DEBUG = 1
ifeq ($(DEBUG),1)
  CFLAGS  = -g -Wpointer-arith -Wall
else
  CFLAGS  = -O2
endif

LFLAGS = $(libG3X)
PFLAGS = $(incG3X) -I./include

src = src/

all : drapeau_lebonq

# règle générique de création de xxx.o à partir de src/xxx.c
%.o : $(src)%.c
	@echo "module $@"
	@$(CC) $(STD) $(CFLAGS) $(PFLAGS) -c $< -o $@
	@echo "------------------------"

# règle générique de création de l'executable xxx à partir de src/xxx.c (1 seul module)
% : %.o
	@echo "assemblage [$^]->$@"
	@$(CC) $^ $(LFLAGS) -o $@
	@echo "------------------------"

.PHONY : clean cleanall ?

# informations sur les paramètres de compilation
? :
	@echo "---------informations de compilation----------"
	@echo "  processeur     : $(PROCBIT)"
	@echo "  compilateur    : $(CC)"
	@echo "  standard       : $(STD)"
	@echo "  CFLAGS         : $(CFLAGS)"
	@echo "  LFLAGS         : $(LFLAGS)"
	@echo "  PFLAGS         : $(PFLAGS)"

clean :
	@rm -f *.o

cleanall :
	@rm -f *.o g3x_*

