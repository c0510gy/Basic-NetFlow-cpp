#include <iostream>
#include <queue>
#include <set>
#include <thread>
#include "UDP-Client.h"
#include "PacketCapture.h"
#include "Types.h"
using namespace std;

queue<Flow> q;
set<FlowRecord> flowCache;
bool handler = true;

void flowHandler();
void showCache();

void callback(Flow flow){ // PacketCapture
    q.push(flow);
}

int main(){
    UDPclient udp("127.0.0.1", 1234, 1024);

    thread thread_obj(flowHandler);

    initPCAP();

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