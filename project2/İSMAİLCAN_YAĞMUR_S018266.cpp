#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

vector<string> FILE_STATES{"NON-TERMINAL", "TERMINAL", "RULES", "START"};
vector<string> LETTER_POOL{"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

struct CFG
{
    vector<string> NON_TERMINAL;
    vector<string> TERMINAL;
    map<string, set<string>> RULES;
    string START;
    vector<string> insertionOrderOfRules;
};

auto FileReader(string file_path)
{

    cout << "[INFO]Reading the file : " << file_path << endl;

    CFG cfg;
    ifstream file(file_path);

    if (!file.is_open())
    {
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
                    cfg.NON_TERMINAL.push_back(line);
                }
                else if (file_state == FILE_STATES[1])
                {
                    cfg.TERMINAL.push_back(line);
                }
                else if (file_state == FILE_STATES[2])
                {
                    // cout<<line<<endl;
                    string delimiter = ":";
                    size_t pos = 0;
                    std::string token;
                    while ((pos = line.find(delimiter)) != std::string::npos)
                    {
                        token = line.substr(0, pos);
                        // std::cout << token << std::endl;
                        line.erase(0, pos + delimiter.length());
                    }
                    cfg.RULES[token].insert(line);
                    if (find(cfg.insertionOrderOfRules.begin(), cfg.insertionOrderOfRules.end(), token) == cfg.insertionOrderOfRules.end())
                        cfg.insertionOrderOfRules.push_back(token);
                    // std::cout << line << std::endl;
                }
                else if (file_state == FILE_STATES[3])
                {
                    cfg.START = line;
                }
            }
        }

        file.close();
    }

    // remove all LETTER_POOL coming from cfg definition from the LETTER_POOL pool.
    for (auto a : cfg.NON_TERMINAL)
    {
        LETTER_POOL.erase(remove(LETTER_POOL.begin(), LETTER_POOL.end(), a), LETTER_POOL.end());
    }
    for (auto a : cfg.TERMINAL)
    {
        LETTER_POOL.erase(remove(LETTER_POOL.begin(), LETTER_POOL.end(), a), LETTER_POOL.end());
    }
    LETTER_POOL.erase(remove(LETTER_POOL.begin(), LETTER_POOL.end(), cfg.START), LETTER_POOL.end());
    // remove all LETTER_POOL coming from cfg definition from the LETTER_POOL pool.

    return cfg;
}

auto print_rules(CFG cfg)
{
    for (auto rule : cfg.insertionOrderOfRules)
    {
        cout << rule << "->";
        for (auto rule_rhs : cfg.RULES[rule])
        {
            cout << rule_rhs << "|";
        }
        cout << endl;
    }
}
auto print_rules_in_file_format(CFG cfg)
{
    for (auto rule : cfg.insertionOrderOfRules)
    {
        for (auto rule_rhs : cfg.RULES[rule])
        {
            cout << rule << ":" << rule_rhs << endl;
        }
    }
}

auto print_cfg(CFG cfg)
{
    cout << "NON_TERMINAL" << endl;
    for (auto non_terminal : cfg.NON_TERMINAL)
        cout << non_terminal << endl;
    cout << "TERMINAL" << endl;
    for (auto terminal : cfg.TERMINAL)
        cout << terminal << endl;
    cout << "RULES" << endl;
    print_rules_in_file_format(cfg);
    cout << "START" << endl;
    cout << cfg.START << endl;
}

vector<int> findLocation(string sample, char findIt)
{
    vector<int> characterLocations;
    for (int i = 0; i < sample.size(); i++)
        if (sample[i] == findIt)
            characterLocations.push_back(i);

    return characterLocations;
}

void permute_recursive(set<string> &permutations, set<string> local_permutations, vector<int> indexes_of_key_variables, int index_counter)
{
    if (index_counter == indexes_of_key_variables.size())
    {
        return;
    }
    else
    {
        for (auto permuted_string : permutations)
        {
            // cout<<index_counter<<endl;
            string temp = permuted_string;
            temp[indexes_of_key_variables[index_counter]] = 'E';
            permutations.insert(temp);
            permutations.insert(permuted_string);
            local_permutations.clear();
            local_permutations.insert(temp);
            local_permutations.insert(permuted_string);
            permute_recursive(permutations, local_permutations, indexes_of_key_variables, index_counter + 1);
        }
    }
}
auto update_rule_by_removing_epsilon(string key_variable, string string_containing_key_variable)
{

    // if A->e, then key_variable is A and string_containing_key_variable is a string containing the key variable in RHS of the rules.
    char findIt = key_variable[0]; // key variable is assumed to
    auto indexes_of_key_variables_in_string = findLocation(string_containing_key_variable, findIt);
    set<string> all_permutations;
    if (indexes_of_key_variables_in_string.size() == string_containing_key_variable.size())
    {
        all_permutations.insert("e");
    }
    else
    {
        set<string> all_permutations_temp;
        all_permutations_temp.insert(string_containing_key_variable);
        permute_recursive(all_permutations_temp, all_permutations_temp, indexes_of_key_variables_in_string, 0);

        for (auto str : all_permutations_temp)
        {
            str.erase(std::remove(str.begin(), str.end(), 'E'), str.end());
            all_permutations.insert(str);
        }
    }

    return all_permutations;
}

auto search_rules_for_epsilon(CFG cfg)
{

    set<string> rules_containing_epsilon;
    for (auto rule : cfg.RULES)
    {

        for (auto rule_rhs : rule.second)
        {
            if (rule_rhs == "e")
                rules_containing_epsilon.insert(rule.first);
        }
    }
    // rules_containing_epsilon.erase(cfg.START);
    return rules_containing_epsilon;
}
auto search_rules_for_AB(CFG cfg)
{

    vector<string> rules_containing_AB;
    for (auto rule : cfg.RULES)
    {
        for (auto rule_rhs : rule.second)
        {
            if (cfg.NON_TERMINAL.end() != find(cfg.NON_TERMINAL.begin(), cfg.NON_TERMINAL.end(), rule_rhs)) // if the rule contains  single non terminal in RHS.
                rules_containing_AB.push_back(rule.first);
        }
    }
    return rules_containing_AB;
}

auto search_rules_for_more_than_2_non_terminals_in_rhs(CFG cfg_in_cnf)
{
    map<string, set<string>> rules_with_more_than_2_non_terminals_reversed_map;
    for (auto rule : cfg_in_cnf.RULES)
    {
        for (auto rule_rhs : rule.second)
        {
            if (rule_rhs.size() > 2) // check only strings that have length larger than 2
            {
                int non_terminal_counter = 0;
                for (auto non_terminal_str : cfg_in_cnf.NON_TERMINAL)
                {
                    // if(non_terminal_str == cfg_in_cnf.START)
                    // continue;
                    non_terminal_counter += count(rule_rhs.begin(), rule_rhs.end(), non_terminal_str[0]);
                }
                if (non_terminal_counter > 2)
                {
                    rules_with_more_than_2_non_terminals_reversed_map[rule_rhs].insert(rule.first);
                }
            }
        }
    }
    return rules_with_more_than_2_non_terminals_reversed_map;
}

auto search_rules_having_minlength_2_and_containing_atleast_one_terminal(CFG cfg_in_cnf)
{
    map<string, map<string, set<string>>> terminals_with_minlength_2_and_containing_atleast_one_terminal;
    for (auto terminal : cfg_in_cnf.TERMINAL)
    {

        for (auto rule : cfg_in_cnf.RULES)
        {
            for (auto rule_rhs : rule.second)
            {
                if (rule_rhs.size() > 1)
                {
                    if (rule_rhs.find(terminal) != string::npos)
                    {
                        terminals_with_minlength_2_and_containing_atleast_one_terminal[terminal][rule.first].insert(rule_rhs);
                    }
                }
            }
        }
    }

    return terminals_with_minlength_2_and_containing_atleast_one_terminal;
}

auto rule0_add_new_start_variable(CFG &cfg_in_cnf, string old_start)
{
    auto new_varcharacter_for_start = LETTER_POOL.back();
    LETTER_POOL.pop_back();
    cfg_in_cnf.START = new_varcharacter_for_start;
    cfg_in_cnf.NON_TERMINAL.push_back(cfg_in_cnf.START);
    cfg_in_cnf.RULES[cfg_in_cnf.START].insert(old_start);
    cfg_in_cnf.insertionOrderOfRules.insert(cfg_in_cnf.insertionOrderOfRules.begin(), cfg_in_cnf.START);
    return cfg_in_cnf;
}
auto rule1_remove_epsilon_productions(CFG &cfg_in_cnf)
{
    while (1)
    {
        auto rules_containing_epsilon = search_rules_for_epsilon(cfg_in_cnf);
        rules_containing_epsilon.erase(cfg_in_cnf.START);
        if (rules_containing_epsilon.empty())
        {
            break;
        }
        else
        {
            for (auto rule_with_empty_string : rules_containing_epsilon)
            {
                for (auto r : cfg_in_cnf.RULES)
                {
                    for (auto rule_rhs : r.second)
                    {
                        if (rule_rhs.find(rule_with_empty_string) != std::string::npos) // if the other_rule_rhs contains the rule_with_empty_string
                        {
                            auto new_rules = update_rule_by_removing_epsilon(rule_with_empty_string, rule_rhs);
                            cfg_in_cnf.RULES[r.first].insert(new_rules.begin(), new_rules.end());
                        }
                    }
                }
                cfg_in_cnf.RULES[rule_with_empty_string].erase("e");
            }
        }
    }
}
auto rule2_remove_unit_productions(CFG &cfg_in_cnf)
{
    while (1)
    {
        auto rules_containing_AB = search_rules_for_AB(cfg_in_cnf);
        if (rules_containing_AB.empty())
        {
            break;
        }
        else
        {
            for (auto rule_lhs_with_AB : rules_containing_AB)
            {
                for (auto rhs : cfg_in_cnf.RULES[rule_lhs_with_AB]) // remove all the rules of the form A->A
                {
                    if (rhs == rule_lhs_with_AB)
                    {
                        cfg_in_cnf.RULES[rule_lhs_with_AB].erase(rhs);
                    }
                }

                for (auto rhs : cfg_in_cnf.RULES[rule_lhs_with_AB])
                {
                    if (cfg_in_cnf.NON_TERMINAL.end() != find(cfg_in_cnf.NON_TERMINAL.begin(), cfg_in_cnf.NON_TERMINAL.end(), rhs)) // if the rule  contains  single non terminal in RHS.
                    {
                        auto new_rules = cfg_in_cnf.RULES[rhs];
                        cfg_in_cnf.RULES[rule_lhs_with_AB].erase(rhs);
                        cfg_in_cnf.RULES[rule_lhs_with_AB].insert(new_rules.begin(), new_rules.end());
                    }
                }
            }
        }
    }
}
auto rule3_change_terminals(CFG &cfg_in_cnf)
{
    auto desired_terminal_rule_mapping = search_rules_having_minlength_2_and_containing_atleast_one_terminal(cfg_in_cnf);
    for (auto mymapping : desired_terminal_rule_mapping)
    {
        auto terminal = mymapping.first;
        auto new_varcharacter_for_terminal = LETTER_POOL.back();
        LETTER_POOL.pop_back();
        for (auto rule : mymapping.second)
        {
            auto rule_lhs = rule.first;

            auto temp = cfg_in_cnf.RULES[rule_lhs];
            for (auto x : temp)
            {
                if (x.size() > 1)
                {
                    if (x.find(terminal) != string::npos)
                    {

                        auto new_string = x;
                        replace(new_string.begin(), new_string.end(), terminal[0], new_varcharacter_for_terminal[0]); // replace all terminalchars to newchar
                        cfg_in_cnf.RULES[rule_lhs].erase(x);
                        cfg_in_cnf.RULES[rule_lhs].insert(new_string);
                    }
                }
            }
        }

        cfg_in_cnf.RULES[new_varcharacter_for_terminal].insert(terminal);
        cfg_in_cnf.insertionOrderOfRules.push_back(new_varcharacter_for_terminal);
        cfg_in_cnf.NON_TERMINAL.push_back(new_varcharacter_for_terminal);
    }
}
auto rule4_change_long_rules(CFG &cfg_in_cnf)
{
    while (1)
    {
        auto rules_with_more_than_2_non_terminals_reversed_map = search_rules_for_more_than_2_non_terminals_in_rhs(cfg_in_cnf);
        if (rules_with_more_than_2_non_terminals_reversed_map.empty())
        {
            break;
        }
        else
        {
            for (auto myrule : rules_with_more_than_2_non_terminals_reversed_map)
            {
                auto rule_rhs = myrule.first;
                auto new_non_terminal_first_character = rule_rhs.substr(0, 1);
                auto new_non_terminal_second_part = rule_rhs.substr(1, -1);
                auto new_non_terminal_character = LETTER_POOL.back();
                LETTER_POOL.pop_back();
                auto combined_new_rule = new_non_terminal_first_character + new_non_terminal_character;
                for (auto rule_lhs : myrule.second)
                {
                    cfg_in_cnf.RULES[rule_lhs].erase(rule_rhs);
                    cfg_in_cnf.RULES[rule_lhs].insert(combined_new_rule);
                }
                cfg_in_cnf.RULES[new_non_terminal_character].insert(new_non_terminal_second_part);
                cfg_in_cnf.insertionOrderOfRules.push_back(new_non_terminal_character);
                cfg_in_cnf.NON_TERMINAL.push_back(new_non_terminal_character);
            }
        }
    }
}

auto make_cfg_in_cnf(CFG cfg)
{
    // Print the rules of the original CFG.
    cout << "----------------------------Rules of the original CFG----------------------------" << endl;
    print_rules(cfg);

    // 0. Add a new start variable
    auto cfg_in_cnf = cfg;
    rule0_add_new_start_variable(cfg_in_cnf, cfg.START);
    cout << "----------------------------Rules of the CFG AFTER New Start State is Added----------------------------" << endl;
    print_rules(cfg_in_cnf);

    // 1. Remove all the rules containing epsilon
    rule1_remove_epsilon_productions(cfg_in_cnf);
    cout << "----------------------------Rules of the CFG AFTER Epsilon Rule----------------------------" << endl;
    print_rules(cfg_in_cnf);

    // 2. Remove all the rules in the form of A-B where A and B are non-terminals
    rule2_remove_unit_productions(cfg_in_cnf);
    cout << "----------------------------Rules of the CFG AFTER A->B Rule----------------------------" << endl;
    print_rules(cfg_in_cnf);

    // 3.Find out all the rules having length of RHS > 1 and at least 1 terminal.
    rule3_change_terminals(cfg_in_cnf);
    cout << "----------------------------Rules of the CFG AFTER Simplyfing Terminal Cases----------------------------" << endl;
    print_rules(cfg_in_cnf);

    // 4.Find out all the rules having more than 2 non-terminals in RHS.
    rule4_change_long_rules(cfg_in_cnf);
    cout << "----------------------------Rules of the CFG AFTER Simplyfing More Than TWO Variables Cases----------------------------" << endl;
    print_rules(cfg_in_cnf);

    return cfg_in_cnf;
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

    try
    {
        auto cfg = FileReader("G1.txt");

        auto cfg_in_cnf = make_cfg_in_cnf(cfg);

        cout << "----------------------------New CFG in file format----------------------------" << endl;
        print_cfg(cfg_in_cnf);
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}