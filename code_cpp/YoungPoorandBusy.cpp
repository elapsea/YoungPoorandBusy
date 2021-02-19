#include<stdio.h>
#include<stdlib.h>
#include<iostream>
//c++version:c++11,compiler:Dev-c++5.11
#include<vector>
#include<string>
#include<sstream>
#include<algorithm>
#include<fstream>

#define MAXCONN 2000+1
#define MAXCITY 100
#define INFINITE 999999

using namespace std;

struct train {
  	int from,to;	//the number of departure station and arrival station
  	int dpt,arv;	//the number of departure time and arrival time
 	int fare;		//the cost 
};

int from_dpt[MAXCITY][MAXCONN], /* fr_h */
	from_arv[MAXCITY][MAXCONN],/* fr_t */
	to_dpt[MAXCITY][MAXCONN], /* to_h */
 	to_arv[MAXCITY][MAXCONN]; /* to_t */		
 	
vector<train> trains;
vector<train> rtrains;
int NumOfStation;				//total number of station
vector<int> Timeline;			//used to record sequence
vector<string> CityNum;			//used to number the city
vector<string>::iterator iter; 	//used for CityNum


void init(){	//empty the values in the vector and the number of station
  	NumOfStation = 0;
 	Timeline.clear();
 	CityNum.clear();
  	trains.clear();
 	rtrains.clear();
}

void split(const string& s,vector<int>& sv,const char flag = ' ') { //used in function ConvertedTime to split hh:mm
    sv.clear();
    istringstream iss(s);
    string temp;
    while (getline(iss, temp, flag)) {
        sv.push_back(stoi(temp));	//need to add the following commands in compile option:-std=c++11
    }
    return;
}

int ConvertedTime(string s){	//convert string-time into int-time
	vector<int> sv;
    split(s, sv, ':');
  	return ( sv[0]*60+sv[1] ) ;
}

string ResultTime(int time){	//convert int-time into hh:mm for output
	string h = std::to_string(time/60);
	string m;
	if(time%60 < 10)	m = "0"+std::to_string(time%60);
	else	m = std::to_string(time%60);
	return(h + ":" + m);
}

int TimeOrder(int time) {	//return the order of time
 	 return lower_bound(Timeline.begin(),Timeline.end(),time) - Timeline.begin();
}

bool cmp_stol(const train &a,const train &b) {	//sorting from small to large
  	return a.dpt < b.dpt;
}

bool cmp_ltos(const train &a,const train &b) {	//sorting from large to small
 	 return a.dpt > b.dpt;
}

bool cmp_fare(const train &a,const train &b) {	//sorting fare from large to small
 	 return a.fare > b.fare;
}

bool cmp_time(const train &a,const train &b) {	//sorting time from large to small
 	 return Timeline[a.arv] - Timeline[a.dpt] > Timeline[b.arv] - Timeline[b.dpt];
}

void MaxFare(vector<train> &vec,int write)		//checkpoint(1) max fare
{
	int max_fare;
	string outpath = "E:\\YoungPoorandBusy\\code_cpp\\output_text\\output"+std::to_string(write)+".txt";
   	ofstream out;
	out.open(outpath,ios::out|ios::app);
	sort(vec.begin(),vec.end(),cmp_fare);
	max_fare = vec[0].fare;
	cout << "The most expensive train : " << endl;			
	
	if(write > 0) out << "The most expensive train : " << endl;
	for(int i = 0;i < vec.size();i++)
	{
		if(vec[i].fare == max_fare){
			cout << CityNum[vec[i].from] << " " << CityNum[vec[i].to] << " "  << ResultTime(Timeline[vec[i].dpt]) << " "  << ResultTime(Timeline[vec[i].arv]) << " "  << vec[i].fare << endl;
			if(write > 0)			out << CityNum[vec[i].from] << " " << CityNum[vec[i].to] << " "  << ResultTime(Timeline[vec[i].dpt]) << " "  << ResultTime(Timeline[vec[i].arv]) << " "  << vec[i].fare << endl;
			//checkpoint(1) max fare written into file
		}
	}
	cout << endl;
	if(write>0)		out << endl;
}

void MaxTime(vector<train> &vec,int write)		//checkpoint(1) max time
{
	int max_time,count;
	string outpath = "E:\\YoungPoorandBusy\\code_cpp\\output_text\\output"+std::to_string(write)+".txt";
   	ofstream out;
	out.open(outpath,ios::out|ios::app);
	sort(vec.begin(),vec.end(),cmp_time);
	max_time = Timeline[vec[0].arv] - Timeline[vec[0].dpt];
	cout << "The most time-consuming train : " << endl;			
	
	if(write > 0) out << "The most time-consuming train : " << endl;
	for(int i = 0;i < vec.size();i++)
	{
		if(Timeline[vec[i].arv] - Timeline[vec[i].dpt]==max_time){
			cout << CityNum[vec[i].from] << " " << CityNum[vec[i].to] << " "  << ResultTime(Timeline[vec[i].dpt]) << " "  << ResultTime(Timeline[vec[i].arv]) << " "  << vec[i].fare << endl;
			if(write > 0)			out << CityNum[vec[i].from] << " " << CityNum[vec[i].to] << " "  << ResultTime(Timeline[vec[i].dpt]) << " "  << ResultTime(Timeline[vec[i].arv]) << " "  << vec[i].fare << endl;
			//checkpoint(1) max time written into file
		}		
	}
	cout << endl;
	if(write > 0)		out << endl;
}

train ProcessConnection(string context,string dpttime,string arvtime){
  	vector<string> buf;	//be used to parse one sentence
 	stringstream ss;
 	ss << context;
	while( ss >> context ){
    buf.push_back(context);
  	}
	int from;		//the number of departure station
	if (!count(CityNum.begin(),CityNum.end(),buf[0])){
		from = NumOfStation;
		CityNum.push_back(buf[0]);
		NumOfStation++;
	}
	else{
 		iter = find(CityNum.begin(),CityNum.end(),buf[0]);
 		from = iter - CityNum.begin(); 
 	}
 	int to;			//the number of arrival station
	if (!count(CityNum.begin(),CityNum.end(),buf[2])){
		to = NumOfStation++;
		CityNum.push_back(buf[2]);
	}
	else{
 		iter = find(CityNum.begin(),CityNum.end(),buf[2]);
 		to = iter - CityNum.begin(); 
 	}
  	int dpt = ConvertedTime(buf[1]);		//the int-time of departure time
  	int arv = ConvertedTime(buf[3]);		//the int-time of arrival time
  	if(dpt < ConvertedTime(dpttime) || arv > ConvertedTime(arvtime)) {
    	return (train){-1,-1,-1,-1,-1};
  	}
  	Timeline.push_back(dpt);
  	Timeline.push_back(arv);
  	return (train){from,to,dpt,arv,(atoi)(buf[4].c_str())};
}


void make_table(int v[MAXCITY][MAXCONN],int cityID,vector<train> &vec,int NumOfdptTime,int NumOfarvTime,int flag) {	//use Dijkstra algorithm to make the min_cost_table 
	//flag==1 for the process of trains,flag==2 for the process of rtrains
  	for(int j = 0;j < NumOfStation;j++) for(int k = 0;k <= NumOfarvTime;k++) v[j][k] = INFINITE;
  	int cur = 0;
	if(flag == 1)		{v[cityID][NumOfdptTime] = 0; sort(vec.begin(),vec.end(),cmp_stol);}
	else if(flag == 2)	{v[cityID][NumOfarvTime] = 0; sort(vec.begin(),vec.end(),cmp_ltos);}
  	for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
  		if(flag == 1){
        	if( timeID-1 >= NumOfdptTime ) {
      			for(int stationID = 0;stationID < NumOfStation;stationID++){
	  				v[stationID][timeID] = min(v[stationID][timeID],v[stationID][timeID-1]);
     	 		}
    		}
		}
		else if(flag == 2){
	    	if( NumOfarvTime-timeID+1 <= NumOfarvTime ) {
      			for(int stationID = 0;stationID < NumOfStation;stationID++){
	  				v[stationID][NumOfarvTime-timeID] = min(v[stationID][NumOfarvTime-timeID],v[stationID][NumOfarvTime-timeID+1]);
      			}
			}
		}
		int num = (flag ==1 ? timeID : NumOfarvTime - timeID);
    	while( cur < (int)vec.size() && vec[cur].dpt == num ){
			v[vec[cur].to][vec[cur].arv] = min(v[vec[cur].to][vec[cur].arv],v[vec[cur].from][num] + vec[cur].fare);
      		cur++;
    	}
  }
}

void Cal_min_cost(vector<train> &trains,vector<train> &rtrains,string dptstation,string arvstation,string dpttime,string arvtime,int meettime,int write){
	//calculate the minimum cost
  	if( !(count(CityNum.begin(),CityNum.end(),dptstation)) || !(count(CityNum.begin(),CityNum.end(),arvstation)) ) {
   	 	cout << "0" << endl;
    	return;
  	}
 	iter = find(CityNum.begin(),CityNum.end(),dptstation);		//checkpoint(6) variable of startstation
 	int dptID = iter - CityNum.begin(); 
 	iter = find(CityNum.begin(),CityNum.end(),arvstation);		//checkpoint(6) variable of endstation
 	int arvID = iter - CityNum.begin(); 
  	int NumOfdptTime = TimeOrder(ConvertedTime(dpttime)); 		//checkpoint(6) variable of starttime
	int NumOfarvTime = TimeOrder(ConvertedTime(arvtime)); 		//checkpoint(6) variable of starttime 
	  	
	make_table(to_dpt,dptID,trains,NumOfdptTime,NumOfarvTime,1);	//checkpoint(6) 
	make_table(to_arv,arvID,trains,NumOfdptTime,NumOfarvTime,1);
	make_table(from_dpt,dptID,rtrains,NumOfdptTime,NumOfarvTime,2);
	make_table(from_arv,arvID,rtrains,NumOfdptTime,NumOfarvTime,2);
	
  	int min_cost = INFINITE;	//the minimum cost
  	int flag = min_cost;		//to record each min_cost
  	string CityName;
  	int StartMeeting;			//start meeting time
    int EndMeeting;				//end meeting time
    int CostOfEachStation;		//cost of each station
	int min_COES[NumOfStation];	//find min_cost per station
	
    for(int i = 0;i < NumOfStation;i++)
    min_COES[i] = INFINITE; 
  	for(int cityID = 0;cityID < NumOfStation;cityID++){
		for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
      		if( to_dpt[cityID][timeID] == INFINITE || to_arv[cityID][timeID] == INFINITE ) continue;
      		int MeetingTimeID = lower_bound(Timeline.begin(),Timeline.end(),Timeline[timeID] + meettime) - Timeline.begin();	
			//checkpoint(6) variable of meetingtime && consider the time of meeting
      		if( MeetingTimeID >= (int)Timeline.size() ) continue;
        	if( from_dpt[cityID][MeetingTimeID] == INFINITE || from_arv[cityID][MeetingTimeID] == INFINITE ) continue;
			CostOfEachStation = to_dpt[cityID][timeID] + to_arv[cityID][timeID] + from_dpt[cityID][MeetingTimeID] + from_arv[cityID][MeetingTimeID];
      		min_cost = min(min_cost,CostOfEachStation);
			   
			if(CostOfEachStation < INFINITE)
		 	{
		 		min_COES[cityID] = min(min_COES[cityID],CostOfEachStation);	//minimum cost per station
		 	}
		 	if(flag != min_cost)	//checkpoint(6) && record each information of min_cost
		 	{
		 		flag = min_cost;
		 		int l = MeetingTimeID;
		 		while(flag == to_dpt[cityID][timeID] + to_arv[cityID][timeID] + from_dpt[cityID][l+1] + from_arv[cityID][l+1])	l++;	//get the number of leave
		 		CityName = CityNum[cityID];
		 		StartMeeting = Timeline[timeID];
		 		EndMeeting = Timeline[l];
		 	}
    	}
  	}  
  	
	cout << "to_" << dptstation << " : " << endl;		//checkpoint(2) output fr_h table
	for(int cityID = 0;cityID < NumOfStation;cityID++){
		for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
			cout << "from_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << from_dpt[cityID][timeID] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;
	
	cout << "to_" << arvstation << " : " << endl;		//checkpoint(2) output fr_t table
	for(int cityID = 0;cityID < NumOfStation;cityID++){
		for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
			cout << "from_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << from_arv[cityID][timeID] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;
	
	cout << "from_" << dptstation << " : " << endl;		//checkpoint(3) output to_h table
	for(int cityID = 0;cityID < NumOfStation;cityID++){
		for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
			cout << "to_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << to_dpt[cityID][timeID] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;
	
	cout << "from_" << arvstation << " : " << endl;		//checkpoint(3) output to_t table
	for(int cityID = 0;cityID < NumOfStation;cityID++){
		for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
			cout << "to_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << to_arv[cityID][timeID] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;
	
  	cout << "fare:";						
  	if( min_cost == INFINITE )	cout << "0" << endl;
  	else 	cout << min_cost << " " << CityName << ": " << ResultTime(StartMeeting) << " - " << ResultTime(EndMeeting) << endl;
  	for(int j = 0;j < NumOfStation;j++)
  	cout << CityNum[j] << ": " << min_COES[j] << endl;		//checkpoint(4) by outputting the values
  	
  	
  	
  	if(write > 0)		//wirtten into file
  	{
  		string outpath="E:\\YoungPoorandBusy\\code_cpp\\output_text\\output"+std::to_string(write)+".txt";
    	ofstream out;
		out.open(outpath,ios::out|ios::app);

  		out << "to_" << dptstation << " : " << endl;		//out fr_h table
		for(int cityID = 0;cityID < NumOfStation;cityID++){
			for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
				out << "from_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << from_dpt[cityID][timeID] << "\t\t";
			}
		out << endl;
		}	
		out << endl;
	
		out << "to_" << arvstation << " : " << endl;		//out fr_t table
		for(int cityID = 0;cityID < NumOfStation;cityID++){
			for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
				out << "from_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << from_arv[cityID][timeID] << "\t\t";
			}
			out<<endl;
		}
		out<<endl;
	
		out << "from_" << dptstation << " : " << endl;		//out to_h table
		for(int cityID = 0;cityID < NumOfStation;cityID++){
			for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
				out << "to_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << to_dpt[cityID][timeID] << "\t\t";
			}
			out << endl;
		}
		out << endl;
	
		out << "from_" << arvstation << " : " << endl;		//out to_t table
		for(int cityID = 0;cityID < NumOfStation;cityID++){
			for(int timeID = NumOfdptTime;timeID <= NumOfarvTime;timeID++){
				out << "to_" << CityNum[cityID] << "_at_" << ResultTime(Timeline[timeID]) << " : " << to_arv[cityID][timeID] << "\t\t";
			}
		out << endl;
		}
		out << endl;
		
		out << "fare: ";					
		if( min_cost == INFINITE )	out << "0" << endl;
  		else 	out << min_cost << " " << CityName << ": " << ResultTime(StartMeeting) << " - " << ResultTime(EndMeeting) << endl;	
  		for(int j = 0;j < NumOfStation;j++)
  		out << CityNum[j] << ": " << min_COES[j] << endl;		//checkpoint(4) by writting file
  		out.close();		//close the out file
	}
}

int main(){
  	int n,choice,write;
  	string dptstation,arvstation,dpttime,arvtime;
  	int meettime;
  	while(1) {
  		cout << "if you want to input the values manually,input 1 / if you want to read the file,input 2 : ";
  		cin >> choice;
  		if(choice == 0)			//input 0 to exit
  			break;
  		if(choice==1){	//input 1 to input values manually
  			write=0;
  			cout << "the number of connections:";
  			cin >> n;
  			init();
  			cout << "駅 1:";
  			cin >> dptstation;
  			cout << "駅 2:";
	  		cin >> arvstation;
	  		cout << "出発時刻:";
  			cin >> dpttime;
  			cout << "帰着時刻:";
  			cin >> arvtime;
  			cout << "面会時間:";
  			cin >> meettime;				//checkpoint(5) by inputting the values manually
    		cin.ignore();
    		for(int i = 0;i < n;i++){	//prepare_data
      			string s;
      			getline(cin,s);
      			train t = ProcessConnection(s,dpttime,arvtime);
      			if( t.fare == -1 ) continue;
      			trains.push_back(t);
    		}
		}
		
		else if(choice==2)	//input 2 to read values in file
		{
			cout << "the number of file please (1/2/3/4): ";
  			cin >> write;
			string filename = "E:\\YoungPoorandBusy\\code_cpp\\input_text\\input"+std::to_string(write)+".txt";
			fstream fin;
			fin.open(filename.c_str(), ios::in);
			if (fin.fail())
			{
				cout << "Input file opening failed.\n";
				exit(1);
			}
			vector<string> v;
			string tmp;
  			init();
  			int count = 0;
  			string point;
  			while (fin >> tmp){
				v.push_back(tmp);
			}
			n = atoi(v[4].c_str());			//checkpoint(5) by reading file
			dptstation = v[6];
			arvstation = v[8];
			dpttime = v[10];
			arvtime = v[12];
			meettime = atoi(v[14].c_str());
			for(int i = 0;i < n;i++){	//prepare_data
				int j = 15+i*5;
      			string s = v[j]+" "+v[j+1]+" "+v[j+2]+" "+v[j+3]+" "+v[j+4];
      			train t = ProcessConnection(s,dpttime,arvtime);
      			if( t.fare == -1 ) continue;
      			trains.push_back(t);
    		} 
		} 
    	Timeline.push_back(ConvertedTime(dpttime));
    	Timeline.push_back(ConvertedTime(arvtime));
    	sort(Timeline.begin(),Timeline.end());
    	Timeline.erase(unique(Timeline.begin(),Timeline.end()),Timeline.end());	//sort the time
    	for(int i = 0;i<(int)trains.size();i++) {	//number the time
      		trains[i].dpt = TimeOrder(trains[i].dpt);
      		trains[i].arv = TimeOrder(trains[i].arv);
    	}
    	string outpath="E:\\YoungPoorandBusy\\code_cpp\\output_text\\output"+std::to_string(write)+".txt";
    	ofstream out(outpath);
    	
    	cout << endl;
		cout << "The available train information : " << endl;		//checkpoint(1) all available train
    	for(vector<train>::iterator j = trains.begin();j != trains.end();j++){
			cout << CityNum[j->from] << " " << CityNum[j->to] << " "  << ResultTime(Timeline[j->dpt]) << " "  << ResultTime(Timeline[j->arv]) << " "  << j->fare << endl;
			
			}
		cout << endl;
 		if(write > 0){
			out << "The available train information : " << endl;		//checkpoint(1) all available train written into file
			for(vector<train>::iterator j = trains.begin();j != trains.end();j++)
				out << CityNum[j->from] << " " << CityNum[j->to] << " "  << ResultTime(Timeline[j->dpt]) << " "  << ResultTime(Timeline[j->arv]) << " "  << j->fare << endl;
    		out << endl;
    	}
		MaxFare(trains,write);		//checkpoint(1) max fare
    	MaxTime(trains,write);		//checkpoint(1) max time
    	for(int num = 0;num < (int)trains.size();num++)		//prepare rtrains' values
    	{
    		train r;
       		r.to = trains[num].from;	
        	r.from = trains[num].to;
        	r.arv = trains[num].dpt;
        	r.dpt = trains[num].arv;
        	r.fare = trains[num].fare;
        	rtrains.push_back(r);
		}
    	Cal_min_cost(trains,rtrains,dptstation,arvstation,dpttime,arvtime,meettime,write);		//calculate min_cost
  	}
  	return 0;
}
