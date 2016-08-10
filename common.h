#ifndef COMMON_H
#define COMMON_H

typedef enum {false,true} bool;

typedef struct
{
	char ** elements;
	int numElements;
} CharArray;

//Holds a test value
typedef struct
{
	char *name;
	int numberOccurences;
	void *nextValue;
} Value;

Value* newValue(char* value);
CharArray* addElement(CharArray *cArray,char *element);
CharArray* addArray(CharArray *destArray,CharArray *copyArray);
void freeElements(CharArray *cArray);

#endif /* COMMON_H */