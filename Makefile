
CC = g++ 
OPT = -Wall -O3
EXE = ./FM
BALFACTOR = 42
OFILES = FMbench1.net.result \
FMbench2.net.result \
FMbench3.net.result \
FMbench4.net.result \
FMbench5.net.result \
FMbench6.net.result \
FMbench7.net.result \
FMbench8.net.result \
FMbench9.net.result \
FMbenchB.net.result \
FMbench10.net.result 

.PHONY : notice all clean distclean 
	


all : notice  $(EXE)  $(OFILES)

notice :
	@echo "FMbench?.net must be put in this directory"
	@echo "result files are FMbench?.net.result"

FMpartition.o : FMpartition.cpp FMpartition.h
	$(CC) $(OPT) -c $< -o $@

$(EXE) : FMpartition.o FM.cpp
	$(CC) $(OPT) $^ -o $@

FMbench%.net.result : FMbench%.net $(EXE)
	$(EXE) $< $(BALFACTOR) > FMbench$*_$(BALFACTOR).log
	
clean: 
	rm -f $(EXE) FMpartition.o

distclean :
	rm -f $(EXE) FMpartition.o FMbench*.net.result FMbench*_*.log 

