HSU_FILE = "hsu.c"
HSU_OUT="hsu"

.PHONY: all clean*

all:
	gcc -o $(HSU_OUT) $(HSU_FILE)
	sudo chown root:wheel $(HSU_OUT)
	sudo chmod u+s $(HSU_OUT)
