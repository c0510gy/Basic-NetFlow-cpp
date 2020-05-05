#include <iostream>
#include <thread>
#include <queue>
#include <set>
#include "UDP-Client.h"
#include "PacketCapture.h"
#include "Types.h"
using namespace std;

#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 1234
#define EXPIRE_TIME 10

queue<Flow> flowQ;
set<FlowRecord> flowCache;
UDPclient udp = UDPclient(COLLECTOR_IP, COLLECTOR_PORT, 1024);
bool handler = true;

void exports();
void flowHandler();
void showCache();

int main(){
    thread flowHandlerThread(flowHandler);

    // Start to capture packets
    pckcap::initPCAP(flowQ);

    handler = false;
    flowHandlerThread.join();

    showCache();

    return 0;
}
void exports(){
    queue<FlowRecord> q;
    for(auto itr = flowCache.begin(); itr != flowCache.end(); ++itr){
        FlowRecord fr = *itr;
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - fr.endTime;

        if(elapsed_seconds.count() > EXPIRE_TIME){
            q.push(fr);
        }
    }
    while(!q.empty()){
        FlowRecord fr = q.front(); q.pop();
        udp.send(fr.toString());
        flowCache.erase(fr);
    }
}
void flowHandler(){
    while(handler){
        if(!flowQ.empty()){
            Flow flow = flowQ.front(); flowQ.pop();
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

        exports();
        
        //showCache();
    }
}
void showCache(){
    cout << "=== NetFlow Cache Table ===" << endl;
    for(auto itr = flowCache.begin(); itr != flowCache.end(); ++itr){
        cout << (*itr).toString() << endl;
    }
}