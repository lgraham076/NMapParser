#ifndef PRINTGROUP_H
#define PRINTGROUP_H

//Holds all the possible values for a given test
typedef struct
{
	char *name;
	Value *value_head;
	int totalOccurences;
	void *nextTest;
} Test;
//Holds all the possible tests for a given probe
typedef struct
{
	char *name;
	Test *test_head;
	CharArray *matchingPrints;//To be implemented, keeps tracks of the fingerprint names to make searching for unique prints easier
	void *nextProbe;
} Probe;

//Holds detailed information for a product
typedef struct
{
	char *name;
	CharArray *versions;
	Probe *probe_head;
	void *nextProduct;
} Product;
//Holds groups of manufactures with their products
typedef struct
{
	char *manufacturer;
	Product *product_head;
	void *nextGroup;
} PrintGroup;

PrintGroup *groupExists(PrintGroup* group_head,char* manufacturer); //Checks PrintGroup already exists to prevent duplicates
Probe* probeExists(Probe *probe_head,char* line); //
Test* testExists(Probe* probe,char* line); //
Probe* addProbe(Probe *probe_head,char* line); //
void addTests(Probe *probe,char *testCopy); //
PrintGroup* addGroupProbe(PrintGroup *group_head,PrintClass *class_curr,char *probeLine);//
PrintGroup* findPrintGroup(PrintGroup *group_head,char *manufacturer); //
Product* findProduct(PrintGroup *group,char *product); //
PrintGroup* addProducts(PrintGroup *group_head,CharArray *classInfo); //Adds group and product information found in class line

#endif /* PRINTGROUP_H */