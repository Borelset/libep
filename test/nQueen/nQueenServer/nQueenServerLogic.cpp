//
// Created by borelset on 2/1/18.
//

#include <string.h>
#include <test/nQueen/nQueenServer/nQueenServerLogic.h>

bool verify(vector<int> status){
    for (int i = 0; i < status.size(); i++){
        for (int j = i + 1; j < status.size(); j++){
            if (status[i] == status[j] ||
                abs(status[i] - status[j]) == abs(i - j)){
                return false;
            }
        }
    }
    return true;
}

int nQueenSolver(vector<int> status, int q, int left){
    if (!verify(status)){
        return 0;
    }
    if (left == 0){
        return 1;
    }
    int result = 0;
    for (int i = 0; i < q; i++){
        vector<int> temp = status;
        temp.push_back(i);
        result += nQueenSolver(temp, q, left - 1);
    }
    return result;
}

nQueenServerLogic::ServerState nQueenServerLogic::stateParser(string &str) {
    const char* strPtr = str.c_str();
    if(!strncmp(strPtr,
                ServerStateString[(int)ServerState::Confirm],
                strlen(ServerStateString[(int)ServerState::Confirm]))){
        str = str.c_str() + strlen(ServerStateString[(int)ServerState::Confirm]);
        return ServerState::Confirm;

    }else if(!strncmp(strPtr,
                      ServerStateString[(int)ServerState::Task],
                      strlen(ServerStateString[(int)ServerState::Task]))){
        str = str.c_str() + strlen(ServerStateString[(int)ServerState::Task]);
        return ServerState::Task;

    }else{
        return ServerState::Unknown;
    }
}

void nQueenServerLogic::parseTask(string str, int &total, int &index) {
    int temp = 0;
    for(int i=0; i<str.size(); i++){
        if(str.c_str()[i] <= '9' && str.c_str()[i] >= '0'){
            temp *= 10;
            temp += str.c_str()[i] - '0';
        }else if(str.c_str()[i] == '@'){
            index = temp;
            temp = 0;
        }
    }
    total = temp;
}

string nQueenServerLogic::confirmCallback(string &content, const string &connName) {
    cout << "Confirm Ready" <<endl;
    string result = "Ready";

    return result;
}

string nQueenServerLogic::taskCallback(string &content, const string &connName) {
    int n = -1, t = -1;
    parseTask(content, t, n);

    vector<int> status = {n};
    int res = nQueenSolver(status, t, t-1);

    char buffer[1024];
    sprintf(buffer, "Result %d@%d=%d", n, t, res);
    string result = buffer;

    return result;
}

string nQueenServerLogic::errorCallback(string &content, const string &connName) {

}

