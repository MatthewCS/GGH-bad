CC=g++
FLAGS=-g -O2 -std=c++11 -pthread -march=native

all: encrypt decrypt

encrypt: encrypt.cpp
	$(CC) $(FLAGS) encrypt.cpp -o encrypt -lntl -lgmp -lm

decrypt: decrypt.cpp
	$(CC) $(FLAGS) decrypt.cpp -o decrypt -lntl -lgmp -lm
