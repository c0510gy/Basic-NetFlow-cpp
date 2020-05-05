#include <iostream>
#include <string>
#include "UDP-Server.h"
using namespace std;

UDPserver udp = UDPserver(1234, 1024);

int main(){
    while(true){
        string msg = udp.recv();
        cout << msg << endl;
    }
    return 0;
}