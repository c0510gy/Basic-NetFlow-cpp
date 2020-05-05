
#ifndef NETFLOW_TYPES
#define NETFLOW_TYPES

#include <string>
#include <ctime>

struct Flow{
    std::string srcIP, desIP;
    int protocol;
    int srcPort, desPort;
    int typeOfService;

    int length; // Bytes
    std::chrono::time_point<std::chrono::system_clock> timeStamp;

    bool operator<(const Flow& flow) const{
        if(srcIP == flow.srcIP){
            if(desIP == flow.desIP){
                if(protocol == flow.protocol){
                    if(srcPort == flow.srcPort){
                        if(desPort == flow.desPort){
                            return typeOfService < flow.typeOfService;
                        }else
                            return desPort < flow.desPort;
                    }else
                        return srcPort < flow.srcPort;
                }else
                    return protocol < flow.protocol;
            }else
                return desIP < flow.desIP;
        }else
            return srcIP < flow.srcIP;
    }
    bool operator==(const Flow& flow) const{
        return srcIP == flow.srcIP && desIP == flow.desIP
         && protocol == flow.protocol && srcPort == flow.srcPort
         && desPort == flow.desPort && typeOfService == flow.typeOfService;
    }
    bool operator>(const Flow& flow) const{
        return !((*this) < flow || (*this) == flow);
    }

    Flow(){
        srcIP = ""; desIP = "";
        protocol = srcPort = desPort = typeOfService = length = -1;
    }
};

struct FlowRecord{
    Flow flow;
    int packets;
    std::chrono::time_point<std::chrono::system_clock> startTime, endTime;

    bool operator<(const FlowRecord& fr) const{
        return flow < fr.flow;
    }
    bool operator==(const FlowRecord& fr) const{
        return flow == fr.flow;
    }
    bool operator>(const FlowRecord& fr) const{
        return flow > fr.flow;
    }
    FlowRecord(Flow flow){
        this->flow = Flow(flow);
        startTime = flow.timeStamp;
        endTime = flow.timeStamp;
        packets = 1;
    }
    void addFlow(Flow flow){
        this->flow.length += flow.length;
        endTime = flow.timeStamp;
        ++packets;
    }
    std::string toString() const{
        std::time_t start_time = std::chrono::system_clock::to_time_t(startTime);
        std::tm * ptm = std::localtime(&start_time);
        char buffer[32];
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

        std::chrono::duration<double> elapsed_seconds = endTime - startTime;

        return std::to_string(flow.protocol) + "\t" + buffer + "\t" + std::to_string(elapsed_seconds.count()) + "\t" + flow.srcIP + ":" + std::to_string(flow.srcPort) + "\t" + flow.desIP + ":" + std::to_string(flow.desPort) + "\t" + std::to_string(flow.length) + "\t" + std::to_string(packets);
    }
};

#endif