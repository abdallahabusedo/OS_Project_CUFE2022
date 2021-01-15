build:
	gcc process_generator.c -lm -o process_generator.out
	gcc clk.c -lm -o clk.out
	gcc scheduler.c -lm -o scheduler.out
	gcc process.c -lm -o process.out
	gcc test_generator.c -lm -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
