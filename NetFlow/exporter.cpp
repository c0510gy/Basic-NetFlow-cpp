#include <iostream>
#include <thread>
#include <queue>
#include <set>
#include "headers/UDP-Client.h"
#include "headers/PacketCapture.h"
#include "headers/Types.h"
using namespace std;

// Collector 정보
#define COLLECTOR_IP "127.0.0.1"
#define COLLECTOR_PORT 1234
// flow 종료 판단 시간
#define EXPIRE_TIME 10
// flow 최대 기록 시간
#define EXPIRE_MAX_TIME 6000

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
        std::chrono::duration<double> elapsed_end_seconds = now - fr.endTime;
        std::chrono::duration<double> elapsed_start_seconds = now - fr.startTime;

        if(elapsed_end_seconds.count() > EXPIRE_TIME || elapsed_start_seconds.count() > EXPIRE_MAX_TIME){
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
    }
}
void showCache(){
    cout << "=== NetFlow Cache Table ===" << endl;
    for(auto itr = flowCache.begin(); itr != flowCache.end(); ++itr){
        cout << (*itr).toString() << endl;
    }
}