CFLAGS := -std=c99 -D_XOPEN_SOURCE=600 -Wall -Wpedantic

hex2bin: hex2bin.c
	cc $(CFLAGS) -o $@ $<

decode_message: hex2bin
	cat message.txt | ( ./hex2bin; printf 'Result: %d\n' "$$?" >&2; echo ) | iconv -f ISO-8859-1 -t UTF-8

clean:
	rm -f hex2bin

.PHONY: clean message
