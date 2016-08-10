#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "fingerprints.h"
#include "printgroup.h"
#include "parser.h"

//
PrintGroup *groupExists(PrintGroup* group_head,char* manufacturer)
{
		PrintGroup *tmpGroup=group_head;
	while(tmpGroup!=NULL)
	{
		if(strcmp(manufacturer,tmpGroup->manufacturer)==0)
		{
			return tmpGroup;
		}
		tmpGroup=tmpGroup->nextGroup;
	}
	return NULL;
}
//Checks if probe already exists in list and returns it's location if exists
Probe* probeExists(Probe *probe_head,char* line)
{
	Probe *tmpProbe=probe_head;
	while(tmpProbe!=NULL)
	{
		//printf("Comparing probe name: ");
		//printf("(%s?",line);
		//printf("%s)\n",tmpProbe->name);
		if(strcmp(tmpProbe->name,line)==0)
		{
			return tmpProbe;
		}
		tmpProbe=tmpProbe->nextProbe;
	}
	return NULL;
}
//
Test* testExists(Probe* probe,char* line)
{
	Test *tmpTest=probe->test_head;
	while(tmpTest!=NULL)
	{
		if(strcmp(tmpTest->name,line)==0)
		{
			return tmpTest;
		}
		tmpTest=tmpTest->nextTest;
	}
	return NULL;
}
//Adds the statistic information for the probe found in the line
Probe* addProbe(Probe *probe_head,char* line)
{
	char* testCopy=malloc(strlen(line)+1);
	strncpy(testCopy,line,strlen(line));
	testCopy[strlen(line)]='\0';
	strtok(testCopy,"(");
	//printf("Checking for probe existence!\n");
	Probe *newProbe=probeExists(probe_head,testCopy);
	if(newProbe==NULL)
	{
		//printf("Probe does not exist!\n");
		newProbe=malloc(sizeof(Probe));
		newProbe->name=malloc(strlen(testCopy)+1);
		strncpy(newProbe->name,testCopy,strlen(testCopy));
		newProbe->name[strlen(testCopy)]='\0';
		newProbe->nextProbe=NULL;
		newProbe->test_head=NULL;
		
		if(probe_head==NULL)
		{
			probe_head=newProbe;
		}
		else
		{
			Probe* probe_curr=probe_head;
			while(probe_curr->nextProbe!=NULL)
			{
				probe_curr=probe_curr->nextProbe;
			}
			probe_curr->nextProbe=newProbe;
		}
	}
	testCopy=strtok(NULL,")");
	//printf("Adding test for individual probe!\n");
	addTests(newProbe,testCopy);
	//printf("Done adding individual probe!\n");
	return probe_head;
}
//
void addTests(Probe *probe, char *testCopy)
{
	char* test=strtok(testCopy,"%");
	CharArray *cArray=NULL;
	while(test!=NULL)
	{
		cArray=parseTest(test);
		Test *tmpTest=testExists(probe,cArray->elements[0]);
		if(tmpTest==NULL)
		{
			tmpTest=malloc(sizeof(Test));
			//First element in array is test name, place in test
			tmpTest->name=malloc(strlen(cArray->elements[0])+1);
			strncpy(tmpTest->name,cArray->elements[0],strlen(cArray->elements[0]));
			tmpTest->name[strlen(cArray->elements[0])]='\0';
			tmpTest->totalOccurences=0;
			tmpTest->value_head=NULL;
			tmpTest->nextTest=NULL;
			
			//Create new test list
			if(probe->test_head==NULL)
			{
				probe->test_head=tmpTest;
			}
			//Add to end of existing test list
			else
			{
				Test *test_curr=probe->test_head;
				while(test_curr->nextTest!=NULL)
				{
					test_curr=test_curr->nextTest;
				}
				test_curr->nextTest=tmpTest;
			}
		}
		Value *value_curr=NULL;
		int i;
		if(tmpTest->value_head==NULL)
		{
			//Remaining values start at the second array element and continue to the end of the array
			tmpTest->value_head=newValue(cArray->elements[1]); //Place first value at head
			tmpTest->totalOccurences++;
			value_curr=tmpTest->value_head;
			
			for(i=2; i < cArray->numElements; i++)
			{
				value_curr->nextValue=newValue(cArray->elements[i]);
				tmpTest->totalOccurences++;
				value_curr=value_curr->nextValue;
			}
		}
		else
		{
			bool valueFound=false;
			Value *endValue=NULL;
			for(i=1; i < cArray->numElements; i++)
			{
				valueFound=false;
				value_curr=tmpTest->value_head;
				while(value_curr!=NULL)
				{	
					if(value_curr->nextValue==NULL)
					{
						endValue=value_curr;
					}
					
					if(strcmp(value_curr->name,cArray->elements[i])==0)
					{
						value_curr->numberOccurences++;
						tmpTest->totalOccurences++;
						valueFound=true;
						break;
					}
					value_curr=value_curr->nextValue;
				}
				
				if(valueFound==false)
				{
					endValue->nextValue=newValue(cArray->elements[i]);
					tmpTest->totalOccurences++;
				}
			}
		}
		//Free the memory from the first array and move to next test
		freeElements(cArray);
		//Move to next test and corresponding values
		test=strtok(NULL,"%");
	}
}
//
PrintGroup* addGroupProbe(PrintGroup *group_head,PrintClass *class_curr, char *probeLine)
{
	//printf("Finding group!\n");
	PrintGroup *group=findPrintGroup(group_head,class_curr->manufacturer);
	/*if(group==NULL)
	{
		printf("Group not found!\n");
	}
	else
	{
		printf("%s\n",group->manufacturer);
	}*/
	//printf("Finding product!\n");
	Product *product=findProduct(group,class_curr->product);
	/*if(product==NULL)
	{
		printf("Product not found!\n");
	}
	else
	{
		printf("%s\n",product->name);
	}*/
	//printf("Adding individual group probe!\n");
	product->probe_head=addProbe(product->probe_head,probeLine);
	//printf("Done adding individual group probe!\n");
	
	return group_head;
}
//
PrintGroup* findPrintGroup(PrintGroup *group_head,char *manufacturer)
{
	PrintGroup *group_curr=group_head;
	while(group_curr!=NULL)
	{
		if(strcmp(group_curr->manufacturer,manufacturer)==0)
		{
			return group_curr;
		}
		group_curr=group_curr->nextGroup;
	}
	return NULL;
}
//
Product* findProduct(PrintGroup *group,char *product)
{
	Product *prod_curr=group->product_head;
	while(prod_curr!=NULL)
	{
		if(strcmp(prod_curr->name,product)==0)
		{
			return prod_curr;
		}
		prod_curr=prod_curr->nextProduct;
	}
	return NULL;
}
//
PrintGroup* addProducts(PrintGroup *group_head,CharArray *classInfo)
{
	//Check if manufacturing group exists
	PrintGroup *tmpGroup=groupExists(group_head,classInfo->elements[0]);
	
	if(tmpGroup==NULL)
	{
		//Create new manufacturing group
		tmpGroup=malloc(sizeof(PrintGroup));
		tmpGroup->manufacturer=malloc(strlen(classInfo->elements[0])+1);
		strncpy(tmpGroup->manufacturer,classInfo->elements[0],strlen(classInfo->elements[0]));
		tmpGroup->manufacturer[strlen(classInfo->elements[0])]='\0';
		tmpGroup->product_head=NULL;
		tmpGroup->nextGroup=NULL;
		
		//Put at head of list
		if(group_head==NULL)
		{
			group_head=tmpGroup;
		}
		//Put at end of list
		else
		{
			//Iterate through group list to get to end and place new group at end
			PrintGroup *grpIterator=group_head;
			while(grpIterator->nextGroup!=NULL)
			{
				grpIterator=grpIterator->nextGroup;
			}
			grpIterator->nextGroup=tmpGroup;
		}
	}
	Product *product=NULL;
	if(tmpGroup->product_head==NULL)
	{	
		//printf("Creating new product head!\n");
		//Create new product within manufacturing group
		product=malloc(sizeof(Product));
		product->name=malloc(strlen(classInfo->elements[1])+1);
		strncpy(product->name,classInfo->elements[1],strlen(classInfo->elements[1]));
		product->name[strlen(classInfo->elements[1])]='\0';
		//Create new version within product in manufacturing group
		product->versions=NULL;
		product->versions=addElement(product->versions,classInfo->elements[2]);
		product->probe_head=NULL;
		product->nextProduct=NULL;
		tmpGroup->product_head=product;
		//printf("Created new product head! %s\n",tmpGroup->product_head->name);
	}
	else
	{
		Product *prdIterator=tmpGroup->product_head;
		Product *endOfProducts=NULL;
		while(prdIterator!=NULL)
		{
			//Compare product names to see if product already exists
			if(strcmp(prdIterator->name,classInfo->elements[1])==0)
			{
				product=prdIterator;
			}
			//Check the last value and break
			if(prdIterator->nextProduct==NULL)
			{
				//Used as pointer to end of list if product doesn't currently exist
				endOfProducts=prdIterator;
			}
			
			prdIterator=prdIterator->nextProduct;
		}
		if(product==NULL)
		{
			//printf("Creating new product in list!\n");
			//Create new product within manufacturing group
			product=malloc(sizeof(Product));
			product->name=malloc(strlen(classInfo->elements[1])+1);
			strncpy(product->name,classInfo->elements[1],strlen(classInfo->elements[1]));
			product->name[strlen(classInfo->elements[1])]='\0';
			//Create new version within product in manufacturing group
			product->versions=NULL;
			product->versions=addElement(product->versions,classInfo->elements[2]);
			product->probe_head=NULL;
			product->nextProduct=NULL;
			endOfProducts->nextProduct=product;
		}
		else
		{
			//printf("Updating existing product in list!\n");
			bool versionExists=false;
			int count;
			for(count=0; count < product->versions->numElements; count++)
			{
				if(strcmp(product->versions->elements[count],classInfo->elements[2])==0)
				{
					versionExists=true;
					break;
				}
			}
			//Add new version if version doesn't currently exist
			if(versionExists==false)
			{
				product->versions=addElement(product->versions,classInfo->elements[2]);
			}
		}
	}
	
	return group_head;
}