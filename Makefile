HEADERS = common.h parser.h printgroup.h fingerprints.h nmapparser.h
OBJECTS = common.o parser.o printgroup.o fingerprints.o nmapparser.o

all : NMapParser

%.o: %.c $(HEADERS)
	gcc -c -Wall $< -o $@

NMapParser : $(OBJECTS)
	gcc -Wall $(OBJECTS) -o $@