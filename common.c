#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

//
Value* newValue(char* value)
{
	Value *newValue=malloc(sizeof(Value));
	newValue->name=malloc(strlen(value)+1);
	strncpy(newValue->name,value,strlen(value));
	newValue->name[strlen(value)]='\0';
	newValue->numberOccurences=1;
	newValue->nextValue=NULL;
	return newValue;
}
//
CharArray* addElement(CharArray *cArray,char *element)
{
	if(element==NULL)
	{
		printf("Empty element!\n");
		return cArray;
	}
	
	if(cArray==NULL)
	{	
		cArray=malloc(sizeof(cArray));
		cArray->numElements=1;
		cArray->elements=malloc(sizeof(cArray->elements));
		cArray->elements[0]=malloc(strlen(element)+1);
		strncpy(cArray->elements[0],element,strlen(element));
		cArray->elements[0][strlen(element)]='\0';
	}
	else
	{
		cArray->numElements++;
		int numElements=cArray->numElements;
		char** tmp=realloc(cArray->elements,numElements*sizeof(cArray->elements));
		cArray->elements=tmp;
		cArray->elements[numElements-1]=malloc(strlen(element)+1);
		strncpy(cArray->elements[numElements-1],element,strlen(element));
		cArray->elements[numElements-1][strlen(element)]='\0';
	}

	return cArray;
}

CharArray* addArray(CharArray *destArray,CharArray *copyArray)
{
	int i;
	for(i=0; i < copyArray->numElements; i++)
	{
		addElement(destArray,copyArray->elements[i]);
		free(copyArray->elements[i]);
	}
	copyArray->numElements=0;
	
	return destArray;
}

void freeElements(CharArray *cArray)
{
	int i;
	for(i=0; i < cArray->numElements; i++)
	{
		free(cArray->elements[i]);
	}
	free(cArray->elements);
}