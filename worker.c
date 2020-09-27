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
#include "structs.h"

#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)
int Flag=0; //global var
#define p 131

volatile sig_atomic_t childEnd = -1;
volatile sig_atomic_t orderds_end = 0;
       
void sig_handler1(int sig, siginfo_t *si, void *unused){
	// Handler for the signals that the process will receive
	if(sig == SIGINT || sig == SIGQUIT){
		printf("received\n");
		orderds_end = -1;
	}
	if(sig == SIGCHLD){
		childEnd= si->si_pid;
	}
} 

typedef struct work_diseases{
	char* dis;
	int first_range;
	int sec_range;
	int third_range;
	int fourth_range;
	struct work_diseases* next_disease;
}Work_diseases;

typedef struct work_dates{
	char* filename;
	struct work_diseases* wds;	
	struct work_dates *next_date;
}Work_dates;

typedef struct work_countries{
	char* country;
	struct work_dates* wd;
	struct work_countries* next_country;
}Work_countries;

Work_countries* insertC(Work_countries** head, Work_countries *current_record){
	// Insert a new record in the end of the list
	Work_countries* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_country = current_record;
	return current_record;
}

Work_dates* insertD(Work_dates** head, Work_dates *current_record){
	// Insert a new record in the end of the list
	Work_dates* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_date= current_record;
	return current_record;
}

Work_diseases* insertDs(Work_diseases** head, Work_diseases *current_record){
	// Insert a new record in the end of the list
	Work_diseases* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_disease= current_record;
	return current_record;
}

int TraverseList(char* diseaseID, Work_diseases* head){
	Work_diseases *ptr = head;
	while(ptr!=NULL){
		if (strcmp(ptr->dis, diseaseID) == 0){
			return 1;
		}
		ptr = ptr->next_disease;
	}
	return 0;
}

typedef struct file{
	char* filename;
	Date* fileDate;
	struct file* next_file;
}File;


void deleteList( Record** head) 
{ 
   Record* current = *head; 
   Record* next; 
   while (current != NULL)  
   { 
       next = current->nextRecord; 
       free(current->recordID);
       free(current->situation);
       free(current->patientFirstName);
       free(current->patientLastName);
       free(current->diseaseID);
       free(current->country);
       free(current->entryDate);
       free(current->exitDate);
       free(current); 
       current = next; 
   }    
   *head = NULL; 
}


char* LastcharDel(char* name)
{
    int i = 0;
    while(name[i] != '\0')
    {
        i++;
    }
    name[i-1] = '\0';
    return name;
}


void reverse(char s[]){
     int i, j;
     char c;
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[]){
     int i, sign;
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

int compareDates(Date* entry1, Date *entry2){
	if(entry2->year == 0)
		return 0;
	if(entry1->year == entry2->year)
		if(entry1->month == entry2->month)
			if(entry1->day == entry2->day)
				return 2;

	if (entry1->year < entry2->year)
		return 1;
	else if (entry1->year > entry2->year)
		return 0;
	else{
		if (entry1->month < entry2->month)
			return 1;
		else if (entry1->month > entry2->month)
			return 0;
		else{
			if (entry1->day < entry2->day)
				return 1;
			else if ( entry1-> day > entry2->day)
				return 0;
			else 
				return 0;
		}
	}
}

void deleteTree(AvlNode* node)  
{ 
    if (node == NULL) return; 
    /* first delete both subtrees */
    deleteTree(node->left); 
    deleteTree(node->right); 
    /* then delete the node */

	//free(node->name);
	free(node);
}  

void exitHash(HashNode* diseaseHash, HashNode* counrtyHash){
	int i;
	for(i=0; i<5; i++){
		Bucket *ptr = diseaseHash[i].bucketList;
		Bucket *nextB;
		while(ptr!= NULL){
			nextB = ptr->nextBucket;
			BucketEntry* entry = ptr->bucketEntryList;
			BucketEntry* next ;
			while(entry!=NULL){
				next = entry->nextBucketEntry;
				free(entry->name);
				deleteTree(entry->avlNode);
				free(entry);
				entry = next;
			}
			free(ptr);
			ptr = nextB;
		}
	}
	for(i=0; i<5; i++){
		Bucket *ptr = counrtyHash[i].bucketList;
		Bucket *nextB;
		while(ptr!= NULL){
			nextB = ptr->nextBucket;
			BucketEntry* entry = ptr->bucketEntryList;
			BucketEntry* next ;
			while(entry!=NULL){
				next = entry->nextBucketEntry;
				free(entry->name);
				deleteTree(entry->avlNode);
				free(entry);
				entry = next;
			}
			free(ptr);
			ptr = nextB;
		}
	}
}	

Record* createRecord(char* recordID ,char* situation ,char*patientFirstName, char* patientLastName, char* diseaseID, char* country, Date* entryDate, Date* exitDate, int age){
	Record* current_record;

	current_record = malloc(sizeof(Record));	
	current_record->recordID= malloc(sizeof(char)*strlen(recordID)+1);
   	strcpy(current_record->recordID,recordID);
   	current_record->situation = malloc(sizeof(char)*strlen(situation)+1);
   	strcpy(current_record->situation,situation);
   	current_record->patientFirstName= malloc(sizeof(char)*strlen(patientFirstName)+1);
   	strcpy(current_record->patientFirstName,patientFirstName);
   	current_record->patientLastName= malloc(sizeof(char)*strlen(patientLastName)+1);
   	strcpy(current_record->patientLastName,patientLastName);
   	current_record->diseaseID= malloc(sizeof(char)*strlen(diseaseID)+1);
   	strcpy(current_record->diseaseID,diseaseID);
   	current_record->country= malloc(sizeof(char)*strlen(country)+1);
   	strcpy(current_record->country,country);

   	current_record->entryDate= malloc(sizeof(Date));
   	current_record->entryDate->day = entryDate->day;
   	current_record->entryDate->month = entryDate->month;
   	current_record->entryDate->year = entryDate->year;

   	current_record->exitDate= malloc(sizeof(Date));
   	current_record->exitDate->day = exitDate->day;
   	current_record->exitDate->month = exitDate->month;
   	current_record->exitDate->year = exitDate->year;

   	current_record->age = age;
   	current_record->nextRecord = NULL;

  	return current_record;
}

Record* insertRecord(Record** head, Record *current_record){
	// Insert a new record in the end of the list
	Record* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->nextRecord= current_record;
	return current_record;
}

void printRecordList(Record *head){
	Record *current_record = head;
	while(current_record != NULL){
		printf("%s ", current_record->recordID);
		printf("%s ", current_record->situation);
		printf("%s ",current_record->patientFirstName );
		printf("%s ",current_record->patientLastName );
		printf("%s ",current_record->diseaseID );
		printf("%s ",current_record->country );

		printf("%d-",current_record->entryDate->day);
		printf("%d-",current_record->entryDate->month);
		printf("%d ",current_record->entryDate->year);

		printf("%d-",current_record->exitDate->day);
		printf("%d-",current_record->exitDate->month);
		printf("%d ",current_record->exitDate->year);
		printf("%d ",current_record->age);
		printf("\n");
		current_record = current_record->nextRecord;
	}
	return;
}

int height(AvlNode *T){
	int lh,rh;
	if(T==NULL)
		return(0);
	
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
		
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
	
	if(lh>rh)
		return(lh);
	
	return(rh);
}

AvlNode * rotateright(AvlNode *x){
	AvlNode *y;
	y=x->left;
	x->left=y->right;
	y->right=x;
	x->ht=height(x);
	y->ht=height(y);
	return(y);
}
 
AvlNode * rotateleft(AvlNode *x){
	AvlNode *y;
	y=x->right;
	x->right=y->left;
	y->left=x;
	x->ht=height(x);
	y->ht=height(y);
	return(y);
}
 
AvlNode * RR(AvlNode *T){
	T=rotateleft(T);
	return(T);
}
 
AvlNode * LL(AvlNode *T){
	T=rotateright(T);
	return(T);
}
 
AvlNode * LR(AvlNode *T){
	T->left=rotateleft(T->left);
	T=rotateright(T);
	
	return(T);
}
 
AvlNode * RL(AvlNode *T){
	T->right=rotateright(T->right);
	T=rotateleft(T);
	return(T);
}
 
int BF(AvlNode *T){
	int lh,rh;
	if(T==NULL)
		return(0);
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
	return(lh-rh);
}

AvlNode* insertAvl(AvlNode *T,Record *rec){
//	if(searchAvl(T,rec) == 0){
		if(T==NULL){
			T=(AvlNode*)malloc(sizeof(AvlNode));
			//T->ptr = malloc(sizeof(Record));
			T->ptr = rec;
			T->left=NULL;
			T->right=NULL;
		}
		else
			if(compareDates(rec->entryDate,T->ptr->entryDate) != 1)		// insert in right subtree
			{
				T->right=insertAvl(T->right,rec);
				if(BF(T)==-2)
					if (compareDates(rec->entryDate,T->right->ptr->entryDate) != 1)
						T=RR(T);
					else
						T=RL(T);
			}
			else
				if(compareDates(rec->entryDate,T->ptr->entryDate) == 1)
				{
					T->left=insertAvl(T->left,rec);
					if(BF(T)==2)
						if(compareDates(rec->entryDate, T->left->ptr->entryDate) == 1)
							T=LL(T);
						else
							T=LR(T);
				}
			
			T->ht=height(T);
			
			return(T);
}



int searchEntries(BucketEntry *head, char* name){
	BucketEntry *ptr2 = head;
	while(ptr2!= NULL){
		if( strcmp(ptr2->name,name) == 0)
			return 1;
		ptr2 = ptr2->nextBucketEntry;
	}
	return 0;
}

int  searchHash(Bucket* head,char* name){
	Bucket *ptr = head;
	int key;
	//int counter = 0;
	while(ptr != NULL){
		key = searchEntries(ptr->bucketEntryList, name);
		if (key == 1)
			return 1;
		ptr = ptr->nextBucket;
	}
	return 0; 
}

Bucket* insertBucket(Bucket **bucketList, int max_entries, char*name, Record** head){
	int key = searchHash(*bucketList,name);
	//printf("kanw isnert thn arrwstia %s\n", name);
	if (key == 0){
		//Record* ptr = *head;
		if ((*bucketList) == NULL){
			(*bucketList) = malloc(sizeof(Bucket));
			(*bucketList)->entries = 1;
			(*bucketList)->nextBucket = NULL;

			
			(*bucketList)->bucketEntryList = malloc(sizeof(BucketEntry));
			(*bucketList)->bucketEntryList->name = malloc(sizeof(char)*strlen(name)+1);
			strcpy((*bucketList)->bucketEntryList->name,name);
			//(*bucketList)->bucketEntryList->name = name;
			(*bucketList)->bucketEntryList->avlNode = NULL;
			(*bucketList)->bucketEntryList->nextBucketEntry = NULL;
	
			while((*head)!=NULL){
				if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
						(*bucketList)->bucketEntryList->avlNode = insertAvl((*bucketList)->bucketEntryList->avlNode,(*head));
				}
				(*head) = (*head)->nextRecord;
			}
			(*bucketList)->bucketEntryList->nextBucketEntry = NULL;
			return *bucketList;
		}
		Bucket* currentBucket = *bucketList;
		while(currentBucket->entries ==  max_entries){
			//Record *ptr2 = *head;
			if (currentBucket->nextBucket == NULL){
				currentBucket->nextBucket = malloc(sizeof(Bucket));
				currentBucket->nextBucket->entries = 1;
				currentBucket->nextBucket->nextBucket = NULL;

				currentBucket->nextBucket->bucketEntryList = malloc(sizeof(BucketEntry));
				currentBucket->nextBucket->nextBucket = NULL; //here
				currentBucket->nextBucket->bucketEntryList->name = malloc(sizeof(char)*strlen(name)+1);
				strcpy(currentBucket->nextBucket->bucketEntryList->name,name);
				//currentBucket->nextBucket->bucketEntryList->name = name;
				currentBucket->nextBucket->bucketEntryList->avlNode = NULL;
				currentBucket->nextBucket->bucketEntryList->nextBucketEntry = NULL; //here

				while((*head)!=NULL){
					if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
						//Record *rec = head;
						//if(searchAvl(currentBucket->nextBucket->bucketEntryList->avlNode,rec) == 0)
							currentBucket->nextBucket->bucketEntryList->avlNode = insertAvl(currentBucket->nextBucket->bucketEntryList->avlNode,(*head));
							//return *bucketList;
					}
					(*head) = (*head)->nextRecord;
				}
				currentBucket->nextBucket->bucketEntryList->nextBucketEntry = NULL; 
				return *bucketList;
			}
			currentBucket = currentBucket->nextBucket;
		}
		BucketEntry *entry;
		entry = malloc(sizeof(BucketEntry));
		entry->name = malloc(sizeof(char)*strlen(name)+1);
		strcpy(entry->name,name);
		//entry->name = name;
		entry->nextBucketEntry = NULL; //here
		entry->avlNode = NULL; 
		entry->nextBucketEntry = currentBucket->bucketEntryList;

		currentBucket->bucketEntryList = entry;
		//Record *ptr3 = *head;

		while((*head)!=NULL){
			if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
				//Record *rec = head;
				//if (searchAvl(currentBucket->bucketEntryList->avlNode,rec) == 0)
					currentBucket->bucketEntryList->avlNode = insertAvl(currentBucket->bucketEntryList->avlNode,(*head));
					//return *bucketList;
				//preorder(currentBucket->bucketEntryList->avlNode);
			}
			(*head) = (*head)->nextRecord;
		} 
		currentBucket->entries++;
		return *bucketList;
	}
	else{
		Bucket *bkt = *bucketList;
		//Record* ptr4 = *head;
		while(bkt!=NULL){
			BucketEntry *entry = bkt->bucketEntryList;
			while(entry !=NULL){
				if (strcmp(entry->name, name) == 0){ //vrhka se poio bucket ienai to sygkekrimeno disease/country
					while((*head)!=NULL){
						if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
							//Record *rec = head;
						//	if(searchAvl(entry->avlNode,rec) == 0)
								entry->avlNode = insertAvl(entry->avlNode,(*head));
								//return *bucketList;
						}
						(*head) = (*head)->nextRecord;
					}
				}
				entry = entry->nextBucketEntry;
			}
			bkt = bkt->nextBucket;
		}
	return *bucketList;
	}
}

void inorder(AvlNode *T)
{
	if(T!=NULL)
	{
		inorder(T->left);
		//printf("%s(Bf=%d)  ",T->ptr->patientLastName,BF(T));
		printf("%d-", T->ptr->entryDate->day);
		printf("%d-", T->ptr->entryDate->month);
		printf("%d\n", T->ptr->entryDate->year);		
		inorder(T->right);
	}
}

int inorder5(AvlNode *T, int *counter,Date *entryDate, Date *exitDate, char*country)
{
	//Date* perm1 = entryDate;
	//Date *perm2 = exitDate;
	if(T!=NULL)
	{
		inorder5(T->left,counter,entryDate,exitDate,country);
		if(compareDates(entryDate,T->ptr->exitDate )== 1 && compareDates(T->ptr->exitDate, exitDate) == 1 && (strcmp(T->ptr->country,country) == 0))
			(*counter)++;
		inorder5(T->right,counter,entryDate,exitDate,country);
	}
	return(*counter);
}
void printEntries(BucketEntry *head){
	BucketEntry *ptr2 = head;
	while(ptr2!= NULL){
		printf("%s\n", ptr2->name);
		inorder(ptr2->avlNode);
		ptr2 = ptr2->nextBucketEntry;
	}
}

void printHash(Bucket* head){
	Bucket *ptr = head;
	int counter = 0;
	while(ptr != NULL){
		//printf("Bucket no: %d has %d\n", counter, ptr->entries);
		printEntries(ptr->bucketEntryList);
		ptr = ptr->nextBucket;
		counter++;
	} 
}

File* insertList_file(File** head, File *current_node){
	// Insert a new node in the end of the list
	File* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_node;
		return *head;
	}	
	help_ptr->next_file= current_node;
	return current_node;
}

void printList(File *head){
	File *node = head;
	while(node != NULL){
		printf("%d-%d-%d and whole filename: %s\n",node->fileDate->day,node->fileDate->month,node->fileDate->year,node->filename);
		node = node->next_file;
	}
	return;
}

int inorder4(AvlNode *T, int *counter,Date *entryDate, Date *exitDate, char* diseaseID)
{
	if(T!=NULL)
	{
		inorder4(T->left,counter,entryDate,exitDate,diseaseID);
		if(compareDates(entryDate,T->ptr->exitDate )== 1 && compareDates(T->ptr->exitDate, exitDate) == 1)
			if(strcmp(T->ptr->diseaseID,diseaseID) == 0)
				(*counter)++;
		inorder4(T->right,counter,entryDate,exitDate,diseaseID);
	}
	return(*counter);
}

void swap(File *node1, File *node2){
	int ptr1, ptr2, ptr3;
	char *temp = node1->filename; 
	ptr1 = node1->fileDate->day;
	ptr2 = node1->fileDate->month;
	ptr3 = node1->fileDate->year;

  	node1->filename = node2->filename; 
  	node1->fileDate->day = node2->fileDate->day;
  	node1->fileDate->month = node2->fileDate->month;
  	node1->fileDate->year = node2->fileDate->year;

  	node2->filename = temp; 
  	node2->fileDate->day = ptr1;
  	node2->fileDate->month = ptr2;
  	node2->fileDate->year = ptr3;
}



void bubbleSort(File* start) 
{ 
    int swapped, i; 
    File *ptr1; 
    File *lptr = NULL; 

    if (start == NULL) 
        return; 
  
     
     do{
     	swapped = 0; 
        ptr1 = start; 
        while (ptr1->next_file != lptr) 
        { 
            if (compareDates(ptr1->fileDate, ptr1->next_file->fileDate) == 0){  
                swap(ptr1, ptr1->next_file); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next_file; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
}

int Hashfunction(char* entry, int buckets){
	//hash table function for strings

	unsigned int h=0, a=20,len,i;
	int value;
	len=strlen(entry);

	for(i=0;i<len;i++){
		value=(int)entry[i];
		h=(h*a + value ) % p;
	}
	return h%buckets;
}
//workers code

int main(int argc, char* argv[]){

	struct sigaction sa2,sa1;
	sa2.sa_sigaction = sig_handler1;
	sa2.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa2.sa_mask);

	if (sigaction(SIGSEGV, &sa2, NULL) == -1)
	   	handle_error("sigaction");

	sigaction(SIGCHLD,&sa2,NULL);
	sigaction(SIGINT,&sa2,NULL);
	sigaction(SIGQUIT,&sa2,NULL);

	int fd2;
	char* input_dir = malloc(sizeof(char)*strlen(argv[1])+1);
	strcpy(input_dir,argv[1]);
	int fd,bytes,i = *argv[2],first_read;
	char buf1[15] ,buffer[8];
	Work_countries *node1 = NULL , *head_list1 = NULL, *head1 = NULL;
	int counter_list1 = 0;
	printf("child no %d is executing....\n",i  );
	Record* head_rec = NULL , *head2_rec = NULL;
	HashNode* diseaseHash = NULL, *counrtyHash = NULL; //2 hashtables , one for diseases and the other for the countries
	diseaseHash = malloc(5*sizeof(HashNode));  
	counrtyHash = malloc(5*sizeof(HashNode));

	int rec_count = 0;
	snprintf(buf1, 15, "rec_fifo%d",i);
	if ((fd = open(buf1, O_RDONLY, 0666)) == -1){
		perror("open child side");
		exit(EXIT_FAILURE);
	}
    do{ //open each directory 
    	char* m = malloc(sizeof(char))+1;
   		if ((first_read = read(fd,m,1)) == -1){
			perror("read");
        	exit(EXIT_FAILURE);
		}
		m[first_read] = '\0';
		int len = atoi(m);
        if ((bytes = read(fd,buffer,(sizeof(char)*len))) == -1){
            perror("read");
            exit(EXIT_FAILURE);
       }
        buffer[bytes] ='\0';
        if(bytes!=0){
	       	DIR *dp = NULL;
	 		struct dirent *dir;
	 		//strcat each subdirectory
	 		char* str = malloc(sizeof(char)*80);
	 		strcpy(str,input_dir);
	 		strcat(str,"/");
	 		strcat(str,buffer);
	 		//printf("str is%s\n", str);
	 		dp = opendir(str);
			if(dp == NULL){
				perror("open directory \n");
				return -1;
			}
			File * head = NULL , *node = NULL , *hh = NULL;
			int keep = 0;
			while ((dir = readdir(dp)) != NULL){
				if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") !=0 ){
					node = malloc(sizeof(struct file));
					node->filename = malloc(sizeof(char)*strlen(dir->d_name)+1);
					strcpy(node->filename,dir->d_name);
					node->next_file = NULL;
		        	//printf("%s\n", dir->d_name); 
		        	node->fileDate = malloc(sizeof(Date));
					char s[2] = "-";
			   		char *token = NULL;
			   		char* temp_name = malloc(sizeof(node->filename)+1);
			   		strcpy(temp_name,node->filename);
			   		token = strtok(temp_name,s);
			   		int counter = 0;
			  		while( token != NULL ) {
			      	if (counter == 0){
			      		node->fileDate->day = atoi(token);
			      		counter++;
			      	}
			      	else if (counter == 1){
			      		node->fileDate->month = atoi(token);
			      		counter++;
			      	}else{
			      		node->fileDate->year = atoi(token);
			      		counter++;
			    	 }
			     	 token = strtok(NULL, s); 
			   		}
			   		head = insertList_file(&head,node); 
			   		if (keep == 0){
						hh = head;
						keep++;
					}
				}
			}

			bubbleSort(hh);
			//read sorted files 
			File *ptr = hh;
			char *temp_file ;
	 		node1 = malloc(sizeof(Work_countries));
	 		node1->country = malloc(sizeof(char)*strlen(buffer));
	 		node1->next_country = NULL;
	 		strcpy(node1->country,buffer);
	 		head_list1 = insertC(&head_list1,node1);
	 		if (counter_list1 == 0){
	 			head1 = head_list1;
	 			counter_list1++;
	 		}
	 		int counter_list2 = 0;
	 		Work_dates* head2 = NULL;
			while(ptr!= NULL){ //for each gile
				temp_file = malloc(sizeof(ptr->filename)+1);
				strcpy(temp_file,ptr->filename);
	 		    Work_dates* node2;
				node2 = malloc(sizeof(Work_dates));
				node2->filename = malloc(sizeof(char)*strlen(temp_file)+1);
				strcpy(node2->filename,temp_file);
				node2->next_date = NULL;
				node2->wds = NULL;
				head2 = insertD(&head2,node2);
				//printf("head is %p\n",head2);
				if(counter_list2 == 0){
					node1->wd = head2;
					counter_list2++;
				}
				char path[80];
				strcpy(path,str);
				strcat(path,"/");
				strcat(path,temp_file);
				//open it
				FILE* fp = fopen(path,"r");
				if (!fp){
				  fprintf(stderr, "Error opening file '%s'\n", "path.txt");
				  return EXIT_FAILURE;
				}
				//else printf("%s\n\n",path);		
				char* line = NULL;
				char* token = NULL;
				char* recordID = NULL,*patientFirstName = NULL,*patientLastName = NULL , *age = NULL, *diseaseID = NULL, *situation = NULL, *country = NULL;
				size_t len = 0;
				Work_diseases *head3 = NULL;
				int counter_list3 = 0;
				while(getline(&line,&len,fp) != -1){ //for each line of the file

	 			    char* line2 = malloc(sizeof(char)*strlen(line)+1);
	 			    strcpy(line2,line);
			  		char * token = strtok(line2, " ");
				   // loop through the string to extract all other tokens
			  		int counter = 0;
				   while( token != NULL ) { //gia kathe lexh ths grammhs
				   		//printf(" token is %s ",token );
				   		if(token !=NULL){
				    		//printf( "%s\n", token ); //printing each token
							switch(counter){
							case 0: recordID = malloc(sizeof(char)*strlen(token)+1);
								strcpy(recordID,token);
								break;
							case 1: situation = malloc(sizeof(char)*strlen(token)+1);
								strcpy(situation,token);
								break;
							case 2: patientFirstName = malloc(sizeof(char)*strlen(token)+1);
								strcpy(patientFirstName,token);
								break;
							case 3: patientLastName = malloc(sizeof(char)*strlen(token)+1);
								strcpy(patientLastName,token);
								break;
							case 4: diseaseID = malloc(sizeof(char)*strlen(token)+1);
								strcpy(diseaseID,token);
								//printf("%s\n",diseaseID );
								break;
							case 5: age = malloc(sizeof(char)*strlen(token)+1);
								strcpy(age,token);
								break;
							}
							counter++; 
				    	}
				    	//printf("%s\n", dis);
				    	token = strtok(NULL, " ");	
	 			    }
	 			    int age_int = atoi(age);
	 			    
		 		    Work_diseases* node3;
					node3 = malloc(sizeof(Work_diseases));
					node3->dis = malloc(sizeof(char)*strlen(diseaseID)+1);
					strcpy(node3->dis,diseaseID);
					Date* exitDate = malloc(sizeof(Date));
			    	Date* entryDate = malloc(sizeof(Date));			    		
			    	if (strcmp(situation,"ENTER") == 0){
			    		exitDate->day = 0;
			    		exitDate->month = 0;
			    		exitDate->year = 0;
			    		entryDate->day = ptr->fileDate->day;
			    		entryDate->month = ptr->fileDate->month;
			    		entryDate->year = ptr->fileDate->year;	
			    	}
			    	else{
    					exitDate->day = ptr->fileDate->day;
			    		exitDate->month = ptr->fileDate->month;
			    		exitDate->year = ptr->fileDate->year;
			    		entryDate->day = 0;
			    		entryDate->month =0;
			    		entryDate->year = 0;
			    	}
			    	if (strcmp(situation,"ENTER") == 0){
						if (TraverseList(diseaseID,node2->wds) == 0){ //if it does not exits, i create it
							head3 = insertDs(&head3,node3);
							if (age_int <= 20){
								node3->first_range = 1;
								node3->sec_range = 0;
								node3->third_range = 0;
								node3->fourth_range = 0;
							}
							else if (age_int <= 40){
								node3->first_range = 0;
								node3->sec_range = 1;
								node3->third_range = 0;
								node3->fourth_range = 0;
							}
							else if (age_int <= 60){
								node3->first_range = 0;
								node3->sec_range = 0;
								node3->third_range = 1;
								node3->fourth_range = 0;
							}
							else{
								node3->first_range = 0;
								node3->sec_range = 0;
								node3->third_range = 0;
								node3->fourth_range = 1;
							}
						}
						else{// it exists, so i search for it
							Work_diseases * perm = node2->wds;
							while(perm != NULL){
								if (strcmp(perm->dis,diseaseID) == 0){
									if (age_int <= 20)
										perm->first_range++;
									else if (age_int <= 40)
										perm->sec_range++;
									else if (age_int <= 60)
										perm->third_range++;
									else
										perm->fourth_range++;
									//break;
								}
								perm = perm->next_disease;	
							}
						}
						if(counter_list3 == 0){
							node2->wds = head3;
							counter_list3++;
						}
					}
			    	country = malloc(sizeof(char)*strlen(buffer)+1);
			    	strcpy(country,buffer);
			    	Record* new = createRecord(recordID,situation,patientFirstName,patientLastName,diseaseID,country,entryDate,exitDate,age_int);
			    	new->nextRecord  = NULL;
			    	int flag = 0;
			    	
			    	if (strcmp(new->situation,"ENTER") != 0){ 
				    	Record* cur = head_rec;
				    	while(cur!= NULL){
				    		if (strcmp(cur->recordID,new->recordID) == 0){ //an vrethei to record id
				    			cur->exitDate->day = new->exitDate->day;
				    			cur->exitDate->month = new->exitDate->month;
				    			cur->exitDate->year = new->exitDate->year;
				    		}
				    		cur = cur->nextRecord;
				    	}
		
					}
					else flag = 1; 
					if (flag == 1){
				    	head_rec = insertRecord(&head_rec,new);
				    	if (rec_count == 0){
				    		head2_rec = head_rec;
				    		rec_count++;
				    	}
					   int pos_dis = Hashfunction(diseaseID,5);
					   int pos_country = Hashfunction(country,5);
					   Record* new2 = new;
					   new2->nextRecord = NULL;
					   diseaseHash[pos_dis].bucketList = insertBucket(&(diseaseHash[pos_dis].bucketList),3, diseaseID,&new);
					   counrtyHash[pos_country].bucketList = insertBucket(&(counrtyHash[pos_country].bucketList),3, country,&new2); 
					}
					else printf("Error\n");
				}
				ptr = ptr->next_file;
			}
			closedir(dp);
	    }
    }while (bytes > 0);
    Work_countries* ptr1 = head1;
	while(ptr1!=NULL){
		Work_dates * ptr2 = ptr1->wd;
		while(ptr2!=NULL){
			char* send = malloc(sizeof(char)*strlen(ptr2->filename)+1);
			strcpy(send,ptr2->filename);
			send = realloc(send,sizeof(char)*strlen("\n")+1);
			strcat(send,"\n");

			send = realloc(send,sizeof(char)*strlen(ptr1->country)+1);
			strcat(send,ptr1->country);
	
			send = realloc(send,sizeof(char)*strlen("\n")+1);
			strcat(send,"\n");
			Work_diseases *ptr3 = ptr2->wds;
			while(ptr3 != NULL){ //send statistics
				char* temp_disease = malloc(sizeof(char)*strlen(ptr3->dis)+1);
				strcpy(temp_disease,ptr3->dis);
				char* nl = malloc(sizeof(char)*strlen("\n")+1);
				strcpy(nl,"\n");
				strcat(send,temp_disease);
				strcat(send,nl);
				
				char* temp1, *temp2 ,*temp3 ,*temp4;
				temp1 = malloc(sizeof(char)*strlen("Age range 0-20 years: %d cases\n")+1);
				sprintf(temp1, "Age range 0-20 years: %d cases\n",ptr3->first_range);				
				//printf("Age range 0-20 years: %d cases\n",head1->wd->wds->first_range);
				strcat(send,temp1);
				
				temp2 = malloc(sizeof(char)*strlen("Age range 21-40 years: %d cases\n")+1);
				sprintf(temp2, "Age range 21-40 years: %d cases\n",ptr3->sec_range);
				//printf("Age range 21-40 years: %d cases\n",head1->wd->wds->sec_range);
				strcat(send,temp2);
				
				temp3 = malloc(sizeof(char)*strlen("Age range 41-60 years: %d cases\n")+1);
				sprintf(temp3, "Age range 41-60 years: %d cases\n",ptr3->third_range);
				//printf("Age range 41-60 years: %d cases\n",head1->wd->wds->third_range);
				strcat(send,temp3);

				temp4 = malloc(sizeof(char)*strlen("Age range 60+ years: %d cases\n")+1);
				sprintf(temp4, "Age range 60+ years: %d cases\n",ptr3->fourth_range);
				//printf("Age range 60+ years: %d cases\n",head1->wd->wds->fourth_range);
				strcat(send,temp4);

				ptr3 = ptr3->next_disease;
			}
			//printf("sending child %d....................\n",i );
			char buf2[15];
			sprintf(buf2, "send_fifo%d", i);
			//printf("buf2 is %s\n",buf2 );
			int b;
			if ((fd2 = open(buf2, O_WRONLY ,0666)) == -1){
				perror("opening pipe in child2");
				exit(EXIT_FAILURE);
			} 
			int len = strlen(send); //mhkos aftounou poy thelw na steilw
			int num = len;
			int digits = 0;
			while(num>0){
				num = num/10;
				digits++;
			}

			char* length = malloc(sizeof(char)*digits + strlen(send)+ 1+ strlen("_")+1);
			itoa(len,length);
			strcat(length,"_");
			strcat (length,send);
			if ((b = write(fd2, length ,strlen(length)+1)) == -1){
				perror("write pipe");
				exit(EXIT_FAILURE);
			}
			close(fd2);
			ptr2 = ptr2->next_date;
		} 
		ptr1 = ptr1->next_country;
	}

	int m;

	//end with statistis
	close(fd);
	close(fd2);
	int fd3,fd6, fd4,fd5,fdd;
	char* buf4, *buf5,buf6;
	//printf("end of statistics\n");
	//Loop for the orders

	while(1){
		if(orderds_end == -1){ //received signal
			printf("received SIGINT/SIGQUIT\n");
			kill(getpid(),SIGKILL);
			char* buf1 = malloc(sizeof(char)*15);
		   	char* buf2 = malloc(sizeof(char)*15);
		   	snprintf(buf1, 15, "rec_fifo%d", i);
		   	snprintf(buf2, 15, "send_fifo%d", i);
		   	unlink(buf1);
		   	unlink(buf2);
		   	free(buf1);
		   	free(buf2);
		   	exit(0);
		}
		char* bf = malloc(sizeof(char)*15);
		snprintf(bf,15,"rec_fifo%d",i);
		fdd = open(bf,O_RDONLY,0666);
		int check = 0; // 
		read(fdd,&check,sizeof(int));
		if (check == 1){ //one spesific order run ( searchPatientRecord)
			buf4 = malloc(sizeof(char)*15);
			snprintf(buf4, 15, "rec_fifo%d", i);
			//int fd4; /// gia na grafoun ta paidia
			if ((fd3 = open(buf4, O_RDONLY ,0666)) == -1){
				perror("opening pipe in parentn");
				 exit(EXIT_FAILURE);
			} 
			//printf("perasa\n");
			int val, counter = 0; 
			int bytesread;
			char* buffer = malloc(sizeof(char)*1024);
			memset(buffer,'\0',1024);	
				bytesread = read(fd3, &val,sizeof(int));
				bytesread = read(fd3,buffer,val);
				buffer[bytesread]='\0';
				Record* hh = head2_rec;
				//printRecordList(hh);

				int cc = 0;
				//char buf5[15];
				buf5 = malloc(sizeof(char)*15);
				snprintf(buf5, 15, "send_fifo%d", i);

				int len;
				if ((fd4 = open(buf5, O_WRONLY ,0666)) == -1){
					perror("opening pipe in parentn hkk ");
					exit(EXIT_FAILURE);
				} 
				int found = 0;
				Record *help = hh;
				while(help!=NULL){
					if(strcmp(help->recordID,buffer)== 0){
						found = 1;
						write(fd4,&found,sizeof(int));

						len = strlen(help->recordID);
						write(fd4,&len,sizeof(int));
						write(fd4,help->recordID,sizeof(char)*len);

						len = strlen(help->patientFirstName);
						write(fd4,&len,sizeof(int));
						write(fd4,help->patientFirstName,sizeof(char)*len);

						len = strlen(help->patientLastName);
						write(fd4,&len,sizeof(int));
						write(fd4,help->patientLastName,sizeof(char)*len);

						len = strlen(help->diseaseID);
						write(fd4,&len,sizeof(int));
						write(fd4,help->diseaseID,sizeof(char)*len);

						write(fd4,&help->age,sizeof(int));
						
						write(fd4,&help->entryDate->day,sizeof(int));
						write(fd4,&help->entryDate->month,sizeof(int));
						write(fd4,&help->entryDate->year,sizeof(int));

						write(fd4,&help->exitDate->day,sizeof(int));
						write(fd4,&help->exitDate->month,sizeof(int));
						write(fd4,&help->exitDate->year,sizeof(int)); 
						//break;
						//close(fd4);
						//close(fd3);
						//break;
					} 
					help = help->nextRecord;
				}
			close(fd4);
			close(fd3);
			free(buffer);
			//exit(0);
		}  
		if (check == 2){ //the other order runs
			int  bytesread, val;
			//char* buffer = malloc(sizeof(char)*30);
			char buf5[15];
			snprintf(buf5, 15, "rec_fifo%d", i);
			//printf("thn anoixa\n");
			int len;
			if ((fd5 = open(buf5, O_RDONLY ,0666)) == -1){
				perror("opening pipe");
				exit(EXIT_FAILURE);
			}
			
			bytesread = read(fd5, &val,sizeof(int));
			//printf("val: %d\n", val);
			char* diseaseID = malloc(sizeof(char)*30);
			bytesread = read(fd5,diseaseID,val);
			diseaseID[bytesread] = '\0';
			
			char* entrydate = malloc(sizeof(char)*30);
			bytesread = read(fd5, &val,sizeof(int));
			bytesread = read(fd5,entrydate,val);
			entrydate[bytesread]='\0';
			
			char* exitdate = malloc(sizeof(char)*30);
			bytesread = read(fd5, &val,sizeof(int));
			bytesread = read(fd5,exitdate,val);
			exitdate[bytesread]='\0';

			char* country = malloc(sizeof(char)*30);
			bytesread = read(fd5, &val,sizeof(int));
			bytesread = read(fd5,country,val);
			country[bytesread]='\0';
			
			int k;
			Bucket* b;
			//token the dates
			Date* entryDate = malloc(sizeof(Date));
			Date* exitDate = malloc(sizeof(Date));
			char* token1 = strtok(entrydate,"-");
		    int counter1 = 0;
		    while( token1 != NULL ) {
		      if (counter1 == 0){
		      	entryDate->day = atoi(token1);
		      	counter1++;
		      }
		      else if (counter1 == 1){
		      	entryDate->month = atoi(token1);
		      	counter1++;
		      }else{
		      	entryDate->year = atoi(token1);
		      	counter1++;
		      }
		      token1 = strtok(NULL,"-");
		   }
			char* token2 = strtok(exitdate,"-");
		    int counter2 = 0;
		    while( token2 != NULL ) {
		      if (counter2 == 0){
		      	exitDate->day = atoi(token2);
		      	counter2++;
		      }
		      else if (counter2 == 1){
		      	exitDate->month = atoi(token2);
		      	counter2++;
		      }else{
		      	exitDate->year = atoi(token2);
		      	counter2++;
		      }
		      token2 = strtok(NULL,"-");
		   }
			int* patients = malloc(sizeof(int));
			(*patients) = 0;
			for(k=0;k<5;k++){
				b = counrtyHash[k].bucketList;
				while(b!=NULL){
					BucketEntry *ptr = b->bucketEntryList;
					while(ptr!=NULL){
						if(strcmp(country,"error")!=0){
							if(strcmp(ptr->name,country) == 0 ){// th vrhka th xwra
								//printf("%s %s\n",ptr->name,country );
								(*patients) = inorder5(ptr->avlNode,patients,entryDate,exitDate,country);
								printf("%d\n",*patients );

							}
						}
						else{
							(*patients) = inorder5(ptr->avlNode,patients,entryDate,exitDate,country);
							printf("%s %d\n",ptr->name,*patients );							
						}
						ptr = ptr->nextBucketEntry;
					}
					b = b->nextBucket;
				}
			}
			free(b);
			free(diseaseID);
			free(entrydate);
			free(exitdate);
			free(country);
		}
		close(fd5);
	}
	unlink(buf4);
  	unlink(buf5); 
  	deleteList(&head2_rec);
  	exitHash(diseaseHash,counrtyHash);
   	exit(0);
}