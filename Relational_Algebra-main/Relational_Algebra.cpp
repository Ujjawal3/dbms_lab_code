#include "header.h"

string clean(string query){
    string result = "";
    int ic = 0;
    for(auto ch: query){
        if(ch == '\'')
            ic = ic^1;
        if(ch == ' ' && ic == 0)
            continue;
        result += ch;
    }
    return result;
}

bool isValid(string query){
    stack<char> st;
    for(auto ch: query){
        if(ch == '('){
            st.push(ch);
        }
        if(ch == ')'){
            if(!st.size())
                return false;
            else
                st.pop();
        }
    }
    if(st.size())
        return false;
    return true;
}

int main(){
    string query;
    while(true){
        cout << endl << ">> ";
        getline(cin, query);
        query = clean(query);
        if(query == "exit"){
            cout << endl;
            exit(0);
        }

        try{
            if(!isValid(query))
                throw Error(-1, "Brackets are Invalid");
            Relation result = Query_Parser(query);
            cout << endl;
            result.print();
        }

        catch(Error e){
            e.print();
        }
    }
    
    return 0;
}