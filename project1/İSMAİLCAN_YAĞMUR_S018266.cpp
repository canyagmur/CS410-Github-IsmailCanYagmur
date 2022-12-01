#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>


std::vector<std::string> FILE_STATES{ "ALPHABET","STATES","START","FINAL","TRANSITIONS","END"};
std::string VOID_STATE_STR = "SINK";
std::set<std::string> VOID_STATE({VOID_STATE_STR});

typedef std::unordered_set<std::string> SetOfStrings;
typedef std::unordered_map<std::string, std::unordered_map<std::string, std::set<std::string>>> TransitionFunction;

struct FiniteAutomata {
SetOfStrings ALPHABET;
SetOfStrings STATES;
SetOfStrings START; //CHANGED!
SetOfStrings FINAL;
TransitionFunction TRANSITIONS;
bool isNFA;
};

auto FileReader(std::string file_path){

        std::cout<< "[INFO]Reading the file : "<<file_path << std::endl;
        FiniteAutomata NFA;
        NFA.isNFA = true;

        std::ifstream file(file_path);

        if (!file.is_open()) {
            throw std::runtime_error("[ERR]File not found.");
        }

        else {

            std::string line;
            std::string file_state;
  
           while (std::getline(file, line)) {
                line.erase(std::remove_if(line.end()-1, line.end(), ::isspace), line.end()); //to capture each line properly
                if (std::find(FILE_STATES.begin(), FILE_STATES.end(), line) != FILE_STATES.end()){
                    // FILE_STATE found
                    file_state = line;
                }
                else{
                    if(file_state == FILE_STATES[0]){
                        NFA.ALPHABET.insert(line);
                    }
                    else if(file_state == FILE_STATES[1]){
                        NFA.STATES.insert(line);
                    }
                    else if(file_state == FILE_STATES[2]){
                        NFA.START.insert(line);
                    }
                    else if(file_state == FILE_STATES[3]){
                        NFA.FINAL.insert(line);
                    }
                    else if(file_state == FILE_STATES[4]){
                        std::istringstream iss(line);
                        std::string input1,input2,input3;
                        if (!(iss >> input1 >> input2 >> input3)) { break; } // error
                        NFA.TRANSITIONS[input1][input2].insert(input3);
                    } 
                }   
                


            }
            file.close();
        }
 return NFA;
    
}


template <typename SetType>
std::string set_to_string(const SetType set) {
        if (set.empty()) {
            return "Ø";
        } 
         else {
            std::string str = "";
            for (const auto& item : set) {
                str += item;
            }
            return str;
        }
    }
auto checkTransitionExist(std::unordered_map<std::string, std::unordered_map<std::string, std::set<std::string>>> transition, std::string state, std::string member) {
                if (transition.find(state) == transition.end()) 
            {
                // not found
                return false;
            } 
            else 
            {
                    if (transition[state].find(member) == transition[state].end()) 
                    {
                    // not found
                    return false;
                    }
                    else
                    {
                        return true;
                    } 
            }
}
auto print(FiniteAutomata fa){

        std::cout << "ALPHABET" << std::endl;
        for (const auto member : fa.ALPHABET) {
            std::cout << member << std::endl;
        }
        std::cout << "STATES" << std::endl;
        for (const auto state : fa.STATES) {
            std::cout << state << std::endl;
        }
        std::cout << "START" << std::endl;
        for (const auto start : fa.START) {
            std::cout << start << std::endl;
        }
        std::cout << "FINAL" << std::endl;
        for (const auto fstate : fa.FINAL) {
            std::cout << fstate << std::endl;
        }
        std::cout << "TRANSITIONS" << std::endl;
        for (const auto state : fa.STATES) {
            for (const auto member : fa.ALPHABET) {
                    if(checkTransitionExist(fa.TRANSITIONS,state,member)){
                        if(fa.isNFA){
                            for (const auto transition : fa.TRANSITIONS[state][member]) {
                                std::cout << state << " " << member << " " << transition << std::endl;
                            }
                        }
                        else{
                        std::cout << state << " " << member << " " << set_to_string(fa.TRANSITIONS[state][member]) << std::endl;
                        }
                    }
                    
            }
        }
        std::cout << "END" << std::endl;
}

auto printTransitionTable(FiniteAutomata fa){
    for(auto i : fa.ALPHABET){
        std::cout<<"\t"<<i;
    }
    std::cout<<std::endl;
    for(auto i : fa.STATES){
        std::cout<<i;
        for(auto j : fa.ALPHABET){

            if(checkTransitionExist(fa.TRANSITIONS,i,j)){
                std::cout<<"\t"<<set_to_string(fa.TRANSITIONS[i][j]);
            }
            else{
                std::cout<<"\t"<<"Ø";
            }         
        }
        std::cout<<std::endl;
    }
}

template <typename StateType>
void FillDFA(StateType state,FiniteAutomata & dfa,TransitionFunction old_transitions,SetOfStrings final_states)
{
    auto state_str = set_to_string(state); //convert set to string

    if (dfa.STATES.find(state_str) == dfa.STATES.end()) //if state is not in states
    { 
        dfa.STATES.insert(state_str); //insert state to states
        if(state_str == VOID_STATE_STR){ //if state is sink state
            for(auto a : dfa.ALPHABET){ //for each alphabet member
                dfa.TRANSITIONS[state_str][a].insert(VOID_STATE_STR); //add transition to sink state
            }
        }
        else
        {
            for (auto a : dfa.ALPHABET) //iterate over alphabet
            {
                std::set<std::string> new_transition_acc; //new transition set of strings accumulator
                for(auto s : state) //iterate over state to get unit states
                {
                    if(final_states.find(set_to_string(s)) != final_states.end()){ //if any unit state is final state
                        dfa.FINAL.insert(state_str); //assign main state as final state
                    }
                    if(checkTransitionExist(old_transitions,s,a)) //check whether transition exist for given unit state and member of alphabet
                    {
                        auto mytransition = old_transitions[s][a]; //get transition
                        new_transition_acc.insert(mytransition.begin(),mytransition.end()); //add transition to accumulator
                    }

                }
                auto new_transition_acc_str = set_to_string(new_transition_acc); //convert transition set to string
                if(!new_transition_acc.empty()) //if accumulated transition set is not empty
                {
                    
                    dfa.TRANSITIONS[state_str][a] = new_transition_acc; //add transition  to DFA
                }
                else
                {
                    dfa.TRANSITIONS[state_str][a] = VOID_STATE; //add sink transition to DFA
                }
                if(dfa.STATES.find(new_transition_acc_str) == dfa.STATES.end()) //check whether new transition is not in DFA states.
                {
                    if(!new_transition_acc.empty()){ //if accumulated transition set is not empty
                        FillDFA(new_transition_acc,dfa,old_transitions,final_states); //fill DFA with new transition
                    }
                    else{
                        FillDFA(VOID_STATE,dfa,old_transitions,final_states); //fill DFA with sink state
                    }
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    // if (argc < 2)
    // {
    //     // print usage

    //     std::cerr << "[ERR]Expected filename as an argument.\n[INFO]Exiting... " << std::endl;
    //     return 1;
    // }
    // std::string file_path = argv[1];

    try {
        auto NFA = FileReader("NFA1.txt");
        std::cout<<"<<<<<<< NFA >>>>>>>"<<std::endl;
        print(NFA);
        FiniteAutomata DFA;
        DFA.ALPHABET = NFA.ALPHABET;
        DFA.START = NFA.START;
        DFA.isNFA = false;
        FillDFA(DFA.START,DFA,NFA.TRANSITIONS,NFA.FINAL);
        std::cout<<"<<<<<<< DFA >>>>>>>"<<std::endl;
        print(DFA);

   } catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}