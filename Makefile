build:
	gcc -o main main.c
run:
	./main
all : build run
	echo "Build and Run"
clean:
	rm main