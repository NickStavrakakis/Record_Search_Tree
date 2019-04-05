#include <stdio.h>

typedef struct{
	double tmp;
	double min_sm_time;
	double max_sm_time;
	double avg_sm_time;
	double min_s_time;
	double max_s_time;
	double avg_s_time;
} MyTime;

void PrintTimes(MyTime mytime){

	printf("\nMin time for Searchers: %f\nMax time for Searchers: %f\nAvg time for Searchers: %f\n", mytime.min_s_time, mytime.max_s_time, mytime.avg_s_time);
	printf("\nMin time for Splitters-Mergers: %f\nMax time for Splitters-Mergers: %f\nAvg time for Splitters-Mergers: %f\n", mytime.min_sm_time, mytime.max_sm_time, mytime.avg_sm_time);

}

void SetMinMax(MyTime *mytime, double time, int flag){

	if(flag == 1){ //1 = searcher times
		//to check if a double value iz zero
		char s_temp[30]; sprintf(s_temp, "%.6f", mytime->min_s_time);
		if(strcmp(s_temp, "0.000000")==0) //to check if a double value iz zero
			mytime->min_s_time = time;
		else if(time < mytime->min_s_time)
			mytime->min_s_time = time;
		if(time > mytime->max_s_time)
			mytime->max_s_time = time;
	}
	else{ //if(flag == 2) //2 = splitter-merger times
		//to check if a double value iz zero
		char sm_temp[30]; sprintf(sm_temp, "%.6f", mytime->min_sm_time);
		if(strcmp(sm_temp, "0.000000")==0)
			mytime->min_sm_time = time;
		else if(time < mytime->min_sm_time)
			mytime->min_sm_time = time;
		if(time > mytime->max_sm_time)
			mytime->max_sm_time = time;
	}
}

void SetAvg(MyTime *mytime, double time1, double time2, double total_t, int flag){

	if(flag == 1) //1 = searcher times
		mytime->avg_s_time = (time1 + time2)/2;
	else{ //if(flag == 2) //2 = splitter-merger times

		if( (time1 != 0) && (time2 != 0) )
			mytime->avg_sm_time = (time1 + time2 + total_t)/3;
		else if( (time1 == 0) && (time2 != 0) )
			mytime->avg_sm_time = (time2 + total_t)/2;
		else if( (time1 != 0) && (time2 == 0) )
			mytime->avg_sm_time = (time1 + total_t)/2;
		else
			mytime->avg_sm_time = total_t;
	}
}

void ConfigureTime(MyTime *mytime, MyTime mytime1, MyTime mytime2){

	if(mytime1.min_s_time < mytime2.min_s_time)
		mytime->min_s_time = mytime1.min_s_time;
	else
		mytime->min_s_time = mytime2.min_s_time;

	if(mytime1.max_s_time > mytime2.max_s_time)
		mytime->max_s_time = mytime1.max_s_time;
	else
		mytime->max_s_time = mytime2.max_s_time;

	//

	if(mytime1.min_sm_time < mytime2.min_sm_time)
		mytime->min_sm_time = mytime1.min_sm_time;
	else
		mytime->min_sm_time = mytime2.min_sm_time;

	if(mytime1.max_sm_time > mytime2.max_sm_time)
		mytime->max_sm_time = mytime1.max_sm_time;
	else
		mytime->max_sm_time = mytime2.max_sm_time;

}

void SetTime(MyTime *mytime, MyTime *mytime1, MyTime *mytime2, double total_t, int flag){
	if(flag == 1){ //1 = searcher times
		SetMinMax(mytime1, mytime1->tmp, 1);
		SetMinMax(mytime2, mytime2->tmp, 1);
		ConfigureTime(mytime, *mytime1, *mytime2);
		SetAvg(mytime, mytime1->tmp, mytime2->tmp, 0, 1);
	}
	else{ //if(flag == 2) //2 = splitter-merger times
		SetMinMax(mytime1, total_t, 2);
		SetMinMax(mytime2, total_t, 2);
		ConfigureTime(mytime, *mytime1, *mytime2);
		SetAvg(mytime, mytime1->avg_s_time, mytime2->avg_s_time, 0, 1);
		SetAvg(mytime, mytime1->avg_sm_time, mytime2->avg_sm_time, total_t, 2);
	}
}
