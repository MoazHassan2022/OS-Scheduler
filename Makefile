build:
	gcc process_generator.c -o process_generator.out -std=c99 -lm
	gcc clk.c -o clk.out -std=c99 -lm
	gcc scheduler.c -o scheduler.out -std=c99 -lm
	gcc process.c -o process.out -std=c99 -lm
	gcc test_generator.c -o test_generator.out -std=c99 -lm

clean:
	rm -f *.out  processes.txt

run:
	./process_generator.out


all: clean build run
