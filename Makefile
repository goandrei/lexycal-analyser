output: dfa.o tokenizer.o main.o 
	g++ dfa.o tokenizer.o main.o 

main.o : main.cpp
	g++ -c main.cpp

tokenizer.o : tokenizer.cpp
	g++ -c tokenizer.cpp

dfa.o : dfa.cpp
	g++ -c dfa.cpp

clean:
	rm *.o