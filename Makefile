nr:
	g++ -D NIGHT_MODE -D RANDOM_RULES main.cpp -o cellular

nd:
	g++ -D NIGHT_MODE main.cpp -o cellular

dr:
	g++ -D RANDOM_RULES main.cpp -o cellular

dd:
	g++ main.cpp -o cellular