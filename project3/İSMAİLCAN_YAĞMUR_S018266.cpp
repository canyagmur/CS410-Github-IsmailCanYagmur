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

#include <future>
#include <chrono>

using namespace std;
vector<string> FILE_STATES{"INPUT", "TAPE", "BLANK", "STATES", "START", "ACCEPT", "REJECT", "TRANSITION", "STRING"};
string INPUT_STRING;
int TIMEOUT_IN_SECONDS = 5;
typedef map<string, map<string, map<string, string>>> TransitionFunction;

struct TuringMachine
{
    set<string> INPUT_ALPHABET;
    set<string> TAPE_ALPHABET;
    list<string> TAPE;
    string BLANK;
    set<string> STATES;
    string START;
    string ACCEPT;
    string REJECT;
    TransitionFunction TRANSITIONS;
};

auto FileReader(string file_path)
{
    cout << "[INFO]Reading the file : " << file_path << endl;
    TuringMachine TM;

    ifstream file(file_path);

    if (!file.is_open())
    {
        cout<<"[ERR]File not found."<<endl;
        throw runtime_error("[ERR]File not found.");
    }

    else
    {

        string line;
        string file_state;

        while (getline(file, line))
        {
            line.erase(remove_if(line.end() - 1, line.end(), ::isspace), line.end()); // to capture each line properly
            if (find(FILE_STATES.begin(), FILE_STATES.end(), line) != FILE_STATES.end())
            {
                // FILE_STATE found
                file_state = line;
            }
            else
            {
                if (file_state == FILE_STATES[0])
                {
                    istringstream iss(line);
                    string substring;
                    while (iss >> substring)
                    {
                        TM.INPUT_ALPHABET.insert(substring);
                    }
                }
                else if (file_state == FILE_STATES[1])
                {
                    istringstream iss(line);
                    string substring;
                    while (iss >> substring)
                    {
                        TM.TAPE_ALPHABET.insert(substring);
                    }
                    //insert input alphabet to tape alphabet
                    for (auto i = TM.INPUT_ALPHABET.begin(); i != TM.INPUT_ALPHABET.end(); ++i)
                    {
                        TM.TAPE_ALPHABET.insert(*i);
                    }
                }
                else if (file_state == FILE_STATES[2])
                {
                    TM.BLANK = line[0]; // BLANK is a single character
                    TM.TAPE_ALPHABET.insert(TM.BLANK);
                }
                else if (file_state == FILE_STATES[3])
                {
                    istringstream iss(line);
                    string substring;
                    while (iss >> substring)
                    {
                        TM.STATES.insert(substring);
                    }
                }
                else if (file_state == FILE_STATES[4])
                {
                    TM.START = line;
                }
                else if (file_state == FILE_STATES[5])
                {
                    TM.ACCEPT = line;
                }
                else if (file_state == FILE_STATES[6])
                {
                    TM.REJECT = line;
                }
                else if (file_state == FILE_STATES[7])
                {
                    istringstream iss(line);
                    string input1, input2, input3, input4, input5;
                    if (!(iss >> input1 >> input2 >> input3 >> input4 >> input5))
                    {
                        break;
                    } // error
                    TM.TRANSITIONS[input1][input2]["overwrite_symbol"] = input3;
                    TM.TRANSITIONS[input1][input2]["direction"] = input4;
                    TM.TRANSITIONS[input1][input2]["next_state"] = input5;
                }
                else if (file_state == FILE_STATES[8])
                {
                    INPUT_STRING = line; // BLANK is a single character
                }
            }
        }
        file.close();
    }
    return TM;
}

// write a method to print TuringMachine
void printTM(TuringMachine TM)
{
    cout << "INPUT ALPHABET: ";
    for (auto i = TM.INPUT_ALPHABET.begin(); i != TM.INPUT_ALPHABET.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "TAPE ALPHABET: ";
    for (auto i = TM.TAPE_ALPHABET.begin(); i != TM.TAPE_ALPHABET.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "BLANK SYMBOL: " << TM.BLANK << endl;

    cout << "STATES: ";
    for (auto i = TM.STATES.begin(); i != TM.STATES.end(); ++i)
        cout << *i << " ";
    cout << endl;

    cout << "START: " << TM.START << endl;

    cout << "ACCEPT: " << TM.ACCEPT << endl;

    cout << "REJECT: " << TM.REJECT << endl;

    cout << "TRANSITIONS: " << endl;
    for (auto i = TM.TRANSITIONS.begin(); i != TM.TRANSITIONS.end(); ++i)
    {
        for (auto j = i->second.begin(); j != i->second.end(); ++j)
        {
            cout << i->first << " " << j->first << " " << j->second["overwrite_symbol"] << " " << j->second["direction"] << " " << j->second["next_state"] << endl;
        }
    }

    // cout << "STRING: " << INPUT_STRING<< endl;
}

auto simulate_tm(TuringMachine TM, string input_string)
{
    // initialize tape
    for (auto i : input_string)
    {
        // convert i to string
        string s(1, i);
        TM.TAPE.push_back(s);
    }
    auto tape_head = TM.TAPE.begin();

    vector<string> route = {TM.START};
    auto current_state = TM.START;

    // write while loop until current_state is accept or reject
    while (current_state != TM.ACCEPT && current_state != TM.REJECT)
    {
        // get current symbol
        string current_symbol = *tape_head;

        // get next state
        auto overwrite_symbol = TM.TRANSITIONS[current_state][current_symbol]["overwrite_symbol"];
        auto direction = TM.TRANSITIONS[current_state][current_symbol]["direction"];
        auto next_state = TM.TRANSITIONS[current_state][current_symbol]["next_state"];

        *tape_head = overwrite_symbol;
        if (direction == "R")
        {
            tape_head++;
            if (tape_head == TM.TAPE.end())
            {
                TM.TAPE.push_back(TM.BLANK);
                tape_head = TM.TAPE.end();
                tape_head--;
            }
        }
        else if (direction == "L")
        {
            if (tape_head == TM.TAPE.begin())
            {
                // stay at the same position
            }
            else
            {
                tape_head--;
            }
        }
        else
        {
            throw invalid_argument("[ERR]Invalid direction.");
        }
        current_state = next_state;
        route.insert(route.end(), current_state);
    }

    struct returnValues
    {
        vector<string> route;
        string accept_or_reject;
    };

    return returnValues{route, current_state};
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

        TuringMachine TM = FileReader("Input_İSMAİLCAN_YAĞMUR_S018266.txt");
    

        cout << "----------------Turing Machine Content----------------" << endl;
        printTM(TM);

        auto result = async(simulate_tm, TM, INPUT_STRING);

        if (result.wait_for(std::chrono::seconds(TIMEOUT_IN_SECONDS)) == std::future_status::timeout)
        {
            cout << "------------------------------------------------" << endl;
            cout<<"Loop Detected!"<<endl;
            cout<<"Exiting..."<<endl;
            throw runtime_error("[ERR]Timeout!");
        }
        else
        {
            auto results = result.get();

            // print results
            cout << "----------------Simulation Results----------------" << endl;
            cout << "ROUT: ";
            for (auto i = results.route.begin(); i != results.route.end(); ++i)
            {
                cout << *i << " ";
            }
            cout << endl;

            // if qR then write reject else write accept
            if (results.accept_or_reject == TM.REJECT)
            {
                cout << "RESULT: REJECT" << endl;
            }
            else if (results.accept_or_reject == TM.ACCEPT)
            {
                cout << "RESULT: ACCEPT" << endl;
            }
            else
            {
                throw runtime_error("[ERR]Invalid result.");
            }
        }
        cout<<"----------------End of Simulation----------------"<<endl;


    return 0;
}