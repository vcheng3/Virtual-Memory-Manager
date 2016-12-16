#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <time.h>
#include <unordered_map>

using namespace std;
int *mem_array;
vector<int> file_array;
unordered_map<int,vector<int>> inputMap;
string policy;
int frames;
int rest = 0;
int jack = 0;
int mempointer = 0;
int thatindex = 0;
int fifofault = 0;
int lrustackfault = 0;
int lfufault = 0;
double timeOptimal = 1.0 ;
double timePolicy = 1.0;
int replacementOptimal = 1 ;
int replacementPolicy = 1 ;

int gotit(int value) {
    for (int i = 0; i < frames; i++) {
        if (mem_array[i] == value) {

            return i;

        }

    }
    return -1;
};

int gotit2(int value) {
    for (int i = frames - 1; i != -1; i--) {
        if (mem_array[i] == value) {
            thatindex = 0;
            return i;
        } else if (mem_array[i] != value && mem_array[i] == -2) {
            thatindex = -2;
            return i;
        }
    }
    thatindex = -1;
    return -1;
}

int lookforNull() {
    for (int i = frames - 1; i != -1; i--) {
        if (mem_array[i] == -2) {
            return i;
        }
    }
    return -1;
}

void readjuststack(int start, int end) {

    for (int i = start; i != (end - 1); i--) {
        mem_array[i] = mem_array[i - 1];

    }
    mem_array[0] = -2;
}

void printstack() {
    for (int i = 0; i < frames; i++) {
        cout << i << " : " << mem_array[i] << endl;
    }
}

int maxFromVector(vector<int> x){
    int maxValue = 0;
    for(int i = 0; i < file_array.size(); i++){
        if(maxValue < file_array.at(i)){
            maxValue = file_array.at(i);
        }
    }
    return maxValue;
}
void LeastFrequentlyUsed() {
    bool flag;
    int reps,min = 0;
    int hits = 0;
    int numOfReplacements = 0;
    int pageCounts = 0;
    int sizeOfPageArray = maxFromVector(file_array);

    int * pageCountsArray = new int[sizeOfPageArray]; //array to track page counts up to highest number from page sequence
    int * repeatFrequency = new int[frames];

    for(int i = 0; i < file_array.size(); i++){
        int value = gotit(file_array.at(i));			//value of counter at position in sequence -- page
        flag = true;
        for(int j = 0; j < frames; j++){
            if(mem_array[j] == value){			//if it matches in the mem_array to page sequence
                flag = false;
                pageCountsArray[value]++;
                //hits++;
                //increase page hit values. no page fault here.
            }
        }
        //replacement -- victim pages
        if(flag == true){
            lfufault++;
            if(i >= frames){										//make sure it doesn't exceed the number of frames
                for(int k = 0; k < frames; k++){
                    reps = mem_array[k];
                    repeatFrequency[k] = pageCountsArray[reps];
                }
                min = repeatFrequency[0];
                for(int l = 0 ; l < frames; l++){
                    if(repeatFrequency[l] == min){
                        pageCountsArray[value]++;
                        mem_array[l] = value;					//replacement of frames
                    }
                    if(repeatFrequency[l] < min){
                        min = repeatFrequency[l];				//you have a new minimum value based on the repeatFrequency
                    }
                }
            }
            else{
                mem_array[i] = value;
                pageCountsArray[value]++;
            }
        }
    }

    replacementPolicy = lfufault;
//    delete pageCountsArray;
//    delete repeatFrequency;
}
void LRU_Stack() {
    jack = frames;

    for (int i = 0; i < file_array.size(); i++) {
        rest = gotit2(file_array.at(i));
        if (jack != 0) {
            if (rest != -1 && thatindex == -2) {
                mem_array[--jack] = file_array.at(i);

//                cout << "value : " << file_array.at(i) << " has been assigned index : " << jack << " in Main MEMORY" <<
//                endl;
//                cout << "Stack after readjustment" << endl;
//                printstack();
            } else if (rest != -1 && thatindex == 0) {
//                cout << "Found value : " << file_array.at(i) << ", at index : " << rest << endl;
                int lookingfornull = lookforNull();

                if (lookingfornull != -1) {
                    mem_array[lookingfornull] = mem_array[rest];
                    mem_array[rest] = -2;
                    readjuststack(rest, lookingfornull);
//                    cout << "Stack after readjustment" << endl;
//                    printstack();
                }
            }
        } else {

            if (rest == -1) {
                //page fault
                lrustackfault = lrustackfault + 1;
//                cout << "Value : " << file_array.at(i) << " not found" << endl;
                mem_array[frames - 1] = -2;
                readjuststack((frames - 1), 0);
                mem_array[0] = file_array.at(i);
//                cout << "Stack after readjustment" << endl;
//                printstack();
            } else {
//                cout << "Found value : " << file_array.at(i) << ", at index :" << rest << endl;
                int found = mem_array[rest];
                mem_array[rest] = -2;
                readjuststack(rest, 0);
                mem_array[0] = found;
//                cout << "Stack after readjustment" << endl;
//                printstack();
            }
        }

    }

}

void lfuFunc() {
    vector<int> inputPageNumbers = file_array ;
    int frameSize = frames ;
    int pageRep = 0 ;
    bool exists = false  ;
    struct node {
        int value;
        int freq ;
        int misses ;
    } newNode;
    int victim = 0 ;
    vector<node> memoryFrame ;
    int lowestFreq = 0 ;
    int highestMisses = 0 ;
    int oldestNode = 0 ;
    bool tie = false ;
    for(auto request : inputPageNumbers){
        //cout<<"\nRequest "<<request << endl ;
        exists = false ;
        for(int k = 0 ; k < memoryFrame.size() ; k++){
            if(memoryFrame[k].value==request){
                //cout<<"\nHit"<<endl;
                exists = true ;
                memoryFrame[k].misses = memoryFrame[k].misses + 1 ;
                memoryFrame[k].freq = memoryFrame[k].freq + 1 ;

            }
            else{
                memoryFrame[k].misses = memoryFrame[k].misses + 1 ;
            }
        }

        if(!exists){
            if(memoryFrame.size()<frameSize){
                newNode.value = request ;
                newNode.freq = 1 ;
                newNode.misses = 0 ;
                memoryFrame.push_back(newNode);
            }

            else{
                //cout<<"\nReplace. " << endl  ;
                pageRep++;
                lowestFreq = 1000; victim = 0 ;
                for(int i = 0 ; i < memoryFrame.size() ; i++){
                    if(memoryFrame[i].freq<lowestFreq){
                        lowestFreq = memoryFrame[i].freq ;
                        victim = i ;
                    }
                }

                //cout<<"Lowest Freq - " << lowestFreq <<endl ;
                tie = false ;
                highestMisses = memoryFrame[victim].misses ; oldestNode = 0 ;
                for(int j = 0 ; j < memoryFrame.size() ; j++){
                    if(memoryFrame[j].freq==lowestFreq){
                        if(memoryFrame[j].misses > highestMisses){
                            highestMisses = memoryFrame[j].misses ;
                            oldestNode = j ;
                            tie = true ;
                        }
                    }
                }

                if(tie){
                    //cout<<"Tie"<<endl;
                    //cout<<"Highest Misses - " << highestMisses << endl ;
                    newNode.value = request ;
                    newNode.freq = 1 ;
                    newNode.misses = 0 ;
                    memoryFrame[oldestNode] = newNode ;
                    //cout<<"Oldest Node " << oldestNode <<endl;
                }

                else{
                    newNode.value = request ;
                    newNode.freq = 1 ;
                    newNode.misses = 0 ;
                    memoryFrame[victim]= newNode;
                    //cout<<"Victim - " << victim<<endl ;
                }
            }
        }


       /* for(auto b : memoryFrame){
            cout<<"| " << b.value << "," << b.freq << "," << b.misses << " |" << endl ;
        }
        cout<<"\n\n=================================\n\n" ;*/

    }


    replacementPolicy = pageRep;
}

void FirstInFirstOut() {


    for (int i = 0; i < file_array.size(); i++) {
        if (jack < frames) {
            rest = gotit(file_array.at(i));
            if (rest != -1) {
//                cout << "found it at index : " << rest << endl;

            } else {
//                cout << file_array.at(i) << " has  been allocated memory space at MAIN MEMORY index " << jack <<
//                " here" << endl;
                mem_array[jack++] = file_array.at(i);
            }
        } else {
            rest = gotit(file_array.at(i));
            if (rest != -1) {
//                cout << "found it at index : " << rest << endl;
            } else {
                //page fault
                fifofault=fifofault+1;
                mem_array[mempointer] = file_array.at(i);
//                cout << file_array.at(i) << " has been allocated MAIN MEMORY space at index " << mempointer <<
//                " else" << endl;
                mempointer = (mempointer + 1) % frames;
            }
        }
    }

    replacementPolicy = fifofault ;
}
void optimal() {
    clock_t t1, t2  ;
    t1 = clock();
    //unordered_map<int,vector<int>> inputMap = inputMap ;
    int frameSize = frames ;
    vector<int> inputPageNumbers = file_array ;
    vector<int> frame ;
    int future[frameSize];
    int victim = 0 ;
    int victimNextLocation = 0 ;
    int temp ;
    int pageFault = 0 ;
    int pageRep = 0 ;
    bool exists = false ;



    for(auto i:inputPageNumbers){
        victim = 0 ;
        exists = false ;
        victimNextLocation = 0 ;
        //cout<<endl<<"Request - " <<i<<endl;
        //cout<<endl;
        /* for(auto a : inputMap){
             cout<<a.first << " :";
             for(auto j : a.second){
                 cout << j << ",";
             }

             cout<<endl ;
         }
         cout<<endl;
*/
        /*
        for(auto f : frame){
            cout<<f << " | ";
        }*/

        for(auto k : frame){
            if(k==i){
                exists = true ;
                //cout<<endl<<"Hit!";
            }
        }

//        cout<<endl;
//        cout<<endl;


        if(exists){
            //cout<<"Hit!" << endl;
            inputMap[i].erase(inputMap[i].begin());
        }
        if(!exists){

            if(frame.size()<frameSize){
                frame.push_back(i);
                inputMap[i].erase(inputMap[i].begin());
                pageFault++;

            }
                //check if exsists
                //4 5 1 1 4 4 3 5 5 3 3 1 4 2 2 3 4 5 2 2 4 5 2 2 4 4 1 2 1 3 2 1 3 5 3 2 1 3 5 3 4 3 1 4 5
                //else find next victim - erase victim - add new page - erase first location
            else{
                //cout<<"Replace."<<endl;
                pageFault++;
                pageRep++ ;
                //cout<<endl ;
                //cout<<"Rep count "<<pageRep;
                for(int j = 0 ; j<frame.size();j++){
                    //exsists in future requests

                    //   if(inputMap.count(frame[j])>0){
                    if(!inputMap[frame[j]].empty()){
                        temp = *(inputMap[frame[j]].begin());
                        //cout<<endl<<"Next future of "<<frame[j]<<" - "<<temp;

                        if(temp>victimNextLocation){
                            victimNextLocation = temp ;
                            victim = j ;
                        }
                    }
                        //does not exsists in future ;
                    else{
                        //cout<<endl<<"No future of " << frame[j];
                        victim = j ;

                    }

                    //  }
                }

                //cout<<endl<<"Victim - " << frame[victim];
                //victim found
                frame.erase(frame.begin()+victim);

                //erase first and add new

                frame.push_back(i);
                if(!inputMap[i].empty()){
                    inputMap[i].erase(inputMap[i].begin());
                }

            }


        }
        //cout<<"\nFrame after replacement "<< endl;
/*
          for(auto& f : frame){
              cout<<f << " | ";
          }
          cout<<endl;
          cout<<"=============================" ;
          cout<<endl;*/
    }

    /*  cout<<"Total Number of requests - " << inputPageNumbers.size()<<endl ;
      cout<<"Page Replacements - " <<pageRep<<endl;
      t2 = clock();
      double diff ((double)t2-(double)t1);
      double seconds = (diff / CLOCKS_PER_SEC)*1000;
      cout<<"Running Time - " << seconds << " msec" << endl;*/

    replacementOptimal = pageRep ;

}
void lruClock() {
    vector<int> inputPageNumbers = file_array ;
    int frameSize = frames ;
    struct node {
        int value;
        int refValue;
    } newNode;

    vector<node> frame;
    int existsAt = 0;
    bool exists = false;
    int victimPointer = 0;
    newNode.value = -1;
    newNode.refValue = 0;
    int pageRep = 0 ;

    for (auto req:inputPageNumbers) {
        //cout << "Request " << req << endl;
        exists = false;
        if (frame.empty()) {
            newNode.value = req;
            newNode.refValue = 0;
            frame.push_back(newNode);
            victimPointer = 1;
        }

        else {

            for (int f = 0; f < frame.size(); f++) {
                if (frame[f].value == req) {
                    //cout << "Hit!" << endl;

                    if (frame[f].refValue == 0) {
                        frame[f].refValue = 1;
                    }
                    exists = true;
                }

            }


            if (!exists) {

                if (frame.size() < frameSize) {
                    newNode.refValue = 0;
                    newNode.value = req;
                    frame.push_back(newNode);
                    victimPointer =  (victimPointer+1)%frameSize;
                }

                else {
                    pageRep++;
                    while (1) {
                        //cout << frame[victimPointer].value << " , " << frame[victimPointer].refValue << endl;
                        if (frame[victimPointer].refValue == 0) {
                            break;
                        }

                        else {
                            frame[victimPointer].refValue = 0;
                            victimPointer =  (victimPointer+1)%frameSize;
                        }
                    }

                    frame[victimPointer].refValue = 0;
                    frame[victimPointer].value = req;

                    victimPointer =  (victimPointer+1)%frameSize;

                }
            }
        }




        /* for (auto a : frame)
             cout << "(" << a.value << "," << a.refValue << ") | ";

         cout << "\nVictim Pointer - " << victimPointer << endl;
         cout << "\n================================== " << endl;*/
    }

    replacementPolicy = pageRep;
    //cout<<"\nPage Replacements LRU-Clock = " << pageRep <<endl;
}
void lruRef() {
    vector<int> inputPageNumbers = file_array ;
    int frameSize = frames ;
    struct node {
        int value;
        int refValue;
    } newNode;

    vector<node> frame;
    int existsAt = 0;
    bool exists = false;
    int victimPointer = 0;
    newNode.value = -1;
    newNode.refValue = 0;
    int pageRep = 0 ;

    for (auto req:inputPageNumbers) {
        //cout << "Request " << req << endl;
        exists = false;
        if (frame.empty()) {
            newNode.value = req;
            newNode.refValue = 0;
            frame.push_back(newNode);
            victimPointer = 1;
        }

        else {

            for (int f = 0; f < frame.size(); f++) {
                if (frame[f].value == req) {
                    //cout << "Hit!" << endl;


                    frame[f].refValue = frame[f].refValue/2 + 128 ;

                    exists = true;
                }

            }


            if (!exists) {

                //cout<<"Miss"<<endl;
                if (frame.size() < frameSize) {
                    newNode.refValue = 0 ;
                    newNode.value = req;
                    frame.push_back(newNode);
                    victimPointer =  (victimPointer+1)%frameSize;
                }

                else {
                    pageRep++;
                    int lowest = 10000;

                    int prevVictim = victimPointer;
                    bool flag = false ;

                    for(int fr = prevVictim ; flag==false || fr != prevVictim ; fr = ++fr%frameSize){
                        flag = true ;
                        if(frame[fr].refValue==0){
                            victimPointer = fr;
                            break ;

                        }
                        else if(frame[fr].refValue<lowest){
                            lowest = frame[fr].refValue ;
                            victimPointer = fr ;
                        }

                    }

                    for(int a=0 ; a< frame.size() ; a++){
                        frame[a].refValue = frame[a].refValue/2;
                    }

                    frame[victimPointer].refValue = 0;
                    frame[victimPointer].value = req;

                    victimPointer =  (victimPointer+1)%frameSize;

                }
            }
        }


        /* for (auto a : frame)
             cout << "(" << a.value << "," << a.refValue << ") | ";

         cout << "\nVictim Pointer at " << victimPointer << endl;
         cout << "\n================================== " << endl;*/
    }

    replacementPolicy = pageRep;
    //cout<<"\nPage Rep LRU-REF = " << pageRep <<endl;
}
int main(int argc, char *argv[]) {
    bool rtruth = false;
    bool ftruth = false;
    bool itruth = false;
    bool htruth = false;
    int arguments = argc;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            htruth = true;
        }
        else if (strcmp(argv[i], "-f") == 0) {
            ftruth = true;
        }
        else if (strcmp(argv[i], "-i") == 0) {
            itruth = true;
        } else if (strcmp(argv[i], "-r") == 0) {
            rtruth = true;
        }
    }
    if (argc == 1) {
        frames = 5;
        mem_array = new int[5];
        cout << "array created with size " << frames << endl;
        ftruth = 1;

        policy = "FIFO";
        rtruth = 1;

        itruth = true;

        int x;
        cout << "Enter the page references followed by enter." << endl;
        cout << "Enter -1 to stop" << endl;
        int j = 0 ;
        for (int i = 0; ; i++) {
            cin >> x;
            int c ;
            if (x == -1) { break; }
            else {
                file_array.push_back(x);
                c = inputMap.count(x);
                if(c<=0)
                    inputMap[x].push_back(i);
                else inputMap[x].push_back(i);
            }
        }

    } else {
        for (int i = 0; i < argc; i++) {
//        if (argc < 2) {
//
//            cout << "please enter the appropriate amount of arguments" << endl;
//            return 0;
//        }
            if (strcmp(argv[i], "-h") == 0) {
                cout << "usage summery will be implemented with options"<<endl;
                cout<<"-f available-frames \t : Set the number of frames. By default it should be 5"<<endl;
                cout<<"-r replacement-policy \t : Set the page replacement policy. It can either be:"<<endl;
                cout<<"                      \t \t FIFO (First-in-first-out)"<<endl;
                cout<<"                      \t \t LFU (Least-frequently-used)"<<endl;
                cout<<"                      \t \t LRU-STACK (Least-recently-used stack implementation )"<<endl;
                cout<<"                      \t \t LRU-CLOCK (Least-recently-used clock implementation)"<<endl;
                cout<<"                      \t \t LRU-REF8 (Least-recently-used Reference bit implementation)"<<endl;
                cout<<"                      \t \t The default will be FIFO"<<endl;
                cout<<"-i input file        \t : Read the page reference from a specified file. If not given,"<<endl;
                cout<<"                     \t   the sequence should be read from STDIN(ended with Enter)"<<endl;
                return 0;
            }
            if (strcmp(argv[i], "-f") == 0) {
                frames = atoi(argv[i + 1]);
                mem_array = new int[frames];
//                cout << "array created with size " << frames << endl;


            } else if (ftruth == 0 && i == 1) {
                frames = 5;
                mem_array = new int[5];
//                cout << "Array created with size " << frames << endl;
                ftruth = 1;

            }
            if (strcmp(argv[i], "-r") == 0) {
                policy = argv[i + 1];
//                cout << policy << endl;
            } else if ((strcmp(argv[i], "-r") != 0) && rtruth == 0) {
                policy = "FIFO";
                rtruth = 1;

            }
            if (strcmp(argv[i], "-i") == 0) {
                string line;
                string filename = argv[i + 1];
//                cout << filename << endl;
                ifstream myfiles(filename.c_str(), ios::in);
//                cout << filename << endl;
                myfiles.open(filename.c_str());

                if (!myfiles.is_open()) {
                    cout << "can not find file" << endl;
                    return 0;
                }else{
                    cout<<"File found and read."<<endl;

                    ifstream infile;
                    int i=0;
                    char cNum[10] ;
                    infile.open (filename, ifstream::in);
                    if (infile.is_open())
                    {
                        while (infile.good())
                        {
                            int c;
                            infile.getline(cNum, 256, ' ');
                            file_array.push_back(atoi(cNum)) ;

                            c = inputMap.count(atoi(cNum));
                            if(c<=0)
                                inputMap[atoi(cNum)].push_back(i);
                            else inputMap[atoi(cNum)].push_back(i);

                            i++ ;
                        }
                        infile.close();
                    }
                    else
                    {
                        cout << "Error opening file";
                        return 0;
                    }



                }

            }

        }
    }
    if (itruth == false) {
        int x;
        cout << "Enter the page references followed by enter." << endl;
        cout << "Enter -1 to stop" << endl;
        int j = 0 ;
        for (int i = 0; ; i++) {
            cin >> x;
            int c ;
            if (x == -1) { break; }
            else {
                file_array.push_back(x);
                c = inputMap.count(x);
                if(c<=0)
                    inputMap[x].push_back(i);
                else inputMap[x].push_back(i);
            }
        }

    }
    for (int i = 0; i < frames; i++) {
        mem_array[i] = -2;
    }

    clock_t t;
    t = clock();
    optimal();
    t = clock()-t;
    timeOptimal = ((double)t/CLOCKS_PER_SEC)*1000 ;

    if (policy == "FIFO") {
        clock_t t;
        t = clock();
        FirstInFirstOut();
        t = clock()-t;
        timePolicy = ((double)t/CLOCKS_PER_SEC)*1000 ;

    } else if (policy == "LFU") {
        clock_t t;
        t = clock();
        //LeastFrequentlyUsed();
        lfuFunc();
        t = clock()-t;
        timePolicy = ((double)t/CLOCKS_PER_SEC)*1000 ;

    }
    else if (policy == "LRU-STACK") {
        clock_t t ;
        t = clock();
        LRU_Stack();
        t = clock()-t;
        timePolicy = ((double)t/CLOCKS_PER_SEC)*1000 ;
        replacementPolicy = lrustackfault ;

    }else if(policy=="LRU-CLOCK"){
        clock_t t ;
        t = clock();
        lruClock();
        t = clock()-t;
        timePolicy = ((double)t/CLOCKS_PER_SEC)*1000 ;
    }else if(policy=="LRU-REF8"){
        clock_t t ;
        t = clock();
        lruRef();
        t = clock()-t;
        timePolicy = ((double)t/CLOCKS_PER_SEC)*1000 ;
    }



    double penalty =  (double)((replacementPolicy - replacementOptimal)/(double)replacementPolicy)*100.0;
    cout<<endl<<endl;
    cout<<"# of page replacements with "<<policy<<"     :"<<replacementPolicy<<endl;
    cout<<"# of page replacements with Optimal"<<"     :"<<replacementOptimal<<endl;
    cout<<"% page replacement penalty using "<<policy<<"     :"<<penalty<<endl;

    cout<<endl<<endl<<endl;

    cout<<"Total time to run "<<policy<<" algorithm     : "<<timePolicy<<" msec"<<endl;
    cout<<"Total time to run Optimal algorithm     : "<<timeOptimal<<" msec"<<endl;

    double faster = (double)((timeOptimal - timePolicy)/timeOptimal) * 100.0;

    cout<<policy<<" is "<< faster << "% faster than Optimal Algorithm."<<endl<<endl<<endl ;

    //printf("RunTime of LRU-STACK : %f msecs.\n", (((double)t)/CLOCKS_PER_SEC)*1000);
    return 0;
}