//SPLITTER-MERGER NODE
#include "header.h"
#include "mytime.h"

int main(int argc, char *argv[]){

	/*
	argv[1] = filename
	argv[2] = start 	| 	argv[3] = finish
	argv[4] = pattern	|	argv[5] = current height
	argv[6] = pipe		|	argv[7] = root
	argv[8] = -s flag	|	argv[9] = tree height
	argv[10] = Total Records
	*/

	clock_t start_t, end_t;
	double total_t;
	int fd,fd2,fd3, fd4, FD; //1 and 2 stants for the two leaf_nodes leafs pipes and 3 and 4 for the two splitters-mergers nodes
    int start = atoi(argv[2]), finish = atoi(argv[3]);
    int rNumber_a, rNumber_b, width = finish - start;
	int ret_val, status;
	char new_height[30];
	char start_a_str[30], finish_a_str[30], start_b_str[30], finish_b_str[30]; //a stands for left child, b stans for rigt child
	char pipe1[20],pipe2[20],pipe3[20],pipe4[20];
    Record rec;
	MyTime mytime, mytime1, mytime2, mytime3, mytime4;

	//check if we have an even number of record in that node
	//and configuring the right times
    if(width % 2 != 1){
        rNumber_a = width/2;
        rNumber_b = width/2;
    }
    else{
        rNumber_a = width/2;
        rNumber_b = width/2+1;
    }
    sprintf(start_a_str, "%d", start);
    sprintf(finish_a_str, "%d", start+rNumber_a);
    sprintf(start_b_str, "%d", start+rNumber_a);
    sprintf(finish_b_str, "%d", start+rNumber_a+rNumber_b);

	//connecting trough this pipe to the parent
	FD = open(argv[6], O_WRONLY);

	//if we are in a splitter-merger node
    if(atoi(argv[5])>1){

		//create pipe for the left sm process
        sprintf(pipe3, "a_%d", getpid());
        ret_val = mkfifo(pipe3, 0666);
        if(ret_val == -1){
            printf("error creating the pipe\n");
			exit(0);
        }

		sprintf(new_height, "%d", atoi(argv[5])-1);
		start_t = clock();
        switch (fork()) {
            case -1:
                printf("fork failed\n");
				exit(0);
            case 0:
                execlp("./splitter_merger_node", "splitter_merger_node", argv[1], start_a_str, finish_a_str, argv[4], new_height, pipe3, argv[7], argv[8], argv[9], argv[10], (char*)NULL);
				break;
            default:
				//create pipe for the right sm process
				sprintf(pipe4, "b_%d", getpid());
                ret_val = mkfifo(pipe4, 0666);
                if(ret_val == -1){
                    printf("error creating the pipe\n");
					exit(0);
                }

                switch (fork()) {
                    case -1:
                        printf("fork failed\n");
						exit(0);
                    case 0:
						execlp("./splitter_merger_node", "splitter_merger_node", argv[1], start_b_str, finish_b_str, argv[4], new_height, pipe4, argv[7], argv[8], argv[9], argv[10], (char*)NULL);
						break;
                    default:
						end_t = clock();

                        fd3 = open(pipe3, O_RDONLY);
                        read(fd3, &rec, sizeof(Record));
                        while((rec.custid)!=-1){
                            write(FD, &rec, sizeof(Record));
                            read(fd3, &rec, sizeof(Record));
                        }
						read(fd3, &mytime3, sizeof(MyTime));

                        fd4 = open(pipe4, O_RDONLY);
                        read(fd4, &rec, sizeof(Record));
                        while((rec.custid)!=-1){
                            write(FD, &rec, sizeof(Record));
                            read(fd4, &rec, sizeof(Record));
                        }
						read(fd4, &mytime4, sizeof(MyTime));

						rec.custid = -1;
						write(FD, &rec, sizeof(Record));

						total_t = (double)(end_t-start_t)/CLOCKS_PER_SEC;
						SetTime(&mytime, &mytime3, &mytime4, total_t, 2);
						write(FD, &mytime, sizeof(MyTime));
                        wait(&status);
                        wait(&status);

						close(fd3);
                        close(fd4);
                        remove(pipe3);
                }
                remove(pipe4);
        }
    }
    else { // if (height == 1) then call leaf_node
		//calculating the right search breadth if -s enabled
		if(strcmp(argv[8], "y")==0){
			int i, j, breadth_a, breadth_b, start_a, sum;
			double final_cut = (double)(atoi(argv[10])/pow(2.0, atoi(argv[9])));
			for(i = 0; i<pow(2.0, atoi(argv[9])); i = i+2){
				if(atoi(start_a_str) == (int)(i*final_cut))
					break;
			}

			sum = 0;
			for(j=1; j<=pow(2.0, atoi(argv[9])); j++){
				sum = sum + j;
			}

			breadth_a = (int)(((double)(i+1))/((double)sum)*atoi(argv[10]));
			breadth_b = (int)(((double)(i+2))/((double)sum)*atoi(argv[10]));
			start_a = 0;
			for(j=0; j<=i; j++){
				start_a = (int)(((double)(j))/((double)sum)*atoi(argv[10])) + start_a;
			}

			sprintf(start_a_str, "%d", start_a);
		    sprintf(finish_a_str, "%d", atoi(start_a_str) + breadth_a);
		    strcpy(start_b_str, finish_a_str);
			if(i == (pow(2.0, atoi(argv[9]))-2))
				strcpy(finish_b_str, argv[10]);
			else
		    	sprintf(finish_b_str, "%d",  atoi(start_b_str) + breadth_b);
		}

		//create pipe for the left leaf_node process
        sprintf(pipe1, "a_%d", getpid());
        ret_val = mkfifo(pipe1, 0666);
        if(ret_val == -1){
            printf("error creating the pipe\n");
			exit(0);
        }

        switch(fork()){
            case -1:
                printf("fork failed\n");
				exit(0);
            case 0:
                execlp("./leaf_node", "leaf_node", argv[1], start_a_str, finish_a_str, argv[4], pipe1, argv[7], (char*)NULL);
                break;
            default:
				//create pipe for the right leaf_node process
                sprintf(pipe2, "b_%d", getpid());
                ret_val = mkfifo(pipe2, 0666);
                if(ret_val == -1){
                    printf("error creating the pipe\n");
					exit(0);
                }

                switch (fork()) {
                    case -1:
                        printf("fork failed\n");
						exit(0);
                    case 0:
						execlp("./leaf_node", "leaf_node", argv[1], start_b_str, finish_b_str, argv[4], pipe2, argv[7], (char*)NULL);
						break;
                    default:
                        fd = open(pipe1, O_RDONLY);

                        read(fd, &rec, sizeof(Record));
                        while((rec.custid)!=-1){
                            write(FD, &rec, sizeof(Record));
                            read(fd, &rec, sizeof(Record));
                        }
						read(fd, &mytime1, sizeof(MyTime));

                        fd2 = open(pipe2, O_RDONLY);
                        read(fd2, &rec, sizeof(Record));
                        while((rec.custid)!=-1){
                            write(FD, &rec, sizeof(Record));
                            read(fd2, &rec, sizeof(Record));
                        }
						read(fd2, &mytime2, sizeof(MyTime));

						rec.custid = -1;
						write(FD, &rec, sizeof(Record));

						SetTime(&mytime, &mytime1, &mytime2, 0, 1);
						write(FD, &mytime, sizeof(MyTime));
                        wait(&status);
                        wait(&status);

                        close(fd);
						close(fd2);
                        remove(pipe1);
                }
				remove(pipe2);
        }
    }

    return 0;
}
