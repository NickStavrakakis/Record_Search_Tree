//LEAF NODE
#include "header.h"
#include "mytime.h"

int compare(Record rec, char pattern[]);

int main(int argc, char *argv[]){

	/*
	argv[1] = filename
	argv[2] = start		|	argv[3] = finish
	argv[4] = pattern	|	argv[5] = pipe
	argv[6] = root
	*/

	clock_t start_t, end_t;
	double total_t;
	start_t = clock();
    FILE *fpb;
	int fd, i, j;
    int start = atoi(argv[2]), finish = atoi(argv[3]);
	Record rec;
	MyTime mytime;

    fpb = fopen(argv[1], "rb" );
    if(fpb == NULL){
        printf("cannot open input file\n");
        exit(0);
    }

    fd = open(argv[5], O_WRONLY);

    //go the right record to start searching records
    for(i=0; i<start; i++){
        fread(&rec, sizeof(Record), 1, fpb);
    }

    //start checking records
    for(j=i; j<finish; j++){
        fread(&rec, sizeof(Record), 1, fpb);
		if(compare(rec, argv[4])==0){
			write(fd, &rec, sizeof(Record));
        }
    }

    rec.custid=-1;
    write(fd, &rec, sizeof(Record));
	fclose(fpb);

	end_t = clock();
	mytime.tmp = (double)(end_t-start_t)/CLOCKS_PER_SEC;
	write(fd, &mytime, sizeof(MyTime));
	close(fd);

	kill(atoi(argv[6]), SIGUSR2);

    return 0;
}

int compare(Record rec, char pattern[30]){

	if(rec.custid == atoi(pattern)){
		return 0;
	}
	else if(strcmp(rec.FirstName, pattern)==0){
		return 0;
	}
	else if(strcmp(rec.LastName, pattern)==0){
		return 0;
	}
	else if(strcmp(rec.Street, pattern)==0){
		return 0;
	}
	else if(rec.HouseID == atoi(pattern)){
		return 0;
	}
	else if(strcmp(rec.City, pattern)==0){
		return 0;
	}
	else if(strcmp(rec.postcode, pattern)==0){
		return 0;
	}
	else if(rec.amount == atof(pattern)){
		return 0;
	}

	return 1;
}
