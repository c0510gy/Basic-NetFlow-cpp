#include <iostream>
#include <string>
#include <sqlite3.h>
#include "headers/UDP-Server.h"
#include "headers/DBManager.h"
#include "headers/Types.h"
using namespace std;

UDPserver udp = UDPserver(1234, 1024);
DBManager db = DBManager("NetFlow.db");

int main(){
    while(true){
        string msg = udp.recv();
        cout << msg << endl;
        FlowDBRecord fdbr(msg);
        db.insert(fdbr);
    }
    return 0;
}