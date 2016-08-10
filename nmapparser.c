/**************************************************************************************************
 * nmapparser.c
 *
 * Description: Parses NMap database to determine most common flags
 *
 * Author: Langston Graham
 * Created: 06/12/2013
 * Last Modified: 07/12/2013
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "fingerprints.h"
#include "printgroup.h"
#include "parser.h"


Fingerprint* print_head=NULL; //Pointer to the beginning of the fingerprint list
Fingerprint* print_curr=NULL; //Pointer to the fingerprint currently being accessed

Probe* probe_head=NULL; //Pointer to the beginning of the probe list

PrintGroup* group_head=NULL;

//Determines if parser is reading matchPointSection so it can skip it
bool matchPointSection=false;

int main()
{	
	FILE *db,*statsTxt,*groupStatsTxt; //The Nmap database file
	db=fopen("nmap-os-db","r"); //Open database file to be read
	statsTxt=fopen("statsTxt.txt","w"); //Open statsTxt file to be written to
	groupStatsTxt=fopen("groupStats.txt","w");//Open groupStatsTxt to be written to 
	if(db != NULL)
	{
		int i=0; //Keeps track of probe being accessed
		char line[800]; //Used to hold the current line being read from database file
		Fingerprint *newPrint=NULL;
		while( fgets(line, sizeof line, db) != NULL)
		{	
			//Skip comment and blank lines
			if(line[0]=='#' || line[0]=='\n')
			{				
				continue;
			}
			else
			{			
				//Sets bool value to skip over the matchpoints section
				if(strstr(line,"MatchPoints"))
				{
					matchPointSection=true;
					continue;
				}
				//Looks for the start of the fingerprint
				else if(strstr(line,"Fingerprint"))
				{
					matchPointSection=false;  //Signal end of matchpoints section
					
					newPrint=(Fingerprint*)malloc(sizeof(Fingerprint));
					
					//Start a new list of prints
					if(i==0)
					{
						print_head=newPrint;
						print_curr=newPrint;
					}
					//Continue linked list of prints
					else
					{
						print_curr->nextPrint=newPrint;
						print_curr=newPrint;
					}
					
					print_curr->nextPrint=NULL;
					print_curr->class_head=NULL;
					print_curr->cpe=NULL;
					print_curr->probes=NULL;

					i++;
					savePrintName(print_curr,line); //Save the name of the fingerprint
				}
				//Skips line if it is part of the matchpoint section
				else if(matchPointSection==true)
				{
					continue;
				}
				//Allocates memory and adds class line to fingerprints
				else if(strstr(line,"Class") && matchPointSection==false)
				{
					char *lineSubstring=malloc(strlen(line)+1);
					strncpy(lineSubstring,line,strlen(line));
					lineSubstring[strlen(line)]='\0';
					//Remove word class from line
					int firstSpace=strcspn(lineSubstring," ");
					lineSubstring += firstSpace+1;
					//Split line into parts divided by |
					CharArray *classInfo=parse(lineSubstring);
					
					saveClass(print_curr,classInfo,line);
					PrintGroup *tmpGroup=addProducts(group_head,classInfo);
					if(group_head==NULL)
					{
						group_head=tmpGroup;
					}
					free(lineSubstring);
				}
				//Allocates memory and adds cpe line to fingerprints
				else if(strstr(line,"CPE") && matchPointSection==false)
				{
					saveCPE(print_curr,line);
				}
				//Any remaining lines should be probe lines
				else if(matchPointSection==false)
				{
					saveProbes(print_curr,line);
					Probe *tmpProbe=addProbe(probe_head,line);
					probe_head=tmpProbe;
					int i;
					PrintClass *class_curr=print_curr->class_head;
					//printf("Adding group probes!\n");
					while(class_curr!=NULL)
					{
						//printf("Calling add group function!\n");
						group_head=addGroupProbe(group_head,class_curr,line);
						class_curr=class_curr->nextClass;
					}
					//printf("Done adding group probes!\n");
				}
			}
		}
		//Print results of parsing
		Probe *probe_curr=probe_head;
		while(probe_curr!=NULL)
		{
			//Print the name of the probe to the file
			fprintf(statsTxt,"%s\n",probe_curr->name);
			
			Test *test_curr=probe_curr->test_head;
			Value *value_curr;
			float percentage;
			while(test_curr!=NULL)
			{
				//Print the name of the test
				fprintf(statsTxt,"\t-%s\n",test_curr->name);
				value_curr=test_curr->value_head;
				float totOccur=test_curr->totalOccurences;
				while(value_curr!=NULL)
				{
					float numOccur=value_curr->numberOccurences;
					percentage=(numOccur/totOccur)*100;//Get the percentage of occurences
					//Print the statistics for the given value, percentage and number of occurences
					fprintf(statsTxt,"\t%-20s (%3.2f%% %5.0f)",value_curr->name,percentage, numOccur);
					//Find any fingerprints with tests values that appear only once
					if(numOccur==1)
					{
						Fingerprint *uniquePrint=findPrint(print_head,probe_curr->name,test_curr->name,value_curr->name);
						if(uniquePrint==NULL)
						{
							fprintf(statsTxt,"(Unique print not found!)");
						}
						else
						{
							//Cut out any newlines already in the formatting of the line
							if(strstr(uniquePrint->name,"\n"))
							{
								strtok(uniquePrint->name,"\n");
							}
							fprintf(statsTxt,"(%s)",uniquePrint->name);
						}
					}
					fprintf(statsTxt,"\n");
					
					//Free memory and move to next value in list
					test_curr->value_head=test_curr->value_head->nextValue;
					free(value_curr->name);
					free(value_curr);
					value_curr=test_curr->value_head;
				}
				fprintf(statsTxt,"\n");
				//Free memory and move to next test in list
				probe_curr->test_head=probe_curr->test_head->nextTest;
				free(test_curr->name);
				free(test_curr->value_head);
				free(test_curr);
				test_curr=test_curr->nextTest;
			}
			//Free memory and move to next probe in list
			probe_head=probe_head->nextProbe;
			free(probe_curr->name);
			free(probe_curr->test_head);
			free(probe_curr);
			probe_curr=probe_head;
		}
		int count;
		//Print gathered group stats
		PrintGroup *groupCurr=group_head;
		while(groupCurr!=NULL)
		{
			fprintf(groupStatsTxt,"%s\n",groupCurr->manufacturer);
			Product *product_curr=groupCurr->product_head;
			while(product_curr!=NULL)
			{
				fprintf(groupStatsTxt,"\t%s\n",product_curr->name);
				fprintf(groupStatsTxt,"\t\t-Versions: ");
				for(count=0; count < product_curr->versions->numElements; count++)
				{
					fprintf(groupStatsTxt,"%s|",product_curr->versions->elements[count]);
				}
				fprintf(groupStatsTxt,"\n");
				Probe *probe_curr=product_curr->probe_head;
				while(probe_curr!=NULL)
				{
					fprintf(groupStatsTxt,"\t\t%s:\n",probe_curr->name);
					Test *test_curr=probe_curr->test_head;
					while(test_curr!=NULL)
					{
						fprintf(groupStatsTxt,"\t\t\t%s:",test_curr->name);
						Value *value_curr=test_curr->value_head;
						while(value_curr!=NULL)
						{
							fprintf(groupStatsTxt,"%s,",value_curr->name);
							value_curr=value_curr->nextValue;
						}
						fprintf(groupStatsTxt,"\n");
						test_curr=test_curr->nextTest;
					}
					
					probe_curr=probe_curr->nextProbe;
				}
				
				product_curr=product_curr->nextProduct;
			}
			groupCurr=groupCurr->nextGroup;
		}
		
		freePrints(print_head);
		
		//Close the files
		fclose(db);
		fclose(statsTxt);
		fclose(groupStatsTxt);
	}
	else
	{
		printf("Could not locate file!");
	}
	
	return 0;
}