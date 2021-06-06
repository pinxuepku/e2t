#include<string>
#include<vector>
#include "tree.hpp"

using namespace std;

class Symbol{
    public:
        string name;
        bool is_global;
        bool is_array;
        int global_index;
        int stack_location;
        int array_size;
        int stack_size;
        int param_index;
        int reg;
};

static vector<Symbol*> global;