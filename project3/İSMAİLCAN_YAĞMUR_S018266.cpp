#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std; 
vector<string> FILE_STATES{ "INPUT","TAPE","BLANK","STATES","START","ACCEPT","REJECT","TRANSITION","STRING" };
typedef map<string, map<string,map<string,map<string,string>>>> TransitionFunction;

struct TuringMachine {
    set<string> INPUT;
    set<string> TAPE;
    string BLANK;
    set<string> STATES;
    string START;
    string ACCEPT;
    string REJECT;
    TransitionFunction TRANSITIONS;
    string STRING;
};


auto FileReader(string file_path){

        cout<< "[INFO]Reading the file : "<<file_path << endl;
        TuringMachine TM;

        ifstream file(file_path);

        if (!file.is_open()) {
            throw runtime_error("[ERR]File not found.");
        }

        else {

            string line;
            string file_state;
  
           while (getline(file, line)) {
                line.erase(remove_if(line.end()-1, line.end(), ::isspace), line.end()); //to capture each line properly
                if (find(FILE_STATES.begin(), FILE_STATES.end(), line) != FILE_STATES.end()){
                    // FILE_STATE found
                    file_state = line;
                }
                else{
                    if(file_state == FILE_STATES[0]){
                        istringstream iss(line);
                        string substring;
                        while (iss >> substring) {
                            TM.INPUT.insert(substring);
                        }
                    }
                    else if(file_state == FILE_STATES[1]){
                        istringstream iss(line);
                        string substring;
                        while (iss >> substring) {
                            TM.TAPE.insert(substring);
                        }
                    }
                    else if(file_state == FILE_STATES[2]){
                        TM.BLANK = line[0]; // BLANK is a single character
                        TM.TAPE.insert(TM.BLANK);
                    }
                    else if(file_state == FILE_STATES[3]){
                        istringstream iss(line);
                        string substring;
                        while (iss >> substring) {
                            TM.STATES.insert(substring);
                        }
                    }
                    else if(file_state == FILE_STATES[4]){
                        TM.START = line; 
                    }
                    else if(file_state == FILE_STATES[5]){
                        TM.ACCEPT = line; 
                    }
                    else if(file_state == FILE_STATES[6]){
                        TM.REJECT = line; 
                    }                    
                    else if(file_state == FILE_STATES[7]){
                        istringstream iss(line);
                        string input1,input2,input3,input4,input5;
                        if (!(iss >> input1 >> input2 >> input3 >> input4 >> input5)) { break; } // error
                        TM.TRANSITIONS[input1][input2][input3][input4] = input5;
                    } 
                    else if(file_state == FILE_STATES[8]){
                        TM.STRING = line; // BLANK is a single character
                    }
                }   
                


            }
            file.close();
        }
 return TM;
    
}

//write a method to print TuringMachine
void printTM(TuringMachine TM){
    cout << "INPUT: ";
    for (auto i = TM.INPUT.begin(); i != TM.INPUT.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "TAPE: ";
    for (auto i = TM.TAPE.begin(); i != TM.TAPE.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "BLANK: " << TM.BLANK << endl;

    cout << "STATES: ";
    for (auto i = TM.STATES.begin(); i != TM.STATES.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "START: " << TM.START << endl;

    cout << "ACCEPT: " << TM.ACCEPT << endl;

    cout << "REJECT: " << TM.REJECT << endl;

    cout << "TRANSITIONS: " << endl;
    for (auto i = TM.TRANSITIONS.begin(); i != TM.TRANSITIONS.end(); ++i){
        for (auto j = i->second.begin(); j != i->second.end(); ++j){
            for (auto k = j->second.begin(); k != j->second.end(); ++k){
                for (auto l = k->second.begin(); l != k->second.end(); ++l){
                    cout << i->first << " " << j->first << " " << k->first << " " << l->first << " " << l->second << endl;
                }
            }
        }
    }

    cout << "STRING: " << TM.STRING << endl;
}



int main(int argc, char *argv[])
{
    // if (argc < 2)
    // {
    //     // print usage

    //     cerr << "[ERR]Expected filename as an argument.\n[INFO]Exiting... " << endl;
    //     return 1;
    // }
    // string file_path = argv[1];

    try {
    auto TM = FileReader("input.txt");
    printTM(TM);


   } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}