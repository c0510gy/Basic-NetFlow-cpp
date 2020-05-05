#include <iostream>
#include <queue>
#include <set>
#include <thread>
#include "UDP-Client.h"
#include "PacketCapture.h"
#include "Types.h"
using namespace std;

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 1234

queue<Flow> q;
set<FlowRecord> flowCache;
bool handler = true;

void flowHandler();
void showCache();

int main(){
    UDPclient udp(COLLECTOR_IP, COLLECTOR_PORT, 1024);

    thread thread_obj(flowHandler);

    initPCAP(q);

    handler = false;

    thread_obj.join();

    showCache();

    return 0;
}
void flowHandler(){
    while(handler){
        if(!q.empty()){
            Flow flow = q.front(); q.pop();
            //cout << flow.protocol << "\t" << flow.srcIP << ":" << flow.srcPort << "\t" << flow.desIP << ":" << flow.desPort << "\t" << flow.length << endl;

            auto itr = flowCache.find(FlowRecord(flow));
            if(itr == flowCache.end()){
                flowCache.insert(FlowRecord(flow));
            }else{
                FlowRecord newRecord = (*itr);
                flowCache.erase(itr);
                newRecord.addFlow(flow);
                flowCache.insert(newRecord);
            }
        }
        
        //showCache();
    }
}
void showCache(){
    cout << "=== NetFlow Cache Table ===" << endl;
    for(auto itr = flowCache.begin(); itr != flowCache.end(); ++itr){
        cout << (*itr).toString() << endl;
    }
}