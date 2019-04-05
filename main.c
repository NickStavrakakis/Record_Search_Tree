//ROOT_NODE
#include "header.h"
#include "mytime.h"

unsigned int s_called=0;

void signal_handler(int signal){ //counting how many searchers nodes exist
    s_called++;
}

int main(int argc, char *argv[]){

	clock_t start_t = clock(), end_t;
	double total_t;
    int ret_val, fLength, rNumber, height, rNumber_new=0;
    char s_option[2] = "n"; //-s option flag
    int fd; /*For open pipes*/
    int status; /*Used for waiting for processes*/
    char filename[40]; //string for Input File's name
    char pattern[40]; //string for Pattern Name
    char pipe[30], root[30], start[SIZEofBUFF], finish[SIZEofBUFF];
    FILE *fpb; //file pointer for Input File
    FILE *pFile; //file pointer for Outpute File (to sort it later)
    Record rec;
    MyTime mytime;
    //getting the arguments for the terminal
    if( (argc == 7) || (argc == 8) ){
        if(strcmp(argv[1], "-h")==0){
            height = atoi(argv[2]);
            if(height<0){
                printf("Invalid height\n");
                exit(0);
            }
            else if(height>5){
                printf("Height too big\n");
                exit(0);
            }
        }
        else{
            printf("Argument 1 is wrong\n");
            exit(0);
        }
        if(strcmp(argv[3], "-d")==0){
            strcpy(filename, argv[4]);
        }
        else{
            printf("Argument 3 is wrong\n");
            exit(0);
        }
        if(strcmp(argv[5], "-p")==0){
            strcpy(pattern, argv[6]);
        }
        if(argc==8){
            if(strcmp(argv[7], "-s")==0){
                strcpy(s_option, "y");
            }
        }
    }
    else{
        printf("Wrong arguments\n");
        exit(0);
    }

    //create pipe for the root process
    sprintf(pipe, "root_%d", getpid()); //with root name for easy deletion at the end of program //same for childs
    sprintf(root, "%d", getpid()); //for signal use
    ret_val = mkfifo(pipe, 0666);
    if(ret_val == -1){
        printf("error creating the pipe\n");
        exit(0);
    }

    // Opening the Input File and
    // getting informations for the records
    fpb = fopen(filename, "rb" );
    if(fpb == NULL){
        printf("cannot open input file\n");
        exit(0);
    }
    fseek(fpb, 0, SEEK_END);
    fLength = ftell (fpb);
    rewind (fpb);
    rNumber= (int) fLength/sizeof(Record);
    fclose(fpb);

    //converting to string to send them trough the exec's
    sprintf(start, "%d", 0);
    sprintf(finish, "%d", rNumber);

    // Opening the Output File
    pFile = fopen("output", "w");
    if(pFile == NULL){
        printf("cannot create output file\n");
        exit(0);
    }

    //going to the top splitter-merger
    switch (fork()) {
        case -1:
            printf("fork failed\n");
            exit(0);
        case 0:
            execlp("./splitter_merger_node", "splitter_merger_node", filename, start, finish, pattern, argv[2], pipe, root, s_option, argv[2], finish, (char*)NULL);
        default:
            //signal job
            signal(SIGUSR2, signal_handler);

            //getting for the pipe and writing to the output file
            fd = open(pipe, O_RDONLY);
            read(fd, &rec, sizeof(Record));
            while((rec.custid)!=-1){
                rNumber_new++;
                fprintf(pFile, "%ld %s %s  %s %d %s %s %-9.2f\n", rec.custid, rec.LastName, rec.FirstName, rec.Street, rec.HouseID, rec.City, rec.postcode, rec.amount);
                read(fd, &rec, sizeof(Record));
            }
            read(fd, &mytime, sizeof(MyTime)); //getting also the times
            break;
    }
    wait(&status);
	close(fd);
    remove(pipe);
    fclose(pFile);

    printf("\nTotal Results: %d\n", rNumber_new);

    //going to the sort node
    switch (fork()) {
        case -1:
            printf("fork failed\n");
            exit(0);
        case 0:
            printf("\n");
            execlp("sort", "sort", "-n", "-k", "1", "output", (char*)NULL);
        default:
            wait(&status);
            break;
    }
    remove("output");

    //Printing the Statistics
    printf("\nSearchers Called: %d \n", s_called);
    PrintTimes(mytime);
    end_t = clock();
    total_t = (double)(end_t-start_t)/CLOCKS_PER_SEC;
    printf("\nTurnaroud Time: %f\n", total_t);

	return 0;
}
