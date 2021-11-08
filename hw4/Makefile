CFLAGS = -O3 -Wall -std=c++11
# CFLAGS = -g -Wall

seatAss: File.o Proof.o Solver.o seater.o seatAss.o
	g++ -o $@ $(CFLAGS) File.o Proof.o Solver.o seater.o seatAss.o

File.o: File.cpp
	g++ -c $(CFLAGS) File.cpp

Proof.o: Proof.cpp
	g++ -c $(CFLAGS) Proof.cpp

Solve.o: Solver.cpp
	g++ -c $(CFLAGS) Solver.cpp

seater.o: seater.cpp
	g++ -c $(CFLAGS) seater.cpp

seatAss.o: seatAss.cpp
	g++ -c $(CFLAGS) seatAss.cpp

clean:
	rm -f *.o seatAss core* tags
