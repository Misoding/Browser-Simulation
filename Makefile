build:
	gcc ./main.c -o tema1 -Wall -Wextra -g

clean:
	rm -f ./tema1*

run:
	./tema1

test:
	./checker.sh