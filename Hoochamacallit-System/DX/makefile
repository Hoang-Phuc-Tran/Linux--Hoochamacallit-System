#  
#  FILE          : makefile
#  PROJECT       : A-03- Data Creator.
#  By	         : Caine Phung
#  FIRST VERSION : Caine Phung
#  DESCRIPTION   : This make file will compile the source files
#
./bin/DX : ./obj/DX.o 
	cc ./obj/DX.o   -o ../DR/bin/DX
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/DX.o : ./src/DX.c ../common/inc/common.h 
	cc -c ./src/DX.c -o ./obj/DX.o



#
# =======================================================
# Other targets
# =======================================================                     

all : ./bin/DX

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	


