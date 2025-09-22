#include "header.h"
using namespace std;

Database DB("./Database/Info.txt");


Error::Error(int eid, string err){
    id = eid;
    error = err;
}

void Error::print(){
    cout << "ERROR(" << id << "): " << error << endl;
}

Relation::Relation(string name){
    Name = name;
}

vector<vector<string>> Relation::get_Columns(){
    vector<vector<string>> result;
    vector<string> t1, t2;
    for(int i=0; i<Column_Names.size(); i++){
        t1.push_back(Column_Names[i]);
        t1.push_back(Name + "." + Column_Names[i]);
        t2.push_back(Data_Types[i]);
        t2.push_back(Data_Types[i]);
    }
    result.push_back(t1);
    result.push_back(t2);
    return result;
}

vector<vector<string>> Relation::get_Tuples(){
    return Tuples;
}

void Relation::set_Column_Names(vector<string> cname){
    Column_Names = cname;
    for(auto x: cname){
        max_width.push_back(x.length()+4);
    }
}

void Relation::set_Column_Types(vector<string> ctype){
    Data_Types = ctype;
}

void Relation::insert_Tuple(vector<string> tuple){
    Tuples.push_back(tuple);
    for(int i=0; i<tuple.size(); i++){
        max_width[i] = max(max_width[i], (int)tuple[i].length()+4);
    }
}

void Relation::rename(string name){
    Name = name;
}

void Relation::print(){
    int N = Column_Names.size();
    string s1, s2, s;
    s.append(N+1, '-');

    for(int i=0; i<N; i++){
        s1.clear();
        s1.append(max_width[i], '-');
        cout << s1;
    } cout << s << endl;

    cout << '|';
    for(int i=0; i<N; i++){
        int c1 = (max_width[i] - Column_Names[i].length())/2;
        int c2 = max_width[i] - Column_Names[i].length() - c1;

        s1.clear();
        s2.clear();
        s1.append(c1, ' ');
        s2.append(c2, ' ');
        cout << s1 << Column_Names[i] << s2 << '|';
    } cout << endl;

    for(int i=0; i<N; i++){
        s1.clear();
        s1.append(max_width[i], '-');
        cout << s1;
    } cout << s << endl;

    for(auto t: Tuples){
        cout << '|';
        for(int i=0; i<N; i++){
            int c1 = (max_width[i] - t[i].length())/2;
            int c2 = max_width[i] - t[i].length() - c1;

            s1.clear();
            s2.clear();
            s1.append(c1, ' ');
            s2.append(c2, ' ');
            cout << s1 << t[i] << s2 << '|';
        } cout << endl;
    }

    for(int i=0; i<N; i++){
        s1.clear();
        s1.append(max_width[i], '-');
        cout << s1;
    } cout << s << endl;

}

Database::Database(string path){
    ifstream info(path);
    string line;
    if(info.is_open()){
        while(getline(info, line)){
            if(line[0] == '#'){
                Tables[line.substr(1)] = new Relation(line.substr(1));
                string name = line.substr(1);
                getline(info, line);
                vector<string> ctype = split(line, ',');
                Tables[name]->set_Column_Types(ctype);
            }
        }
    }
    info.close();
    load_tables();
}

void Database::load_tables(){
    for(auto r: Tables){
        string n = r.first;

        ifstream data("./Database/Tables/" + n + ".txt");
        string line;
        if(data.is_open()){
            getline(data, line);
            vector<string> cname = split(line, ',');

            r.second->set_Column_Names(cname);
            while(getline(data, line)){
                vector<string> tuple = split(line, ',');
                r.second->insert_Tuple(tuple);
            }
        }
        data.close();
    }
}

void Database::reset(string path){
    Tables.clear();
    ifstream info(path);
    string line;
    if(info.is_open()){
        while(getline(info, line)){
            if(line[0] == '#'){
                Relation *tbl;
                tbl = new Relation(line.substr(1));
                Tables[line.substr(1)] = tbl;
            }
        }
    }
    info.close();
    load_tables();
}

void Database::add(Relation table){
    Tables[table.Name] = new Relation(table.Name);
}

vector<string> Database::get_TableNames(){
    vector<string> result;
    for(auto it: Tables){
        result.push_back(it.first);
    }
    return result;
}

Relation Database::get_Table(string name){
    Relation table("");
    for(auto it: Tables){
        if(it.first == name){
            table =  *it.second;
        }
    }
    return table;
}

int Precedence(string s){
    int result;
    if(s=="|")
        result = 1;
    if(s=="&")
        result = 2;
    if(s=="!"||s=="="||s=="<"||s==">")
        result = 3;
    if(s=="+"||s=="-")
        result = 4;
    if(s=="*"||s=="/")
        result = 5;
    
    return result;
}

Relation Query_Parser(string query){
    Relation table("");
    if(query[0] == '('){
        table = Query_Parser(query.substr(1));
        return table;
    }
    if(query.length() <= 3)
        throw Error(-1, "Invalid Query");
    
    string cmd = query.substr(0, 4);
    if(cmd == "SEL["){
        table = SELECT(query.substr(4));
    }
    else if(cmd == "PRO["){
        table = PROJECT(query.substr(4));
    }
    else if(cmd == "UNI("){
        table = UNION(query.substr(4));
    }
    else if(cmd == "DIF("){
        table = DIFFERENCE(query.substr(4));
    }
    else if(cmd == "CRP("){
        table = PRODUCT(query.substr(4));
    }
    else if(cmd == "RNM["){
        table = RENAME(query.substr(4));
    }
    else{
        int pos = 0;
        bool invalid = false;
        while(pos < query.length() && query[pos] != ')' && query[pos] != ','){
            if(!isalnum(query[pos])){
                invalid = true;
                break;
            }
            pos++;
        }

        if(invalid || pos > query.length()){
            throw Error(1, "Invalid Query");
        }
        
        invalid = true;
        string tname = query.substr(0, pos);
        for(auto t: DB.get_TableNames()){
            if(tname == t){
                table = DB.get_Table(tname);
                invalid = false;
                break;
            }
        }

        if(invalid)
            throw Error(-1, "Invalid Query");
    }

    return table;
}

bool isOperator(string s){
    return (s=="+"||s=="-"||s=="*"||s=="/"||s=="&"||s=="|"||s=="="||s=="<"||s==">"||s=="!");
}

string getType(string s, vector<vector<string>> columns){
    bool num = true;
    int point = 0;
    for(auto ch: s){
        if(ch == '.')
            point++;
        if(!isdigit(ch))
            num = false;
    }
    if(num && (point == 0)){
        return "int";
    }
    else if(num && (point == 1)){
        return "float";
    }
    else if(s[0] == '\'' && s[s.length()-1] == '\''){
        return "varchar";
    }

    int idx = find(columns[0].begin(), columns[0].end(), s) - columns[0].begin();
    if(idx < columns[0].size()){
        return columns[1][idx];
    }
    
    return "bool";
}

string doOperation(string a, string b, string op, vector<vector<string>> columns, vector<string> tuple){
    string result;
    string atype = getType(a, columns);
    string btype = getType(b, columns);
    if(atype != btype){
        throw Error(-1, "Invalid Predicate");
    }

    int idxa = find(columns[0].begin(), columns[0].end(), a) - columns[0].begin();
    int idxb = find(columns[0].begin(), columns[0].end(), b) - columns[0].begin();
    if(idxa < columns[0].size())
        a = tuple[idxa/2];
    if(idxb < columns[0].size())
        b = tuple[idxb/2];

    if(atype == "int"){
        int _a = stoi(a);
        int _b = stoi(b);
        if(op == "+")
            result = to_string(_a + _b);
        else if(op == "-")
            result = to_string(_a - _b);
        else if(op == "*")
            result = to_string(_a * _b);
        else if(op == "/")
            result = to_string(_a / _b);
        else if(op == "!")
            result = (_a != _b) ? "true" : "false";
        else if(op == "=")
            result = (_a == _b) ? "true" : "false";
        else if(op == "<")
            result = (_a < _b) ? "true" : "false";
        else if(op == ">")
            result = (_a > _b) ? "true" : "false";
        else
            throw Error(-1, "Invalid Operation");
    }
    else if(atype == "float"){
        float _a = stof(a);
        float _b = stof(b);
        if(op == "+")
            result = to_string(_a + _b);
        else if(op == "-")
            result = to_string(_a - _b);
        else if(op == "*")
            result = to_string(_a * _b);
        else if(op == "/")
            result = to_string(_a / _b);
        else if(op == "!")
            result = (_a != _b) ? "true" : "false";
        else if(op == "=")
            result = (_a == _b) ? "true" : "false";
        else if(op == "<")
            result = (_a < _b) ? "true" : "false";
        else if(op == ">")
            result = (_a > _b) ? "true" : "false";
        else
            throw Error(-1, "Invalid Operation");
        
    }
    else if(atype == "varchar"){
        string _a, _b;
        if(a[0] == '\'')
            _a = a.substr(1, a.length()-2);
        else
            _a = a;

        if(b[0] == '\'')
            _b = b.substr(1, b.length()-2);
        else
            _b = b;

        if(op == "=")
            result = (_a == _b) ? "true" : "false";
        else if(op == "!")
            result = (_a != _b) ? "true" : "false";
        else
            throw Error(-1, "Invalid Operation");
    }
    else{
        bool _a, _b;
        if(a=="true")
            _a = true;
        else if(a=="false")
            _a = false;
        else
            throw Error(-1, "Unknown Value");

        if(b=="true")
            _b = true;
        else if(b=="false")
            _b = false;
        else
            throw Error(-1, "Unknown Value");

        if(op == "&")
            result = (_a && _b) ? "true" : "false";
        else if(op == "|")
            result = (_a || _b) ? "true" : "false";
        else
            throw Error(-1, "Invalid Operation");
    }
    return result;
}

vector<string> split(string line, char c){
    vector<string> result;
    int lpos = 0;
    for(int i=0; i<line.length(); i++){
        if(line[i] == c){
            result.push_back(line.substr(lpos, i-lpos));
            lpos = i+1;
        }
    }
    result.push_back(line.substr(lpos, line.length()-lpos));
    return result;
}

vector<string> SEL_Predicate(string predicate){
    vector<string> result;
    string spchars = "(!<>=&|+-*/)";
    string s = "";
    for(int i=0; i<predicate.length(); i++){
        if(spchars.find(predicate[i]) != string::npos){
            if(s.length())
                result.push_back(s);
            s.clear();
            result.push_back(predicate.substr(i, 1));
            continue;
        }
        s.push_back(predicate[i]);
    }
    if(s.length())
        result.push_back(s);
    return result;
}

vector<string> PRO_Predicate(string predicate){
    vector<string> result;
    string spchars = ",";
    string s = "";
    for(int i=0; i<predicate.length(); i++){
        if(spchars.find(predicate[i]) != string::npos){
            if(s.length())
                result.push_back(s);
            s.clear();
            continue;
        }
        s.push_back(predicate[i]);
    }
    result.push_back(s);
    return result;
}

vector<string> RNM_Predicate(string predicate){
    vector<string> result;
    string spchars = ":,";
    string s = "";
    for(int i=0; i<predicate.length(); i++){
        if(spchars.find(predicate[i]) != string::npos){
            if(s.length())
                result.push_back(s);
            s.clear();
            continue;
        }
        s.push_back(predicate[i]);
    }
    result.push_back(s);
    return result;
}

int comma_pos(string query){
    int pos = -1, b = 0;
    for(int i=0; i<query.length(); i++){
        if(query[i] == '(')
            b++;
        if(query[i] == ')')
            b--;
        if(query[i] ==',' && b == 0){
            pos = i;
            return pos;
        }
    }
}

vector<string> toPostfix(string predicate){
    vector<string> postfix;
    vector<string> pred = SEL_Predicate(predicate);
    stack<string> st;
    for(auto s: pred){
        if(s == "("){
            st.push(s);
        }
        else if(s == ")"){
            while(st.top() != "("){
                postfix.push_back(st.top());
                st.pop();
            }
            st.pop();
        }
        else if(isOperator(s)){
            while(!st.empty() && st.top() != ")" && (Precedence(st.top()) > Precedence(s))){
                postfix.push_back(st.top());
                st.pop();
            }
            st.push(s);
        }
        else{
            postfix.push_back(s);
        }
    }
    while(!st.empty()){
        postfix.push_back(st.top());
        st.pop();
    }
    return postfix;
}

set<vector<string>> tupleSatisfies(vector<string> postfix, vector<vector<string>> tuples, vector<vector<string>> columns){
    set<vector<string>> result;
    for(auto t: tuples){
        stack<string> st;
        for(auto s: postfix){
            if(isOperator(s)){
                if(st.size() < 2){
                    throw Error(1, "Invalid Predicate");
                }
                string b = st.top();
                st.pop();
                string a = st.top();
                st.pop();

                string c = doOperation(a, b, s, columns, t);
                st.push(c);
            }
            else{
                st.push(s);
            }
        }
        if(st.size() != 1){
            throw Error(10, "Invalid Predicate");
        }
        if(st.top() == "true")
            result.insert(t);
        else if(st.top() == "false")
            continue;
        else
            throw Error(10, "Invalid Predicate");
    }
    return result;
}

Relation SELECT(string query){
    int pos = query.find(']');
    if(pos == string::npos){
        throw Error(-1, "Invalid Query");
    }
    string predicate = query.substr(0, pos);
    vector<string> postfix = toPostfix(predicate);
    if(query[pos+1] != '('){
        throw Error(-1, "Invalid Query");
    }
    Relation input = Query_Parser(query.substr(pos+2));
    vector<vector<string>> columns = input.get_Columns();
    vector<vector<string>> tuples = input.get_Tuples();
    
    set<vector<string>> result_tuples = tupleSatisfies(postfix, tuples, columns);
    vector<string> cname, ctype;
    Relation table("");
    for(int i=0; i<columns[0].size(); i+=2){
        cname.push_back(columns[0][i]);
        ctype.push_back(columns[1][i]);
    }
    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t: result_tuples){
        table.insert_Tuple(t);
    }
    return table;
}

Relation PROJECT(string query){
    int pos = query.find(']');
    if(pos == string::npos){
        throw Error(-1, "Invalid Query");
    }
    string predicate = query.substr(0, pos);
    vector<string> cols = PRO_Predicate(predicate);
    if(query[pos+1] != '('){
        throw Error(-1, "Invalid Query");
    }
    Relation input = Query_Parser(query.substr(pos+2));
    vector<vector<string>> columns = input.get_Columns();
    vector<vector<string>> tuples = input.get_Tuples();

    vector<int> idx;
    vector<string> cname, ctype;
    for(auto name: cols){
        if(find(columns[0].begin(), columns[0].end(), name) == columns[0].end())
            throw Error(-1, "Invalid Predicate");
        idx.push_back(find(columns[0].begin(), columns[0].end(), name) - columns[0].begin());
        cname.push_back(columns[0][find(columns[0].begin(), columns[0].end(), name) - columns[0].begin()]);
        ctype.push_back(columns[1][find(columns[0].begin(), columns[0].end(), name) - columns[0].begin()]);
    }
    vector<string> tuple;
    set<vector<string>> result_tuples;
    Relation table("");
    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t: tuples){
        tuple.clear();
        for(auto i: idx){
            tuple.push_back(t[i/2]);
        }
        result_tuples.insert(tuple);
    }
    for(auto t: result_tuples){
        table.insert_Tuple(t);
    }
    return table;
}

Relation UNION(string query){
    int pos = comma_pos(query);
    if(pos == -1){
        throw Error(-1, "Invalid Query");
    }
    string p1 = query.substr(0,pos);
    string p2 = query.substr(pos+1);

    Relation inp1 = Query_Parser(p1);
    Relation inp2 = Query_Parser(p2);

    vector<vector<string>> col1 = inp1.get_Columns();
    vector<vector<string>> col2 = inp2.get_Columns();
    vector<vector<string>> tup1 = inp1.get_Tuples();
    vector<vector<string>> tup2 = inp2.get_Tuples();

    if(col1[0].size() != col2[0].size()){
        throw Error(-1, "Number of Columns should be same");
    }
    for(int i=0; i<col1[0].size(); i+=2){
        if(col1[1][i] != col2[1][i]){
            throw Error(-1, "Column Types should Match");
        }
    }

    vector<string> cname, ctype;
    for(int i=0; i<col1[0].size(); i+=2){
        cname.push_back(col1[0][i]);
        ctype.push_back(col1[1][i]);
    }

    set<vector<string>> result_tuples;
    Relation table("");
    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t: tup1){
        result_tuples.insert(t);
    }
    for(auto t: tup2){
        result_tuples.insert(t);
    }
    for(auto t: result_tuples){
        table.insert_Tuple(t);
    }
    return table;
}

Relation DIFFERENCE(string query){
    int pos = comma_pos(query);
    if(pos == -1){
        throw Error(-1, "Invalid Query");
    }
    string p1 = query.substr(0,pos);
    string p2 = query.substr(pos+1);

    Relation inp1 = Query_Parser(p1);
    Relation inp2 = Query_Parser(p2);

    vector<vector<string>> col1 = inp1.get_Columns();
    vector<vector<string>> col2 = inp2.get_Columns();
    vector<vector<string>> tup1 = inp1.get_Tuples();
    vector<vector<string>> tup2 = inp2.get_Tuples();

    if(col1[0].size() != col2[0].size()){
        throw Error(-1, "Number of Columns should be same");
    }
    for(int i=0; i<col1[0].size(); i+=2){
        if(col1[1][i] != col2[1][i]){
            throw Error(-1, "Column Types should Match");
        }
    }

    vector<string> cname, ctype;
    for(int i=0; i<col1[0].size(); i+=2){
        cname.push_back(col1[0][i]);
        ctype.push_back(col1[1][i]);
    }

    set<vector<string>> result_tuples;
    Relation table("");
    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t: tup1){
        result_tuples.insert(t);
    }
    for(auto t: tup2){
        if(result_tuples.count(t))
            result_tuples.erase(t);
    }
    for(auto t: result_tuples){
        table.insert_Tuple(t);
    }
    return table;
}

Relation PRODUCT(string query){
    int pos = comma_pos(query);
    if(pos == -1){
        throw Error(-1, "Invalid Query");
    }
    string p1 = query.substr(0,pos);
    string p2 = query.substr(pos+1);

    Relation inp1 = Query_Parser(p1);
    Relation inp2 = Query_Parser(p2);

    if(inp1.Name == "" || inp2.Name == ""){
        throw Error(-1, "Rename Tables before Cross Product");
    }
    vector<vector<string>> col1 = inp1.get_Columns();
    vector<vector<string>> col2 = inp2.get_Columns();
    vector<vector<string>> tup1 = inp1.get_Tuples();
    vector<vector<string>> tup2 = inp2.get_Tuples();

    vector<string> cname, ctype;
    for(int i=1; i<col1[0].size(); i+=2){
        cname.push_back(col1[0][i]);
        ctype.push_back(col1[1][i]);
    }
    for(int i=1; i<col2[0].size(); i+=2){
        cname.push_back(col2[0][i]);
        ctype.push_back(col2[1][i]);
    }
    
    vector<string> tuple;
    set<vector<string>> result_tuples;
    Relation table("");
    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t1: tup1){
        for(auto t2: tup2){
            tuple.clear();
            for(auto x: t1){
                tuple.push_back(x);
            }
            for(auto x: t2){
                tuple.push_back(x);
            }
            result_tuples.insert(tuple);
        }
    }
    for(auto t: result_tuples){
        table.insert_Tuple(t);
    }
    return table;
}

Relation RENAME(string query){
    int pos = query.find(']');
    if(pos == string::npos){
        throw Error(-1, "Invalid Query");
    }
    string predicate = query.substr(0, pos);
    vector<string> name = RNM_Predicate(predicate);
    if(query[pos+1] != '('){
        throw Error(-1, "Invalid Query");
    }
    Relation input = Query_Parser(query.substr(pos+2));
    vector<vector<string>> columns = input.get_Columns();
    vector<vector<string>> tuples = input.get_Tuples();
    if(name.size() != 1 && name.size()-1 != columns[0].size()/2){
        throw Error(-1, "Invalid Predicate");
    }

    vector<string> cname, ctype;
    Relation table(name[0]);
    if(name.size()>1){
        for(int i=0; i<columns[0].size(); i+=2){
            cname.push_back(name[(i/2) + 1]);
            ctype.push_back(columns[1][i]);
        }
    }
    else{
        for(int i=0; i<columns[0].size(); i+=2){
            cname.push_back(columns[0][i]);
            ctype.push_back(columns[1][i]);
        }
    }

    table.set_Column_Names(cname);
    table.set_Column_Types(ctype);
    for(auto t: tuples){
        table.insert_Tuple(t);
    }
    return table;
}