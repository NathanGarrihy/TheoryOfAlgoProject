EXECS=args sha512

ShaFiveTwelve: sha512.c
	cc -o sha512 sha512.c
	echo "Action finished executing!"

test: tests.sh
	./tests.sh

clean:
	rm -f $(EXECS)