#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>


typedef struct date{
	int day;
	int month;
	int year;
}Date;

typedef struct list{ //list with the countries
	int num_work;
	char* country;
	struct list* next;
}List;

typedef struct range_list{
	int value;
	int type;
	struct range_list *next;
}Range_list;

typedef struct record{
	char* recordID;
	char* situation;
	char* patientFirstName;
	char* patientLastName;
	char* diseaseID;
	char* country;
	Date* entryDate;
	Date* exitDate;
	int  age;
	struct record* nextRecord;
}Record;


typedef struct avlNode{
	int ht;
	Record* ptr;
	struct avlNode* right;
	struct avlNode* left;
}AvlNode;

typedef struct bucketEntry{
	char* name;
	AvlNode *avlNode;
	struct bucketEntry* nextBucketEntry;
}BucketEntry;

typedef struct bucket{
	BucketEntry* bucketEntryList;
	int entries;
	struct bucket* nextBucket;
}Bucket;

typedef struct hashNode{
	Bucket* bucketList;
}HashNode;
