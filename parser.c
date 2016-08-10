#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

CharArray* parse(char *valueLine)
{
	CharArray *cArray=NULL;
	char *tmpLine=valueLine;
	char *newLine;
	int i=strcspn(tmpLine,"|");
	int count=0;
	while(count < 1000)
	{
	
		newLine=malloc(i+1);
		memset(newLine,0,i+1);
		strncpy(newLine,tmpLine,i);
		if(i!=0)
		{
			count++;
			cArray=addElement(cArray,newLine);
			
			if(i==strlen(tmpLine))
			{
				break;
			}
		}
		tmpLine+= i+1;
		//Replace empty pipes with N/A
		if(tmpLine[0]=='|')
		{
			cArray=addElement(cArray,"N/A");
		}
		i=strcspn(tmpLine,"|");
	}
	//charValues=count;
	return cArray;
}
//
CharArray* parseTest(char *line)
{
	char test[100];
	char value[1000];
	//Split test line on = with test name on left and the value on the right
	int i=strcspn(line,"=");

	memcpy(test,&line[0],i);
	memcpy(value,&line[i+1],strlen(line)-i);
	//Add null characters to the end to indicate end of string
	test[i]='\0';
	value[strlen(value)+1]='\0';
	
	CharArray *parsedTest=NULL;
	parsedTest=addElement(parsedTest,test); //First element of the array is the test name
	//printf("%s test name added to array!\n",parsedTest->elements[0]);
	
	//The rest of the elements are the test values
	CharArray *values=NULL;
	//If | is present in value multiple values must be parsed out
	if(strstr(value,"|"))
	{
		//printf("Adding multiple values!\n");
		values=parse(value);
		parsedTest=addArray(parsedTest,values);
	}
	else
	{
		//printf("Adding single value!\n");
		parsedTest=addElement(parsedTest,value);
	}
	/*for(i=0; i < parsedTest->numElements; i++)
	{
		printf("%s\n",parsedTest->elements[i]);
	}*/
	
	//printf("Returning parsed test!\n");
	return parsedTest;
}