#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "fingerprints.h"

//Allocates space for and copies fingerprint name line
void savePrintName(Fingerprint *print_curr,char *line)
{	
	print_curr->name=malloc(strlen(line)+1);
	strncpy(print_curr->name,line,strlen(line));
	print_curr->name[strlen(line)]='\0';
}

//Saves the CPE values of the fingerprint
void saveCPE(Fingerprint *print_curr,char *line)
{
	print_curr->cpe=addElement(print_curr->cpe,line);
}
//Saves the probe values of the fingerprint
void saveProbes(Fingerprint *print_curr,char *line)
{
	print_curr->probes=addElement(print_curr->probes,line);
}
//Saves the classes of the fingerprint
void saveClass(Fingerprint* print_curr,CharArray *classInfo,char *line)
{
	PrintClass *newClass=malloc(sizeof(PrintClass));
	newClass->classLine=malloc(strlen(line)+1);
	strncpy(newClass->classLine,line,strlen(line));
	newClass->classLine[strlen(line)]='\0';

	//Add manufacturer
	newClass->manufacturer=malloc(strlen(classInfo->elements[0])+1);
	strncpy(newClass->manufacturer,classInfo->elements[0],strlen(classInfo->elements[0]));
	newClass->manufacturer[strlen(classInfo->elements[0])]='\0';
	
	//Add product
	newClass->product=malloc(strlen(classInfo->elements[1])+1);
	strncpy(newClass->product,classInfo->elements[1],strlen(classInfo->elements[1]));
	newClass->product[strlen(classInfo->elements[1])]='\0';
	
	//Add version
	newClass->version=malloc(strlen(classInfo->elements[2])+1);
	strncpy(newClass->version,classInfo->elements[2],strlen(classInfo->elements[2]));
	newClass->version[strlen(classInfo->elements[2])]='\0';
	
	//Add type
	newClass->type=malloc(strlen(classInfo->elements[3])+1);
	strncpy(newClass->type,classInfo->elements[3],strlen(classInfo->elements[3]));
	newClass->type[strlen(classInfo->elements[3])]='\0';
	
	newClass->nextClass=NULL;
	
	//Start new class list
	if(print_curr->class_head==NULL)
	{
		print_curr->class_head=newClass;
	}
	//Add to existing class list
	else
	{
		PrintClass *tmpClass=print_curr->class_head;
		while(tmpClass->nextClass!=NULL) //Iterate to end of list
		{
			tmpClass=tmpClass->nextClass;
		}
		tmpClass->nextClass=newClass;
	}
}
//
PrintClass* printClassExists(Fingerprint* print_curr,char* manufacturer)
{
	if(print_curr->class_head==NULL)
	{
		return NULL;
	}
	else
	{
		PrintClass* tmpClass=print_curr->class_head;
		while(tmpClass!=NULL)
		{
			if(strcmp(tmpClass->manufacturer,manufacturer)==0)
			{
				return tmpClass;
			}
			tmpClass=tmpClass->nextClass;
		}
	}
	return NULL;
}
//
void freePrints(Fingerprint *print_head)
{
	int count;
	Fingerprint *print_curr=print_head;
	//Free memory for fingerprints
	while(print_curr!=NULL)
	{
		print_head=print_head->nextPrint;
		free(print_curr->name);
		//Free all cpe values
		if(print_curr->cpe!=NULL)
		{
			freeElements(print_curr->cpe);
		}
		//Free all class values
		if(print_curr->class_head!=NULL)
		{
			PrintClass *tmpClass=print_curr->class_head;
			while(tmpClass!=NULL)
			{
				free(tmpClass->manufacturer);
				free(tmpClass->product);
				free(tmpClass->classLine);
		
				tmpClass=tmpClass->nextClass;
			}
		}
		//Free all probes
		if(print_curr->probes!=NULL)
		{
			freeElements(print_curr->probes);
		}
		free(print_curr);
		print_curr=print_head;
	}
}
//Searches for a particular fingerprint within list
Fingerprint *findPrint(Fingerprint *print_head,char* probeName,char *testName,char *valueName)
{
	Fingerprint *tmpPrint=print_head;
	while(tmpPrint!=NULL)
	{
		int count;
		//Loop through all probes
		for(count=0; count < tmpPrint->probes->numElements; count++)
		{
			//Look for matching probe and test name
			if(strstr(tmpPrint->probes->elements[count],probeName) && strstr(tmpPrint->probes->elements[count],testName))
			{
				//Look for matching value in probe
				if(strstr(tmpPrint->probes->elements[count],valueName))
				{
					return tmpPrint;
				}
			}
		}
		tmpPrint=tmpPrint->nextPrint;
	}

	//Print not found, potential error
	return NULL;
}