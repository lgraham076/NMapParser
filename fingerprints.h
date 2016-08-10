#ifndef FINGERPRINTS_H
#define FINGERPRINTS_H

//Holds all the information for a class
typedef struct
{
	char *classLine;
	char *manufacturer;
	char *product;
	char *version;
	char *type;
	void *nextClass;
} PrintClass;
//Holds all the gathered fingerprints
typedef struct
{
	char *name;
	PrintClass *class_head;
	CharArray *cpe;
	CharArray *probes;
	void *nextPrint;
} Fingerprint;

void savePrintName(Fingerprint *print_curr,char *line);//Allocates space for and copies fingerprint name line
void saveCPE(Fingerprint *print_curr,char *line); //Allocates space for and copies cpe line into fingerprints
void saveProbes(Fingerprint *print_curr,char *line); //Allocates space for and copies probe line into fingerprints
void saveClass(Fingerprint* print_curr,CharArray *classInfo,char *line); //Allocates space for and copies class line into fingerprints
PrintClass* printClassExists(Fingerprint* print_curr,char* manufacturer); //
void freePrints(Fingerprint *print_head); //Frees allocated memory for all fingerprints in list
//Searches for a particular fingerprint within list
Fingerprint* findPrint(Fingerprint *print_head,char* probeName,char *testName,char *valueName); 

#endif /* FINGERPRINTS_H */
