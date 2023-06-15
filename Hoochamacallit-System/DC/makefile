#  
#  FILE          : makefile
#  PROJECT       : A-03- Data Creator.
#  By	         : Caine Phung
#  FIRST VERSION : Caine Phung
#  DESCRIPTION   : This make file will compile the source files
#
./bin/DC : ./obj/DC.o 
	cc ./obj/DC.o   -o ./bin/DC
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/DC.o : ./src/DC.c ../common/inc/common.h
	cc -c ./src/DC.c -o ./obj/DC.o



#
# =======================================================
# Other targets
# =======================================================                     

all : ./bin/DC

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	


