#ifndef __HEADER
#define __HEADER

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stack>
#include <set>
#include <algorithm>

using namespace std;

class Error{
    private:
        int id;
        string error;
    
    public:
        Error(int eid, string err);
        void print();
};

class Relation{
    private:
        vector<string> Column_Names;
        vector<string> Data_Types;
        vector<vector<string>> Tuples;
        vector<int> max_width;
    
    public:
        string Name;

        Relation(string name);
        vector<vector<string>> get_Columns();
        vector<vector<string>> get_Tuples();

        void set_Column_Names(vector<string> cname);
        void set_Column_Types(vector<string> ctype);
        void insert_Tuple(vector<string> tuple);
        void rename(string name);
        void print();
};

class Database{
    private:
        map<string, Relation*> Tables;

    public:
        Database(string path);
        void load_tables();
        void reset(string path);
        void add(Relation table);

        vector<string> get_TableNames();
        Relation get_Table(string name);
};

int Precedence(string op);
bool isOperator(string s);
string getType(string s, vector<vector<string>> columns);
string doOperation(string a, string b, string op, vector<vector<string>> columns, vector<string> tuple);
vector<string> split(string line, char c);
vector<string> toPostfix(string predicate);
set<vector<string>> tupleSatisfies(vector<string> postfix, vector<vector<string>> tuples, vector<vector<string>> columns);

Relation Query_Parser(string query);

vector<string> SEL_Predicate(string predicate);
Relation SELECT(string query);

vector<string> PRO_Predicate(string predicate);
Relation PROJECT(string query);

int comma_pos(string query);
Relation UNION(string query);
Relation DIFFERENCE(string query);
Relation PRODUCT(string query);

vector<string> RNM_Predicate(string predicate);
Relation RENAME(string query);

#endif //__HEADER