#include <iostream>
#include "DBManager.h"
using namespace std;

DBManager db = DBManager("NetFlow.db");

int main(){
    db.read();
    return 0;
}