#include <iostream>
#include <queue>
#include <thread>
#include "UDP-Client.h"
#include "PacketCapture.h"
#include "Types.h"
using namespace std;

queue<Flow> q;

void callback(Flow flow){
    q.push(flow);
}
void flowHandler(){
    while(true){
        if(q.empty())
            continue;
        Flow flow = q.front(); q.pop();
        cout << flow.protocol << " " << flow.srcIP << ":" << flow.srcPort << " " << flow.desIP << ":" << flow.desPort << " " << flow.length << endl;
    }
}
int main(){
    thread thread_obj(flowHandler);

    initPCAP();

    return 0;
}
