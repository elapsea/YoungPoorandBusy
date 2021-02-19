#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXCONN 2000+1
#define MAXCITY 100
#define INFINITE 999999

struct train {
	int from,to;
	int dpt,arv;
	int fare;
} trains[MAXCONN],rtrains[MAXCONN];

int from_dpt[MAXCITY][MAXCONN], /* fr_h */
    from_arv[MAXCITY][MAXCONN],/* fr_t */
    to_dpt[MAXCITY][MAXCONN], /* to_h */
    to_arv[MAXCITY][MAXCONN]; /* to_t */

int Timeline[MAXCONN*2+2];	//the array of time
char city[MAXCITY][20];		//the array of city
int NumOfTrains;			//the total number of cities
int NumOfCity;				//the total number of cities
char filenumber[3]="";		//the number of txt

void init();			//initialize global variables
int ConvertedTime(char *s);			//convert string-time into int-time
int FindCity(char *key);	 		//insert and return the number of city
void InsertTime(int key);	 		//insert the number of time
void SortTime();					//sort the number of time
int TimeOrder(int key);				//return the number of time
void Sort(struct train *ptrains,int NumOfTrains,int flag);	
//flag=0,dpt from small to large,flag=1,dpt from large to small,flag=2,fare from large to small,flag=3,time from large to small.
int PositionOfMeetingtime(int val);			//return the position of the Meeting time
int min(int a,int b);				//return min(a,b)
char *ResultTime(int time);			//convert int-time into hh:mm for output
char *openfile(char *filenumber,int flag);		//flag=0 read file,flag=1,write file
void MaxFare(struct train *ptrains);		//checkpoint(1) max fare
void MaxTime(struct train *ptrains);		//checkpoint(1) max time
void ProcessConnection(struct train *ptrains,char *from,char *to,char *dpt,char *arv,char *fare,char *dpttime,char *arvtime);	//prepare train data
void make_table(int v[MAXCITY][MAXCONN],int cityID,struct train *ptrains,int NumOfTrains,int NumOfdptTime,int NumOfarvTime,int flag);	
//use Dijkstra algorithm to make the min_cost_table
int Cal_min_cost(struct train *ptrains,struct train *prtrains,int NumOfTrains,char *dptstation,char *arvstation,char *dpttime,char *arvtime,int meettime);	
//checkpoint(4) calculate the minimum cost

int main(void) {
	char dptstation[20],arvstation[20],dpttime[5],arvtime[5];
	int n,meettime;

	while(1) {
		FILE *fp1,*fp2;
		int i,j,k;
		int max_word = 20+5*2000;			//read up to (20+5*2000)words
		char words[max_word][21]; 		//no more than 20 characters per word
		printf("the number of file please (1/2/3/4): ");
		scanf("%s",&filenumber);
		if (strcmp(filenumber, "0") == 0)		//input 0 to exit
			break;
		char *read=openfile(filenumber,0);
		fp1 = fopen(read, "r");
		if(fp1 == NULL) {
			printf("open file error\n");
			return -1;
		}
		for(i=0; i<max_word; i++ ) { 
			if(fscanf(fp1, "%20s",words[i] ) != 1) 	//read file
				break;
		}
		fclose(fp1);
		init();
		n = atoi(words[4]);			//checkpoint(5) by reading file
		strcpy(dptstation, words[6]);
		strcpy(arvstation, words[8]);
		strcpy(dpttime, words[10]);
		strcpy(arvtime, words[12]);
		meettime = atoi(words[14]);
		for(i = 0; i < n; i++) {	//prepare_data
			k = 15+i*5;
			ProcessConnection(trains,words[k],words[k+2],words[k+1],words[k+3],words[k+4],dpttime,arvtime);
		}
		InsertTime(ConvertedTime(dpttime));
		InsertTime(ConvertedTime(arvtime));
		SortTime();
		char *write=openfile(filenumber,1);
		fp2=fopen(write, "w+");
		fputs("The available train information : \n", fp2);
		for(i = 0; i<NumOfTrains; i++) {	//number the time
			trains[i].dpt = TimeOrder(trains[i].dpt);	//transform the departure int-time to the number of departure time
			trains[i].arv = TimeOrder(trains[i].arv);	//transform the arrival int-time to the number of arrival time
			fprintf(fp2, "%s  %s  %s  ",city[trains[i].from],city[trains[i].to],ResultTime(Timeline[trains[i].dpt]));		//checkpoint(1) max fare
			fprintf(fp2, "%s  %d  \n",ResultTime(Timeline[trains[i].arv]),trains[i].fare);  
		}
		fprintf(fp2,"\n");
		fclose(fp2);
		for(i = 0; i < NumOfTrains; i++) {		//prepare rtrains
			rtrains[i].to = trains[i].from;
			rtrains[i].from = trains[i].to;
			rtrains[i].arv = trains[i].dpt;
			rtrains[i].dpt = trains[i].arv;
			rtrains[i].fare = trains[i].fare;
		}
		MaxFare(trains);		//checkpoint(1) max fare
		MaxTime(trains);		//checkpoint(1) max time
		if(Cal_min_cost(trains,rtrains,NumOfTrains,dptstation,arvstation,dpttime,arvtime,meettime))		//calculate the minimum cost
			printf("The calculation is successful.Please check the file:%s\n",write);
	}
	return 0;
}

void init() {		//initialize global variables
	int i;
	memset(trains,0,sizeof(struct train)*MAXCONN);
	memset(rtrains,0,sizeof(struct train)*MAXCONN);
	for(i = 0; i < (MAXCONN*2+2); i++)		Timeline[i] = 0;
	memset(city,0x00,sizeof(char)*(MAXCITY*20));
	NumOfTrains = 0;
	NumOfCity = 0;
}

int ConvertedTime(char *s) {	//convert string-time into int-time
	int i,hh,mm;
	char a = *s,b = *(s+1),c = *(s+3),d = *(s+4);
	return (((a-'0')*10+(b-'0'))*60+(c-'0')*10+(d-'0'));
}


int FindCity(char *key) { 		//insert and return the number of city
	int i,j;
	for(i = 0; i < MAXCITY; i++) {
		if(strlen(city[i]) == 0) {
			j = i;
			break;
		}
	}
	for(i = 0; i < j; i++) {
		if (strcmp(city[i], key) == 0)
			return i;
	}
	strcpy(city[j],key);
	NumOfCity++;
	return j;
}

void InsertTime(int key) { 		//insert the number of time
	int i,j,k = 0;
	for(i = 0; i < (MAXCONN*2+2); i++) {
		if(Timeline[i] == 0) {
			j = i;
			break;
		}
	}
	for(i = 0; i < j; i++) {
		if (Timeline[i] == key) {
			k = 1;
			break;
		}
	}
	Timeline[j] = (k == 1) ? 0 : key;
}

void SortTime() {				//sort the number of time
	int i,j,n,t;
	for(i = 0; i < (MAXCONN*2+2); i++) {
		if(Timeline[i] == 0) {
			n = i;
			break;
		}
	}
	for(i = 0; i < n-1; i++) {
		for(j=i+1; j < n; j++) {
			if(Timeline[i] > Timeline[j]) {
				t = Timeline[i];
				Timeline[i] = Timeline[j];
				Timeline[j] = t;
			}
		}
	}
}

int TimeOrder(int key) {		//return the number of time
	int i,j;
	for(i = 0; i < (MAXCONN*2+2); i++) {
		if(Timeline[i] == 0) {
			j=i;
			break;
		}
	}
	for(i = 0; i < j; i++) {
		if (Timeline[i] == key)
			return i;
	}
}

void Sort(struct train *ptrains,int NumOfTrains,int flag){
//flag=0,dpt from small to large,flag=1,dpt from large to small,flag=2,fare from large to small,flag=3,time from large to small.
	int i,j;
	struct train temp;
	for(i = 0; i < NumOfTrains-1; i++) {
		for(j = i+1; j < NumOfTrains; j++) {
			if(flag == 0) {
				if(ptrains[i].dpt > ptrains[j].dpt) {
					temp = ptrains[i];
					ptrains[i] = ptrains[j];
					ptrains[j] = temp;
				}
			} else if(flag == 1) {
				if(ptrains[i].dpt < ptrains[j].dpt) {
					temp = ptrains[i];
					ptrains[i] = ptrains[j];
					ptrains[j] = temp;
				}
			} else if(flag == 2) {
				if(ptrains[i].fare < ptrains[j].fare) {
					temp = ptrains[i];
					ptrains[i] = ptrains[j];
					ptrains[j] = temp;
				}
			} else if(flag==3) {
				if((Timeline[ptrains[i].arv]-Timeline[ptrains[i].dpt]) < (Timeline[ptrains[j].arv]-Timeline[ptrains[j].dpt])) {
					temp = ptrains[i];
					ptrains[i] = ptrains[j];
					ptrains[j] = temp;
				}
			}
		}
	}
}

int PositionOfMeetingtime(int val) {		//return the position of the value
	int i,m,r,l = 0;
	for(i = 0; i < (MAXCONN*2+2); i++) {
		if(Timeline[i] == 0) {
			r = --i;
			break;
		}
	}
	while(l<r) {
		m = (l+r)/2;
		if(val <= Timeline[m]) {
			r = m-1;
		} else {
			l = m+1;
		}
	}
	return val <= Timeline[l] ? l : l+1;
}

int min(int a,int b) {			//return min(a,b)
	return a <= b ? a : b;
}

char *ResultTime(int time) {	//convert int-time into hh:mm for output
	static char t[5];
	char symbol = ':';
	t[0] = time/60/10 + '0';
	t[1] = time/60%10 + '0';
	t[2] = symbol;
	t[3] = time%60/10 + '0';
	t[4] = time%60%10 + '0';
	return t;
}

char *openfile(char *filenumber,int flag) {	//flag=0 read file,flag=1,write file
	char *s1;
	s1 = (char *)malloc(100);
	if(flag == 0)	strcpy(s1,"E:\\YoungPoorandBusy\\code_c\\input_text\\input");
	else if(flag == 1)	strcpy(s1,"E:\\YoungPoorandBusy\\code_c\\output_text\\output");
	char s2[5] = ".txt";
	int i, j, n;
	for (i = 0; s1[i] != '\0'; i++)
		;
	n = i;
	for (j = 0; filenumber[j] != '\0'; j++) {
		s1[n] = filenumber[j];
		n = n + 1;
	}
	for (j = 0; s2[j] != '\0'; j++) {
		s1[n] = s2[j];
		n = n + 1;
	}
	s1[n] = '\0';
	return s1;
}

void MaxFare(struct train *ptrains) {	//checkpoint(1) max fare
	int i,max_fare;
	FILE *fp = NULL;
	char *write = openfile(filenumber,1);
	fp=fopen(write, "a+");
	fputs("The most expensive train : \n", fp);
	Sort(ptrains,NumOfTrains,2);
	max_fare = ptrains[0].fare;
	for(i = 0; i < NumOfTrains; i++) {
		if(ptrains[i].fare == max_fare) {
			fprintf(fp,"%s %s %s ",city[ptrains[i].from],city[ptrains[i].to],ResultTime(Timeline[ptrains[i].dpt]));
			fprintf(fp,"%s %d\n",ResultTime(Timeline[ptrains[i].arv]),ptrains[i].fare);
		}
	}
	fprintf(fp,"\n");
	fclose(fp);
}

void MaxTime(struct train *ptrains) {	//checkpoint(1) max time
	int i,max_time;
	FILE *fp = NULL;
	char *write=openfile(filenumber,1);
	fp=fopen(write, "a+");
	fputs("The most time-consuming train : \n", fp);
	Sort(ptrains,NumOfTrains,3);
	max_time = Timeline[ptrains[0].arv] - Timeline[ptrains[0].dpt];
	for(i = 0; i < NumOfTrains; i++) {
		if(Timeline[ptrains[i].arv] - Timeline[ptrains[i].dpt]==max_time) {
			fprintf(fp,"%s %s %s ",city[ptrains[i].from],city[ptrains[i].to],ResultTime(Timeline[ptrains[i].dpt]));
			fprintf(fp,"%s %d\n",ResultTime(Timeline[ptrains[i].arv]),ptrains[i].fare);
		}
	}
	fprintf(fp,"\n");
	fclose(fp);
}

void ProcessConnection(struct train *ptrains,char *from,char *to,char *dpt,char *arv,char *fare,char *dpttime,char *arvtime) {	//prepare train data
	int f=FindCity(from);		//the number of departure station
	int t=FindCity(to);			//the number of arrival station
	int d=ConvertedTime(dpt);	//the departure int-time 
	InsertTime(d);
	int a=ConvertedTime(arv);	//the arrival int-time
	InsertTime(a);
	if(d >= ConvertedTime(dpttime) &&a <= ConvertedTime(arvtime)) {
		ptrains[NumOfTrains].from = f;
		ptrains[NumOfTrains].to = t;
		ptrains[NumOfTrains].dpt = d;
		ptrains[NumOfTrains].arv = a;
		ptrains[NumOfTrains].fare = atoi(fare);
		NumOfTrains++;
	}
}

void make_table(int v[MAXCITY][MAXCONN],int cityID,struct train *ptrains,int NumOfTrains,int NumOfdptTime,int NumOfarvTime,int flag) {	//use Dijkstra algorithm to make the min_cost_table
	int i,j,k,timeID,stationID;
	int NumOfStation=NumOfCity;
	for(j = 0; j < NumOfStation; j++) for(k = 0; k <= NumOfarvTime; k++) v[j][k] = INFINITE;
	int cur = 0;
	if(flag == 1)		{
		v[cityID][NumOfdptTime] = 0;
		Sort(ptrains,NumOfTrains,0);		
	}
	else if(flag == 2)	{
		v[cityID][NumOfarvTime] = 0;
		Sort(ptrains,NumOfTrains,1);
	}
	for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
		if(flag == 1) {
			if( timeID-1 >= NumOfdptTime ) {
				for(stationID = 0; stationID < NumOfStation; stationID++) {
					v[stationID][timeID] = min(v[stationID][timeID],v[stationID][timeID-1]);
				}
			}
		} else if(flag == 2) {
			if( NumOfarvTime-timeID+1 <= NumOfarvTime ) {
				for(stationID = 0; stationID < NumOfStation; stationID++) {
					v[stationID][NumOfarvTime-timeID] = min(v[stationID][NumOfarvTime-timeID],v[stationID][NumOfarvTime-timeID+1]);
				}
			}
		}
		int num = (flag ==1 ? timeID : NumOfarvTime - timeID);
		while( cur < NumOfTrains && ptrains[cur].dpt == num ) {
			v[ptrains[cur].to][ptrains[cur].arv] = min(v[ptrains[cur].to][ptrains[cur].arv],v[ptrains[cur].from][num] + ptrains[cur].fare);
			cur++;
		}
	}
}

int Cal_min_cost(struct train *ptrains,struct train *prtrains,int NumOfTrains,char *dptstation,char *arvstation,char *dpttime,char *arvtime,int meettime) {
	//checkpoint(4) calculate the minimum cost
	int i,j,cityID,timeID;
	int dptID = FindCity(dptstation);
	int arvID = FindCity(arvstation);
	int NumOfdptTime = TimeOrder(ConvertedTime(dpttime)); 		//checkpoint(6) variable of starttime
	int NumOfarvTime = TimeOrder(ConvertedTime(arvtime)); 		//checkpoint(6) variable of starttime
	int NumOfStation=NumOfCity;
	
	make_table(to_dpt,dptID,ptrains,NumOfTrains,NumOfdptTime,NumOfarvTime,1);	//checkpoint(6)
	make_table(to_arv,arvID,ptrains,NumOfTrains,NumOfdptTime,NumOfarvTime,1);
	make_table(from_dpt,dptID,prtrains,NumOfTrains,NumOfdptTime,NumOfarvTime,2);
	make_table(from_arv,arvID,prtrains,NumOfTrains,NumOfdptTime,NumOfarvTime,2);


	int min_cost = INFINITE;	//the minimum cost
	int flag = min_cost;		//to record each min_cost
	char CityName[20];
	int StartMeeting;			//start meeting time
	int EndMeeting;				//end meeting time
	int CostOfEachStation;		//cost of each station
	int min_COES[NumOfStation];	//find min_cost per station
	for(i = 0; i < NumOfStation; i++)
		min_COES[i] = INFINITE;
	for(cityID = 0; cityID < NumOfStation; cityID++) {
		for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
			if( to_dpt[cityID][timeID] == INFINITE || to_arv[cityID][timeID] == INFINITE ) 		continue;
			if((Timeline[timeID] + meettime)>ConvertedTime(arvtime)) 	continue;
			int MeetingTimeID = PositionOfMeetingtime(Timeline[timeID] + meettime);
			if( from_dpt[cityID][MeetingTimeID] == INFINITE || from_arv[cityID][MeetingTimeID] == INFINITE ) 	continue;
			CostOfEachStation = to_dpt[cityID][timeID] + to_arv[cityID][timeID] + from_dpt[cityID][MeetingTimeID] + from_arv[cityID][MeetingTimeID];
			min_cost = min(min_cost,CostOfEachStation);
			if(CostOfEachStation < INFINITE) {
				min_COES[cityID] = min(min_COES[cityID],CostOfEachStation);	//minimum cost per station
			}
			if(flag != min_cost) {	//checkpoint(6) && record each information of min_cost
				flag = min_cost;
				int l = MeetingTimeID;
				while(flag == to_dpt[cityID][timeID] + to_arv[cityID][timeID] + from_dpt[cityID][l+1] + from_arv[cityID][l+1])	l++;	//get the number of leave
				strcpy(CityName,city[cityID]);
				StartMeeting = Timeline[timeID];
				EndMeeting = Timeline[l];
			}
		}
	}
	
	FILE *fp = NULL;
	char *write=openfile(filenumber,1);
	fp=fopen(write, "a+");
	
	fprintf(fp,"to_%s : \n",dptstation);		//checkpoint(2) output fr_h table
	for(cityID = 0; cityID < NumOfStation; cityID++) {
		for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
			fprintf(fp,"from_%s_at_%s : %d\t\t",city[cityID],ResultTime(Timeline[timeID]),from_dpt[cityID][timeID]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");

	fprintf(fp,"to_%s : \n",arvstation);		//checkpoint(2) output fr_t table
	for(cityID = 0; cityID < NumOfStation; cityID++) {
		for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
			fprintf(fp,"from_%s_at_%s : %d\t\t",city[cityID],ResultTime(Timeline[timeID]),from_arv[cityID][timeID]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");

	fprintf(fp,"from_%s : \n",dptstation);		//checkpoint(3) output to_h table
	for(cityID = 0; cityID < NumOfStation; cityID++) {
		for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
			fprintf(fp,"from_%s_at_%s : %d\t\t",city[cityID],ResultTime(Timeline[timeID]),to_dpt[cityID][timeID]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");

	fprintf(fp,"from_%s : \n",arvstation);		//checkpoint(3) output to_t table
	for(cityID = 0; cityID < NumOfStation; cityID++) {
		for(timeID = NumOfdptTime; timeID <= NumOfarvTime; timeID++) {
			fprintf(fp,"from_%s_at_%s : %d\t\t",city[cityID],ResultTime(Timeline[timeID]),to_arv[cityID][timeID]);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");

	fprintf(fp,"fare:");
	if( min_cost == INFINITE )	{
		fprintf(fp,"0\n");} 
	else{
		fprintf(fp,"%d %s %s ",min_cost,CityName,ResultTime(StartMeeting));
		fprintf(fp,"- %s\n",ResultTime(EndMeeting));
	}
	for(j = 0; j < NumOfStation; j++) {			//checkpoint(4) lowest price of each station
		fprintf(fp,"%s: %d\n",city[j],min_COES[j]);
	}
	fclose(fp);
	return 1;
}
