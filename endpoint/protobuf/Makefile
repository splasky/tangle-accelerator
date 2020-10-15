all: test-proto

LDFLAGS = -L/usr/local/lib -L./brotli -lbrotli -lprotobuf-c -lmbedcrypto -lmbedx509 -lmbedtls -lm
CFLAGS = -I/usr/local/include -I./brotli/c/include -Wall
SOURCES = test-protobuf.c obd2.pb-c.c b64.c/encode.c b64.c/decode.c b64.c/buffer.c

test-proto: 
	 gcc -g $(CFLAGS) -o test-protobuf $(SOURCES) $(LDFLAGS) 

clean:
	rm test-protobuf