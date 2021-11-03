#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

std::vector<std::string>split(std::string str, char delim) {
    std::stringstream ss(str);
    std::string mallocr;
    std::vector<std::string>stff{};
    while (getline(ss, mallocr, delim)) {
        stff.push_back(mallocr);
    }
    return stff;
}

std::map<std::string, std::string> varmem {  
};

/// (help builtin <func)
/// (help copyright ...)
/// 
std::string b_help(std::vector<std::string>argv) 
{
    if (argv.size() >= 1) {
        if (argv[0] == "builtin") {
            if (argv[1] == "print") {
                std::cout << "<print> is a utility function to print the first argument to stdout.\nprint is not a buffer allocator; it just dummy prints the text given.\nit returns nothing." << std::endl;
            } else if (argv[1] == "let") {
                std::cout << "<let> is a keyword which adds a value with <name> to memory.\nAll keywords are functions - all functions are keywords.\nb_let is the name of the <let> keyword function. This is a utility function." << std::endl;
            } else if (argv[0] == "copyright") {
                std::cout << "HID is licensed under the MIT license.\n";
            }
        }
    } 
    return "";
}

std::string b_let(std::vector<std::string>argv) {
    if (argv.size() >= 2) {
        // std::cout << "var" << std::endl;
        varmem[argv[0]] = argv[1];
        // std::cout << varmem[argv[0]] << std::endl;
    }
    return "null";
}

std::string b_print(std::vector<std::string>argv) {
    for (const auto& i : argv) {
        std::cout << i;
    }

    std::cout << std::endl;
    return "Nothing";
}

std::map<std::string, std::string(*)(std::vector<std::string>)> funcs = {
    { "help", b_help },
    { "print", b_print },
    { "let" , b_let}
};

class HidObject {
    public:
        int state = -1;
        std::string key;
        std::vector<std::string> fargs;
        bool error;
        std::string msg;

};

std::string checkVar(std::string const& name) {
    if (varmem.find(name) != varmem.end()) {
        return varmem[name];
    }
    return "null";
}

/// print myhid
/// done
HidObject *CompileHid(std::string const& sta) {
    HidObject* hobj = new HidObject();

    hobj->state = 0;

    std::string st = "";
    std::vector<std::string>Af;

    for(int i = 0; i < sta.length() ; ++ i) {
        // std::cout << "Char " << sta[i] << std::endl;

        // if (hobj->state == 3) {
        //     std::cout << "strchar " << sta[i] << std::endl;
        // }
        if (sta[i] == '(' && hobj->state == 0) {
            // std::cout << "Entering object" << std::endl; 
            hobj->state = 1;
        }
        else if (sta[i] == ' ' && hobj->state == 1) {
            hobj->key = st;
            st = "";
            hobj->state = 2;
        } else if (sta[i] == '"' && hobj->state == 2) {
            // std::cout << "Entering string" << std::endl;
            hobj->state = 3;
        } else if (sta[i] == '"' && hobj->state == 3) {
            // std::cout << "Closing string " << st << std::endl;
            hobj->state = 2;
        } else if (sta[i] == ' ' && hobj->state == 2) {
            // std::cout << sta[sta.length()-1] << ": is the last char in string" << std::endl;
            // std::cout << st << ": prod" << std::endl;
            Af.push_back(st);
            st = "";
        } else if (sta[i] == ')' && hobj->state != 3) {
            // std::cout << "Found eos: " << sta[i] << std::endl;
            // std::cout << "EOS" << ::endl;
            if (hobj->key.empty()) {
                // std::cout << "warning: no arguments passed " << std::endl;
                hobj->key = st;
                st = "";
            }
            if (!st.empty()) {
                Af.push_back(st);
                st = "";
            }
            break;
        }
        else {
            st = st + sta[i];
        }
    }

    if (hobj->key.empty()) {
        hobj->key = st;
    }

    // for (const auto& i : Af) {
    //     std::cout << i << std::endl;
    // }
    for (int ic = 0; ic < Af.size(); ++ ic) {
        std::string i = Af[ic];
        if (checkVar(i) != "null") {
            Af[ic] = checkVar(i);
        } else {
            // Af[ic] = "null";
        }
    }
    hobj->fargs = Af;
    return hobj;
}

int Interp_Hid(std::string const& s) {
    HidObject *obj = CompileHid(s);
    if (funcs.find(obj->key) != funcs.end()) {
funcs[obj->key](obj->fargs);
    }
    
    return 1;
}

int main(int argc, char* * argv) {
    if (argc > 1) {
        if (std::string(argv[1]) == "-" ) {
            std::cout << "hid: log: handling stdin" << std::endl;

            while (true) {
                    std::cout << ">>>";
                    std::string str;
                    getline(std::cin, str);
                    Interp_Hid(str);
            }
        } else {
            std::string file = argv[1];
            std::string stat;
            std::stringstream buf;
            std::ifstream f(file);

            if (!f) {
                std::cout << "hid: error: File not found." << std::endl;
            } else {
                while (getline(f, stat) ) {
                    if (stat[0] == '#') {
                        continue;
                    } else {
                        buf << stat;
                    }
                }
                std::string fcontents = buf.str();
                std::vector<std::string>Stats = split(fcontents, ';');

                 for (const auto& stat : Stats) {
                    if (stat.length() > 0) {
                        Interp_Hid(stat);
                    }
                 }
            }
        }
    } else {
        while (true) {
                    std::cout << ">>>";
                    std::string str;
                    getline(std::cin, str);
                    Interp_Hid(str);
            }
    }
    
}