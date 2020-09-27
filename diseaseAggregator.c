#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include "structs.h"

#define PERMS 0666
#define handle_error(msg)  do { perror(msg); exit(EXIT_FAILURE); } while (0)

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
	char* line1;
	char* line2;
	char* line3;
	char* line4;
	int first_range;
	int sec_range;
	int third_range;
	int fourth_range;
	int total;
	struct work_diseases* next_disease;
}Work_diseases;

typedef struct work_dates{
	char* filename;
	Date* filedate;
	struct work_diseases* wds;	
	struct work_dates *next_date;
}Work_dates;

typedef struct work_countries{
	char* country;
	int counter;
	struct work_dates* wd;
	struct work_countries* next_country;
}Work_countries;

void deleteRang_List(Range_list** head) 
{ 
   Range_list* current = *head; 
   Range_list* next;  
   while (current != NULL)  { 
       next = current->next; 
       free(current);
       current = next; 
   }   
   *head = NULL; 
}

void deleteLList( List** head) 
{ 
   List* current = *head; 
   List* next; 
   while (current != NULL)  
   { 
       next = current->next; 
       free(current->country);
       free(current); 
       current = next; 
   }  
   *head = NULL; 
} 
void deleteListWDS(Work_diseases** head) 
{ 
   Work_diseases* current = *head; 
   Work_diseases* next;  
   while (current != NULL)  { 
       next = current->next_disease; 
       free(current->dis);
       free(current);
       current = next; 
   }   
   *head = NULL; 
} 

void deleteListWD(Work_dates** head) 
{ 
   Work_dates* current = *head; 
   Work_dates* next;  
   
   while (current != NULL)  { 
       next = current->next_date; 
       deleteListWDS(&current->wds);
       free(current->filename);
       free(current->filedate);
       free(current);
       current = next; 
   }
   *head = NULL; 
} 


void deleteListWC(Work_countries** head) 
{ 
   Work_countries* current = *head; 
   Work_countries* next;  
   
   while (current != NULL)  {
       next = current->next_country;
        deleteListWD(&current->wd);
        free(current->country);

     	free(current);
       current = next; 
   }   
   *head = NULL; 
} 

void swap_range(Range_list *node1, Range_list *node2){
	int ptr, ptr2;
	ptr2 = node1->type; 
	ptr = node1->value;

  	node1->type = node2->type; 
  	node1->value = node2->value;

  	node2->type = ptr2; 
  	node2->value = ptr;
}

void sort_range(Range_list* start) 
{ 
    int swapped, i; 
    Range_list *ptr1; 
    Range_list *lptr = NULL; 

    if (start == NULL) 
        return; 
     do{
     	swapped = 0; 
        ptr1 = start; 
        while (ptr1->next != lptr) 
        { 
            if (ptr1->value<ptr1->value){  
                swap_range(ptr1, ptr1->next); 
                swapped = 1;
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
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

void add_firstD( Work_dates **head,Work_dates *new_node ) 
{
   new_node->next_date = *head;
   *head = new_node;
}

void add_firstDs( Work_diseases **head,Work_diseases *new_node ) 
{
   new_node->next_disease = *head;
   *head = new_node;
}

void add_firstRange( Range_list **head,Range_list *new_node ) 
{
   new_node->next = *head;
   *head = new_node;
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

List* insertList_parent(List** head, List *current_node){
	// Insert a new node in the end of the list
	List* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_node;
		return *head;
	}	
	help_ptr->next= current_node;
	return current_node;
}

int TraverseCList(char* country, Work_countries* head){
	Work_countries *ptr = head;
	while(ptr!=NULL){
		if (strcmp(ptr->country, country) == 0){
			return 1;
		}
		ptr = ptr->next_country;
	}
	return 0;
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
     if ((sign = n) < 0) 
         n = -n;         
     i = 0;
     do {     
         s[i++] = n % 10 + '0';  
     } while ((n /= 10) > 0); 
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void create_lofFiles(List *head, int total,int success , int fail){
	id_t paretnt_pid = getpid();
	orderds_end = 0;
	char proc[20],buffer[50];
	sprintf(proc,"%d\n",paretnt_pid);
	char filename[50];
	strcpy(filename,"log_file.");
	strcat(filename,proc);
	FILE *fp;
	fp = fopen (filename, "w");
	if(fp == NULL){
		perror("create logfile");
	}
	List* ptr = head;
    while(ptr!=NULL){
    	strcpy(buffer,ptr->country);
    	strcat(buffer,"\n");
    	fwrite(buffer,1,strlen(buffer),fp);
    	ptr = ptr->next;
    }
   	sprintf(buffer,"TOTAL %d\n", total);
    fwrite(buffer,1,strlen(buffer),fp);
 	sprintf(buffer,"SUCCESS %d\n", success);
	fwrite(buffer,1,strlen(buffer),fp);
	sprintf(buffer,"FAIL %d\n", fail);
    fwrite(buffer,1,strlen(buffer),fp);
    
   	fclose(fp);
   	return;
}

int main(int argc , char*argv[]){
	struct sigaction sa,sa1;
	sa.sa_sigaction = sig_handler1; 
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGSEGV, &sa, NULL) == -1)  //seg fault
	   	handle_error("sigaction");

	sigaction(SIGCHLD,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGQUIT,&sa,NULL);

	if(argc != 7){
		perror("wrong number of arguments");
		return -1;
	}
	int i, numWorkers,bufferSize , filedesc;
	pid_t pid;
	char* input_dir;
	int total = 0,success=0, fail=0;
	for(i=1; i<=5; i=i+2){
		if(strcmp(argv[i], "-w")==0){
			numWorkers=atoi(argv[i+1]);		
		}	
		else if(strcmp(argv[i], "-i")==0){
			input_dir=malloc(sizeof(char)*strlen(argv[i+1])+1);
			strcpy(input_dir,argv[i+1]);	
		}
		else if(strcmp(argv[i], "-b")==0){
			bufferSize=atoi(argv[i+1]);
		}
	}
	int proc_id[numWorkers]; //array with the proc ids of each child
	int fd5;
 	DIR *dp = NULL;
 	struct dirent *dir;
	dp = opendir(input_dir); 
	if(dp==NULL){
		perror("input directory does not exist\n");
		return -1;
	}

	List *head = NULL ,*head2 = NULL;
	int count = -2 , keepHead = 0 , current_workers = 1;

 	List* node = NULL;	
	while ((dir = readdir(dp)) != NULL){ //reaf the directory
        count++;
        if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") !=0 ){
	        node = malloc(sizeof(struct list));
	        node->country = malloc(sizeof(char)*strlen(dir->d_name)+1);
	        strcpy(node->country,dir->d_name);
	        if (current_workers != numWorkers){
	        	node->num_work = current_workers;
	        	current_workers++;
	        }
	        else{
	        	node->num_work = current_workers;
	        	current_workers = 1;
	        }
	        node->next = NULL;
	        head = insertList_parent(&head,node);
	        if(keepHead == 0){
	        	head2 = head;
	        	keepHead++;
	        }
	    }
    }
    List* ptr = head2;
    // share the jobs
    while(ptr!=NULL){
    	printf("%s in worker %d\n",ptr->country,ptr->num_work);
    	ptr = ptr->next;
    }
    printf("\n");
    closedir(dp);
    int fd , bytes, len, status;
   	for(i=1; i<=numWorkers; i++){
	   		//create fifo's
	   		char* buf1 = malloc(sizeof(char)*15);  
	   		char* buf2 = malloc(sizeof(char)*15);
	   	    snprintf(buf1, 15, "rec_fifo%d", i);
	   		snprintf(buf2, 15, "send_fifo%d", i);
	   		mkfifo(buf1,PERMS);
	   		mkfifo(buf2,PERMS);
	   		//create children
			pid = fork(); 
			proc_id[i] = pid;
			if(pid == -1){
				perror("error in fork");
				exit(EXIT_FAILURE);
			}
			else if(pid > 0){
			 // parent's code 
				if ((fd = open(buf1, O_WRONLY ,PERMS)) == -1){
					perror("opening pipe in parent");
					exit(EXIT_FAILURE);
				}
				List* hh = head2;
				char* length = NULL;
				while(hh != NULL){
					if ((hh->num_work) == i){
						len = strlen(hh->country)+1;
						int num = len;
						int digits = 0;
						while(num>0){
							num = num/10;
							digits++;
						}
						//printf("%d with %d digits \n",len,digits );
						length = malloc(sizeof(char)*100);
						memset(length,'\0',digits);
						itoa(len,length);
						strcat(length,hh->country);
						if ((bytes = write(fd, length ,strlen(length)+1)) == -1){
							perror("write pipe");
							exit(EXIT_FAILURE);
						}
						free(length);
					}
					hh = hh->next;
				}
				free(hh);
				free(buf1);
	   			free(buf2);
				close(fd);
			}
			else{
				int *ptr = malloc(sizeof(int));
				int *length = malloc(sizeof(int));
				ptr = &i;
				//printf(" worker no : %d\n", i);
				fflush(stdout);
				execl("./worker","worker",input_dir,ptr, NULL);
				exit(0);
			}
		close(fd);
	}
	//check if a child has died with SIGCHLD
	if(childEnd>0){
		int found, i;
		for(int i=1;i<=numWorkers;i++){
			if(childEnd == proc_id[i])
				found=i;
		}
		char* buf1 = malloc(sizeof(char)*15);
   		char* buf2 = malloc(sizeof(char)*15);
   	    snprintf(buf1, 15, "rec_fifo%d", found);
   		snprintf(buf2, 15, "send_fifo%d",found);
   		unlink(buf1);
   		unlink(buf2);
   		free(buf1);
   		free(buf2);

   		char* buf1_new = malloc(sizeof(char)*15);
   		char* buf2_new = malloc(sizeof(char)*15);
   	    snprintf(buf1_new, 15, "rec_fifo%d", found);
   		snprintf(buf2_new, 15, "send_fifo%d", found);
   		mkfifo(buf1_new,PERMS);
   		mkfifo(buf2_new,PERMS);

   		int new_pid = proc_id[found] = fork(); //create a new child
   		if (new_pid == -1)
   			perror("fork failed");
   		else if( new_pid == 0){
   			int *ptr = malloc(sizeof(int));
   			ptr = &found;
   			execl("./work4","work4",input_dir,ptr, NULL);

   		}else{
  			 // parent's code 
				if ((fd = open(buf1, O_WRONLY ,PERMS)) == -1){
					perror("opening pipe in parent");
					exit(EXIT_FAILURE);
				}
				List* hh = head2;
				char* length = NULL;
				while(hh != NULL){
					if ((hh->num_work) == i){
						len = strlen(hh->country)+1;
						int num = len;
						int digits = 0;
						while(num>0){
							num = num/10;
							digits++;
						}
						//printf("%d with %d digits \n",len,digits );
						length = malloc(sizeof(char)*100);
						memset(length,'\0',digits);
						itoa(len,length);
						strcat(length,hh->country);
						if ((bytes = write(fd, length ,strlen(length)+1)) == -1){
							perror("write pipe");
							exit(EXIT_FAILURE);
						}
						free(length);
					}
					hh = hh->next;
				}
				free(hh);
				free(buf1);
	   			free(buf2);
				close(fd); 			
   		}


	}
	int fd2;
	Work_countries* array[numWorkers];
	//char* help_ptr;
	for(i=1; i <= numWorkers; i++){ //for each worker i print the statistics and i keep them in a list
		array[i] = NULL;
		Work_countries* head1 = NULL;
		Work_dates *head2 = NULL ;
		int counter1 = 0;
		//printf("for child no %d\n",i );
		int  b, res;
		//char buffer[100];
		char* buffer = NULL;
		char buf2[15];
		char * keep_file = NULL, *help_ptr = NULL;
		int r1, r2,r3 ,r4;
		//printf("ed?\n");	
		snprintf(buf2,15, "send_fifo%d", i);
		if ((fd2 = open(buf2, O_RDONLY ,PERMS)) == -1){
			printf("error in opening pipe %s\n",buf2 );
			perror("opening pipe in parent2");
			exit(EXIT_FAILURE);
		}
		do{	//gia kathe file ths kathe xwras
			//printf("	mohka for child %d\n",i );	
			int first_read;
			char* m =malloc(sizeof(char));
			char* result = malloc(sizeof(char)*50);
			int coun = 0;
			do{
				free(m);
				m = malloc(sizeof(char)+1);
		   		if ((first_read = read(fd2,m,1)) == -1){
					perror("read");
		        	exit(EXIT_FAILURE);
				}
				m[first_read] = '\0';
				usleep(1);
				if(coun == 0){
					strcpy(result,m);
					coun++;
				}
				else strcat(result,m); 
				if (first_read == 0) {//i eof{
					printf("reached EOF\n");
					break;
				}
			}while((strcmp(m,"_")!=0)); 
			free(m);
			//LastcharDel(result);
			int temp_len = strlen(result);
			if (temp_len!=0 ){
			//printf("TEMP LEN %d\n",temp_len);
				result[temp_len-1] = '\0';
			}
			res = atoi(result);
			if( res!=0){
				buffer = realloc(buffer,sizeof(char)*res+1);
				if ((b = read(fd2,buffer,res)) == -1){
			       perror("read");
			       exit(EXIT_FAILURE);
			    }
			    buffer[b] ='\0';
			    printf("%s\n",buffer );
			   	help_ptr = malloc(sizeof(char)*strlen(buffer)+1);
			    strcpy(help_ptr,buffer);
			    //free(buffer);
			    char *token = strtok(help_ptr,"\n");
			    //free(help_ptr);
			    int counter = -2; 
			    Work_dates *node2;
			    Work_diseases *node3;
			    //free(help_ptr);
			    while( token != NULL ){
			    	counter++;
			    	//printf(" token no %d is %s\n",counter,token);
			    	if (counter == -1){// filename
			    		keep_file = malloc(sizeof(char)*strlen(token)+1);
			    		strcpy(keep_file,token);
			    	}
			    	else if (counter == 0){
			    		Work_countries* node1 = malloc(sizeof(Work_countries));
			    		node1->country = malloc(sizeof(char)*strlen(token)+1);
			    		strcpy(node1->country,token);
			    		node1->counter = 0;
			    		node1->next_country = NULL;
			    		node1->wd = NULL;

			    		node2 = malloc(sizeof(Work_dates));
			    		node2->filename = malloc(sizeof(char)*strlen(keep_file)+1);
			    		strcpy(node2->filename,keep_file);
			    		node2->wds = NULL;

			    		if (TraverseCList(token,array[i])!=1){
			    			head1 = insertC(&head1,node1);
			    		}
			    		//head2 = insertD(&head2,node2);
			    		if (counter1 == 0){
			    			array[i] = head1;
			    			//array[i]->wd = head2;
			    			counter1++;
			    		}
			    		Work_countries* ptr1 = array[i];
			    		while(ptr1!= NULL){
			    			if( strcmp(ptr1->country,token)== 0)
			    				add_firstD(&ptr1->wd,node2);
			    			ptr1 = ptr1->next_country;
			    		}
			    	}else if ((counter-1)%5 == 0){ 
			    		node3 = malloc(sizeof(Work_diseases));
			    		node3->dis = malloc(sizeof(char)*strlen(token)+1);
			    		node3->first_range = 0;
			    		node3->sec_range = 0;
			    		node3->third_range = 0;
			    		node3->fourth_range = 0;
			    		strcpy(node3->dis,token);
			    		node3->next_disease = NULL;
			    		add_firstDs(&node2->wds,node3);
			    	}
			    	else if((counter-2)%5 == 0){
			    		//printf("gia hlikia 0 -20 %s\n",token );
			    		//printf("token %s\n",token );
			    		node3->line1 = token;
			    		//char* new = strtok(token,".");
			    	} 
			    	else if((counter-3)%5 == 0){
			    		//printf("token %s\n",token );
			    		node3->line2 = token;
			    	}
			    	else if((counter-4)%5 == 0){
			    		//printf("token %s\n",token );
			    		node3->line3 = token;
			    	}
			    	else if((counter-5)%5 == 0){
			    		//printf("token %s\n",token );
			    		node3->line4 = token;
			    	}
			    	token = strtok(NULL,"\n");

			   }
			   free(keep_file);
			   //free(help_ptr);
			}
			//free(help_ptr);
			free(result);
	    }while (b > 0 && res !=0);

	    free(buffer);
	    //free(help_ptr);
	    //free(keep_file);
	}
	close(fd2);
	//free(help_ptr);
	if(orderds_end == -1){ //received signal
		List* perm1 = head2;
		printf("received SIGINT/SIGQUIT\n");
		create_lofFiles(perm1,total,success,fail);
		int i;
		for(i=1;i<=numWorkers; i++){
				kill(proc_id[i],SIGKILL);
				char* buf1 = malloc(sizeof(char)*15);
		   		char* buf2 = malloc(sizeof(char)*15);
		   	    snprintf(buf1, 15, "rec_fifo%d", i);
		   		snprintf(buf2, 15, "send_fifo%d", i);
		   		unlink(buf1);
		   		unlink(buf2);
		   		free(buf1);
		   		free(buf2);
		}
		exit(0);
	}
	for(i= 1; i<=numWorkers; i++){ //statistics in data stuctures
		//printf("for child %d with %d\n", i, proc_id[i]);
		Work_countries* head1 = array[i];
		while(head1!= NULL){
			Work_dates *head2 = head1->wd;
			while(head2!= NULL){
				char* tok = strtok(head2->filename,".");
				char* token = strtok(tok,"-");
				int counter = 0;
				head2->filedate = malloc(sizeof(Date));
				while(token!=NULL){
					if (counter == 0){
						head2->filedate->day = atoi(token);
					}
					else if (counter == 1){
						head2->filedate->month = atoi(token);
					}
					else if (counter == 2){
						head2->filedate->year = atoi(token);
					}
					counter++;
					token = strtok(NULL,"-");
				}
				Work_diseases* head3 = head2->wds;
				while(head3!= NULL){ //for each disease
					char* t1 = strtok(head3->line1," ");
					int counter = 0;
					while(t1!=NULL){
						if (counter == 4)
							head3->first_range = atoi(t1);
						counter++;
						t1 = strtok(NULL," ");
					}
					char* t2 = strtok(head3->line2," ");
					int counter2 = 0;
					while(t2!=NULL){
						if (counter2 == 4)
							head3->sec_range = atoi(t2);
						counter2++;
						t2 = strtok(NULL," ");
					}
					char* t3 = strtok(head3->line3," ");
					int counter3 = 0;
					while(t3!=NULL){
						if (counter3 == 4)
							head3->third_range = atoi(t3);
						counter3++;
						t3 = strtok(NULL," ");
					}
					char* t4 = strtok(head3->line4," ");
					int counter4 = 0;
					while(t4!=NULL){
						if (counter4 == 4)
							head3->fourth_range = atoi(t4);
						counter4++;
						t4 = strtok(NULL," ");
					}
					head3->total = head3->first_range + head3->sec_range + head3->third_range + head3->fourth_range;
					head3 = head3->next_disease;
				}
				head2 = head2->next_date;
			}
			head1 = head1->next_country;
		}
	}
	char *ch = malloc(30*sizeof(char)+1);
	char c;
	if(orderds_end == -1){ //received signal
		List* perm1 = head2;
		printf("received SIGINT/SIGQUIT\n");
		create_lofFiles(perm1,total,success,fail);
		int i;
		for(i=1;i<=numWorkers; i++){
				kill(proc_id[i],SIGKILL);
				char* buf1 = malloc(sizeof(char)*15);
		   		char* buf2 = malloc(sizeof(char)*15);
		   	    snprintf(buf1, 15, "rec_fifo%d", i);
		   		snprintf(buf2, 15, "send_fifo%d", i);
		   		unlink(buf1);
		   		unlink(buf2);
		   		free(buf1);
		   		free(buf2);
		}
		exit(0);
	}
	close(fd);
	printf("waiting for querries\n");
	if(orderds_end == -1){ //received signal
		List* perm1 = head2;
		printf("received SIGINT/SIGQUIT\n");
		create_lofFiles(perm1,total,success,fail);
		int i;
		for(i=1;i<=numWorkers; i++){
				kill(proc_id[i],SIGKILL);
				char* buf1 = malloc(sizeof(char)*15);
		   		char* buf2 = malloc(sizeof(char)*15);
		   	    snprintf(buf1, 15, "rec_fifo%d", i);
		   		snprintf(buf2, 15, "send_fifo%d", i);
		   		unlink(buf1);
		   		unlink(buf2);
		   		free(buf1);
		   		free(buf2);
		}
		exit(0);
	}
    scanf("%s",ch);
	if(orderds_end == -1){ //received signal
		List* perm1 = head2;
		printf("received SIGINT/SIGQUIT\n");
		create_lofFiles(perm1,total,success,fail);
		int i;
		for(i=1;i<=numWorkers; i++){
				kill(proc_id[i],SIGKILL);
				char* buf1 = malloc(sizeof(char)*15);
		   		char* buf2 = malloc(sizeof(char)*15);
		   	    snprintf(buf1, 15, "rec_fifo%d", i);
		   		snprintf(buf2, 15, "send_fifo%d", i);
		   		unlink(buf1);
		   		unlink(buf2);
		   		free(buf1);
		   		free(buf2);
		}
		exit(0);
	}
    total++;
   	while(strcmp(ch,"/exit")!=0){
	if(orderds_end == -1){ //received signal
		List* perm1 = head2;
		printf("received SIGINT/SIGQUIT\n");
		create_lofFiles(perm1,total,success,fail);
		int i;
		for(i=1;i<=numWorkers; i++){
			kill(proc_id[i],SIGKILL);
			char* buf1 = malloc(sizeof(char)*15);
	   		char* buf2 = malloc(sizeof(char)*15);
	   	    snprintf(buf1, 15, "rec_fifo%d", i);
	   		snprintf(buf2, 15, "send_fifo%d", i);
	   		unlink(buf1);
	   		unlink(buf2);
	   		free(buf1);
	   		free(buf2);
			}
			exit(0);
		}
	   	if(strcmp(ch,"/listCountries") == 0){
	   		total++;
	   		for(i=1; i<=numWorkers; i++){
	   			Work_countries* head = array[i];
	   			while(head !=NULL){
	   				printf("%s %d\n", head->country, proc_id[i]);
	   				head = head->next_country;
	   			}
	   		}
	   	}
	   	else if(strcmp(ch,"/diseaseFrequency") == 0){
	   		total++;
	   		c = getchar();
	   		if(c == ' '){
	   			char* virusName = malloc(sizeof(char)*32);
	   			char* entrydate = malloc(sizeof(char)*32);
	   			char* exitdate = malloc(sizeof(char)*32);
	   			Date* entryDate = malloc(sizeof(Date));
	   			Date* exitDate = malloc(sizeof(Date));
	   			scanf("%s",virusName);
	   			getchar();
	   			scanf("%s",entrydate);
	   			getchar();
	   			scanf("%s",exitdate);
	   			char* token = strtok(entrydate,"-");
			    int counter = 0;
			    while( token != NULL ) {
			      if (counter == 0){
			      	entryDate->day = atoi(token);
			      	counter++;
			      }
			      else if (counter == 1){
			      	entryDate->month = atoi(token);
			      	counter++;
			      }else{
			      	entryDate->year = atoi(token);
			      	counter++;
			      }
			      token = strtok(NULL,"-");
			   }
			    char* token1 = strtok(exitdate,"-");
			    int counter1 = 0;
			    while( token1 != NULL ){
			      if (counter1 == 0){
			      	exitDate->day = atoi(token1);
			      	//counter1++;
			      }
			      else if (counter1 == 1){
			      	exitDate->month = atoi(token1);
			      	//counter1++;
			      }else if (counter1 == 2){
			      	exitDate->year = atoi(token1);
			      	//counter1++;
			      }
			      counter1++;
			      token1 = strtok(NULL,"-");
			   	}
	   			c = getchar();
	   			int coun = 0;
	   			if(c == '\n'){
					for(i= 1; i<=numWorkers; i++){
						//printf("%p\n",array[i] );
						Work_countries* head1 = array[i];
						while(head1!= NULL){							
							Work_dates *head2 = head1->wd;
							while(head2!= NULL){
								Work_diseases* head3 = head2->wds;
								while(head3!= NULL){ //gia kathe disease
									//printf("	%s\n",head3->dis );
									if (strcmp(head3->dis,virusName) == 0){
		   								if(compareDates(entryDate,head2->filedate) == 1 && compareDates(head2->filedate, exitDate) == 1){
		   									coun = coun + head3->total;
		   								}
		   							}
									head3 = head3->next_disease;
								} 
								head2 = head2->next_date;
							} 
							head1 = head1->next_country;
						}
					}
		   			printf("%d\n",coun);
		   		}
	   			else if (c == ' '){
	   				char* country = malloc(sizeof(char)*32);
	   				scanf("%s",country);
	   				for(i= 1; i<=numWorkers; i++){
						Work_countries* head1 = array[i];
						while(head1!= NULL){
								Work_dates *head2 = head1->wd;
								while(head2!= NULL){
									Work_diseases* head3 = head2->wds;
									while(head3!= NULL){ 
										if (strcmp(head3->dis,virusName) == 0){
			   								if(compareDates(entryDate,head2->filedate) == 1 && compareDates(head2->filedate, exitDate) == 1){
			   									if(strcmp(head1->country,country) == 0)
			   										coun = coun + head3->total;
			   								}
			   							}
										head3 = head3->next_disease;
									} 
									head2 = head2->next_date;
								} 
							head1 = head1->next_country;
						}
					}
		   			printf("%d\n",coun);
	   			}
	   		}
	   	}
	   	else if(strcmp(ch,"/topk-AgeRanges") == 0){
	   		total++;
	   		int range1 = 0, range2 = 0 , range3 = 0 , range4 = 0;
	   		char* k_s = malloc(sizeof(char)*10);
	   		int k;
	   		char* country = malloc(sizeof(char)*32);
	   		char* disease = malloc(sizeof(char)*32);
	   		char* entrydate = malloc(sizeof(char)*32);
	   		char* exitdate = malloc(sizeof(char)*32);
	   		Date* entryDate = malloc(sizeof(Date));
	   		Date* exitDate = malloc(sizeof(Date));
	   		getchar(); //space
	   		scanf("%s",k_s);
	   		k = atoi(k_s);
	   		getchar();
	   		scanf("%s",country);
	   		getchar();
	   		scanf("%s",disease);
	   		getchar();
	   		scanf("%s",entrydate);
	   		getchar();
	   		scanf("%s",exitdate);
	   		c = getchar();
	   		if (c == '\n'){
	   			char* token = strtok(entrydate,"-");
			    int counter = 0;
			    while( token != NULL ) {
			      if (counter == 0){
			      	entryDate->day = atoi(token);
			      	counter++;
			      }
			      else if (counter == 1){
			      	entryDate->month = atoi(token);
			      	counter++;
			      }else{
			      	entryDate->year = atoi(token);
			      	counter++;
			      }
			      token = strtok(NULL,"-");
			  	 }
			    char* token1 = strtok(exitdate,"-");
			    int counter1 = 0;
			    while( token1 != NULL ){
			      if (counter1 == 0){
			      	exitDate->day = atoi(token1);
			      	//counter1++;
			      }
			      else if (counter1 == 1){
			      	exitDate->month = atoi(token1);
			      	//counter1++;
			      }else if (counter1 == 2){
			      	exitDate->year = atoi(token1);
			      	//counter1++;
			      }
			      counter1++;
			      token1 = strtok(NULL,"-");
			   	}
			    for(i= 1; i<=numWorkers; i++){ // for each worker
				Work_countries* head1 = array[i];
				while(head1!= NULL){  //gia kathe xwra
						Work_dates *head2 = head1->wd;
						while(head2!= NULL){ //gia kathe arxeio ths xwras
							Work_diseases* head3 = head2->wds;
							while(head3!= NULL){  //gia kathe astheneia ths xwras
								if(strcmp(head1->country,country) == 0 && strcmp(head3->dis,disease) == 0){
									range1 = range1 + head3->first_range;
									range2 = range2 + head3->sec_range;
									range3 = range3 + head3->third_range;
									range4 = range4 + head3->fourth_range;
								}
								head3 = head3->next_disease;
							} 
							head2 = head2->next_date;
						} 
					head1 = head1->next_country;
					}
				}
				//create the list 
				Range_list* head = NULL;
				Range_list* node1 = malloc(sizeof(Range_list));
				Range_list* node2 = malloc(sizeof(Range_list));
				Range_list* node3 = malloc(sizeof(Range_list));
				Range_list* node4 = malloc(sizeof(Range_list));
				node1->value = range1;
				node2->value = range2;
				node3->value = range3;
				node4->value = range4;
				node1->type = 1;
				node2->type = 2;
				node3->type = 3;
				node4->type = 4;
				add_firstRange(&head,node1);
				add_firstRange(&head,node2);
				add_firstRange(&head,node3);
				add_firstRange(&head,node4);
				int total = range1 + range2 + range3 + range4;
				sort_range(head);
				printf("for %s in %s \n",disease,country );
				int cn = 0;
				while(cn!=k){
					if(head->type == 1)
						printf("0-20 %d%%\n", (head->value*100)/total);
					else if (head->type == 2)
						printf("21-40 %d%%\n",(head->value*100)/total);
					else if (head->type == 3)
						printf("41-60 %d%%\n",(head->value*100)/total);
					else if (head->type == 4)
						printf("60+ %d%%\n",(head->value*100)/total);
					cn++;
					head = head->next;
				}
			}
	   	}
	   	else if(strcmp(ch,"/numPatientAdmissions") == 0){
	   		total++;
	   		c = getchar();
	   		if (c == ' '){
	   			char* disease = malloc(sizeof(char)*32);
	   			char* entrydate = malloc(sizeof(char)*32);
	   			char* exitdate = malloc(sizeof(char)*32);
	   			Date* entryDate = malloc(sizeof(Date));
	   			Date* exitDate = malloc(sizeof(Date));
	   			scanf("%s",disease);
	   			getchar();
	   			scanf("%s",entrydate);
	   			getchar();
	   			scanf("%s",exitdate);
	   			char* token = strtok(entrydate,"-");
			    int counter = 0;
			    while( token != NULL ) {
			      if (counter == 0){
			      	entryDate->day = atoi(token);
			      	counter++;
			      }
			      else if (counter == 1){
			      	entryDate->month = atoi(token);
			      	counter++;
			      }else{
			      	entryDate->year = atoi(token);
			      	counter++;
			      }
			      token = strtok(NULL,"-");
			   }
			    char* token1 = strtok(exitdate,"-");
			    int counter1 = 0;
			    while( token1 != NULL ){
			      if (counter1 == 0){
			      	exitDate->day = atoi(token1);
			      	//counter1++;
			      }
			      else if (counter1 == 1){
			      	exitDate->month = atoi(token1);
			      	//counter1++;
			      }else if (counter1 == 2){
			      	exitDate->year = atoi(token1);
			      	//counter1++;
			      }
			      counter1++;
			      token1 = strtok(NULL,"-");
			   	}
	   			c = getchar();
	   			if(c == '\n'){
					for(i= 1; i<=numWorkers; i++){
						Work_countries* head1 = array[i];
						while(head1!= NULL){
							int count = 0;							
							Work_dates *head2 = head1->wd;
							while(head2!= NULL){
								Work_diseases* head3 = head2->wds;
								while(head3!= NULL){ //gia kathe disease
									if (strcmp(head3->dis,disease) == 0){
		   								if(compareDates(entryDate,head2->filedate) == 1 && compareDates(head2->filedate, exitDate) == 1){
		   									count = count + head3->total;
		   								}
		   							}
									head3 = head3->next_disease;
								} 
								head2 = head2->next_date;
							} 
							printf("%s %d\n",head1->country,count);
							head1 = head1->next_country;
						}
					}
		   		}
	   			else if (c == ' '){
	   				int coun = 0;
	   				char* country = malloc(sizeof(char)*32);
	   				scanf("%s",country);
	   				for(i= 1; i<=numWorkers; i++){
						Work_countries* head1 = array[i];
						while(head1!= NULL){
								Work_dates *head2 = head1->wd;
								while(head2!= NULL){
									Work_diseases* head3 = head2->wds;
									while(head3!= NULL){ 
										if (strcmp(head3->dis,disease) == 0){
			   								if(compareDates(entryDate,head2->filedate) == 1 && compareDates(head2->filedate, exitDate) == 1){
			   									if(strcmp(head1->country,country) == 0){
			   										coun = coun + head3->total;
			   									}
			   								}
			   							}
										head3 = head3->next_disease;
									} 
									head2 = head2->next_date;
								} 
							head1 = head1->next_country;
						}
					}
		   			printf("%s %d\n",country,coun);
	   			}
	   		}
	   	}
	   	else if(strcmp(ch,"/searchPatientRecord") == 0){
	   		total++;
	   		//stelnw ston patera 1
	   		int j, bytes2 , fd3, fd4;
	   		getchar();
	   		char * recordID = malloc(sizeof(char)*30);
	   		scanf("%s",recordID);
	   		//int rec = atoi(recordID);
	   		//printf("%d\n",rec );
	   		c = getchar();
	   		if (c == '\n'){
		   		for(j =1; j<=numWorkers; j++){
	   				char* buf4 = malloc(sizeof(char)*15);
	   	    		snprintf(buf4, 15, "rec_fifo%d", j);
		   			if ((fd3 = open(buf4, O_WRONLY ,PERMS)) == -1){
						perror("opening pipe in parent");
						exit(EXIT_FAILURE);
					}

					int key = 1;
					write(fd3,&key,sizeof(int));

					int len = strlen(recordID); //mhkos aftounou poy thelw na steilw
					if ((bytes2 = write(fd3,&len,sizeof(int))) == -1){
						perror("write pipe");
						exit(EXIT_FAILURE);
					}
					if ((bytes2 = write(fd3,recordID,len)) == -1){
						perror("write pipe");
						exit(EXIT_FAILURE);

					}  
					//close(fd3);
					///printf("fd father %d\n",fd3);
					 //diavazw apo paidi
					char buf5[15];
					snprintf(buf5, 15, "send_fifo%d", j);
					if ((fd4 = open(buf5, O_RDONLY ,PERMS)) == -1){
						perror("opening pipe in parent");
						exit(EXIT_FAILURE);
					}
					int bytestoread, val2;
					int found = 0;
					bytestoread = read(fd4,&found,sizeof(int));
					char* buf_send;
					if (found == 1){
						buf_send = malloc(sizeof(char)*1024);
						memset(buf_send, '\0', 1024);

						bytestoread = read(fd4,&val2,sizeof(int));
						bytestoread = read(fd4,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send);


						bytestoread = read(fd4,&val2,sizeof(int));
						bytestoread = read(fd4,buf_send,val2); 
						buf_send[bytestoread]='\0';
						if (found == 1)
							printf("%s ",buf_send );

						bytestoread = read(fd4,&val2,sizeof(int));
						bytestoread = read(fd4,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send );

						bytestoread = read(fd4,&val2,sizeof(int));
						bytestoread = read(fd4,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send );

						int age; 
						bytestoread = read(fd4,&age,sizeof(int));
						buf_send[bytestoread]='\0';
						if (found == 1)
							printf("%d ",age);

						int day1,month1,year1;
						bytestoread = read(fd4,&day1,sizeof(int));
						bytestoread = read(fd4,&month1,sizeof(int));
						bytestoread = read(fd4,&year1,sizeof(int));
						if (found == 1)
							printf("%d-%d-%d ",day1,month1,year1 );

						int day2,month2,year2;
						bytestoread = read(fd4,&day2,sizeof(int));
						bytestoread = read(fd4,&month2,sizeof(int));
						bytestoread = read(fd4,&year2,sizeof(int));
						if (found == 1)
							if (year2 == 0)
								printf("---\n");
							else
								printf("%d-%d-%d\n",day2,month2,year2 );
						//close(fd3);
						//close(fd4);
						free(buf_send);
					}
					else{
						//free(buf_send);
						//close(fd3);
						close(fd4);
						continue;
					}
					//free(buf_send);
	   			}
	   			//close(fd3);
	   			//close(fd4);
				//free(buf_send);

	   		} 
	   	}
	    else if(strcmp(ch,"/numPatientDischarges") == 0){
	    	total++;
	    	int len;
	   		char* diseaseId = malloc(sizeof(char)*30);
	   		char* entrydate = malloc(sizeof(char)*30);
	   		char* exitdate = malloc(sizeof(char)*30);
	   		getchar();
	   		scanf("%s",diseaseId);
	   		getchar();
	   		scanf("%s",entrydate);
	   		getchar();
	   		scanf("%s",exitdate);
	   		c = getchar();
	   		if (c == ' '){// country
	   			char* country = malloc(sizeof(char)*30);
	   			scanf("%s",country);
	   			int j;
	   			for(j=1;j<= numWorkers;j++){
	   				char buf5[15];
					snprintf(buf5, 15, "rec_fifo%d", j);
					if ((fd5 = open(buf5, O_WRONLY ,PERMS)) == -1){
						perror("opening pipe in parent");
						exit(EXIT_FAILURE);
					}
					//4 write
					int key = 2;
					write(fd5,&key,sizeof(int));

					int len = strlen(diseaseId);
					write(fd5,&len,sizeof(int));
					write(fd5,diseaseId,len);
					
					len = strlen(entrydate);
					write(fd5,&len,sizeof(int));
					write(fd5,entrydate,len);

					len = strlen(exitdate);
					write(fd5,&len,sizeof(int));
					write(fd5,exitdate,len);

					len = strlen(country);
					write(fd5,&len,sizeof(int));
					write(fd5,country,len);
	   			}
	   		}
	   		else{ //no country for old men
	   			char* country = malloc(sizeof(char)*30);
	   			//scanf("%s",country);
	   			strcpy(country,"error");
	   			int j;
	   			for(j=1;j<= numWorkers;j++){
	   				char buf5[15];
					snprintf(buf5, 15, "rec_fifo%d", j);
					if ((fd5 = open(buf5, O_WRONLY ,PERMS)) == -1){
						perror("opening pipe in parent");
						exit(EXIT_FAILURE);
					}
					//4 write
					int key = 2;
					write(fd5,&key,sizeof(int));

					int len = strlen(diseaseId);
					write(fd5,&len,sizeof(int));
					write(fd5,diseaseId,len);
					
					len = strlen(entrydate);
					write(fd5,&len,sizeof(int));
					write(fd5,entrydate,len);

					len = strlen(exitdate);
					write(fd5,&len,sizeof(int));
					write(fd5,exitdate,len);

					len = strlen(country);
					write(fd5,&len,sizeof(int));
					write(fd5,country,len);
	   			}	   			
	   		}
	   }
	   else{
	   	fail++;
	   }
		if(orderds_end == -1){ //received signal
			List* perm1 = head2;
			printf("received SIGINT/SIGQUIT\n");
			create_lofFiles(perm1,total,success,fail);
			int i;
			for(i=1;i<=numWorkers; i++){
					kill(proc_id[i],SIGKILL);
					char* buf1 = malloc(sizeof(char)*15);
			   		char* buf2 = malloc(sizeof(char)*15);
			   	    snprintf(buf1, 15, "rec_fifo%d", i);
			   		snprintf(buf2, 15, "send_fifo%d", i);
			   		unlink(buf1);
			   		unlink(buf2);
			   		free(buf1);
			   		free(buf2);
			}
			exit(0);
		}
	   	scanf("%s", ch);
   	}
   	if(orderds_end == -1){ //received signal
		printf("received SIGINT/SIGQUIT\n");
		List* perm = head2;
		create_lofFiles(perm,total,success,fail);
		exit(0);
	}
   	printf("exiting \n");
   	for(i=1;i<=numWorkers;i++)
   		kill(proc_id[i], SIGKILL);
    List* perm = head2;
    success = total-fail;
    create_lofFiles(perm,total,success,fail);
	for(i=1;i<=numWorkers;i++){ //unlink the fifos
		char* buf1 = malloc(sizeof(char)*15);
   		char* buf2 = malloc(sizeof(char)*15);
   	    snprintf(buf1, 15, "rec_fifo%d", i);
   		snprintf(buf2, 15, "send_fifo%d", i);
   		unlink(buf1);
   		unlink(buf2);
   		free(buf1);
   		free(buf2);
	}
	free(input_dir);
	//deallocate the memory
	for (int i = 1;i <=numWorkers; ++i){
		Work_countries* head = array[i];
		deleteListWC(&head);
		
	}
	deleteLList(&head2);
	free(ch);
	return 1;
}