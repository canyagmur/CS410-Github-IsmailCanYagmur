#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


using namespace std;

vector<string> FILE_STATES{ "NON-TERMINAL","TERMINAL","RULES","START"};
struct CFG {
    vector<string> NON_TERMINAL;
    vector<string> TERMINAL;
    map<string,vector<string>> RULES;
    string START;
};



auto FileReader(string file_path){

        cout<< "[INFO]Reading the file : "<<file_path << endl;

        CFG cfg;
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
                        cfg.NON_TERMINAL.push_back(line);
                    }
                    else if(file_state == FILE_STATES[1]){
                        cfg.TERMINAL.push_back(line);
                    }
                    else if(file_state == FILE_STATES[2]){
                        //cout<<line<<endl;
                        string delimiter = ":";
                        size_t pos = 0;
                        std::string token;
                        while ((pos = line.find(delimiter)) != std::string::npos) {
                            token = line.substr(0, pos);
                            //std::cout << token << std::endl;
                            line.erase(0, pos + delimiter.length());
                        }
                        cfg.RULES[token].push_back(line);
                        //std::cout << line << std::endl;
                    }
                    else if(file_state == FILE_STATES[3]){
                        cfg.START = line;
                    }
                        
                    }

                }   
                
            file.close();

            }
            return cfg; }

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
        auto cfg = FileReader("G1.txt");
        for(const auto& elem : cfg.RULES)
        {
        cout << elem.first << " : "<<endl;
        for(const auto& elem2 : elem.second)
        {
            cout << elem2 << endl;
        }
        }

   } catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}