debug:
	g++ -DDEBUG main.cpp -o sorter

gif:
	g++ -DGIF main.cpp -o sorter

default:
	g++ main.cpp -o sorter