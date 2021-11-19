flags=-Wno-deprecated -g -ggdb
comp=g++
prog=DRP+AE

#El problema

## SJC-DRP

#config=Instancias/SJC-DRP/SJC324-3.txt
#config=Instancias/SJC-DRP/SJC402-3.txt
#config=Instancias/SJC-DRP/SJC500-3.txt
#config=Instancias/SJC-DRP/SJC500-7.txt
#config=Instancias/SJC-DRP/SJC708-5.txt
#config=Instancias/SJC-DRP/SJC708-7.txt
#config=Instancias/SJC-DRP/SJC708-11.txt

## SJC-DRP-AEDs
config=Instancias/SJC-DRP-AEDs/SJC324-3.txt
#config=Instancias/SJC-DRP-AEDs/SJC402-3.txt
#config=Instancias/SJC-DRP-AEDs/SJC500-3.txt
#config=Instancias/SJC-DRP-AEDs/SJC500-7.txt
#config=Instancias/SJC-DRP-AEDs/SJC708-5.txt
#config=Instancias/SJC-DRP-AEDs/SJC708-7.txt
#config=Instancias/SJC-DRP-AEDs/SJC708-11.txt


semilla=123
debug=0
iterations=5000

costo_nuevo=1
costo_reubicar=0.2

population_size=200
child_size=50

prob_mutation=1 # 1/ Number of OHCA
prob_mutation_original_AED=10 # 10/100


comp:$(prog)

run:$(prog)
	/usr/bin/time -p ./$(prog) $(config) $(semilla) $(debug) $(iterations) $(costo_nuevo) $(costo_reubicar) $(population_size) $(child_size) $(prob_mutation) $(prob_mutation_original_AED)

$(prog): main.o
	$(comp) main.o -o $(prog) $(flags)

main.o: main.cpp includes.h
	$(comp) -c main.cpp $(flags)

val:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./$(prog) 

clean:
	rm -f *~
	rm -f *.o
	rm -f $(prog)
	rm -f core*

