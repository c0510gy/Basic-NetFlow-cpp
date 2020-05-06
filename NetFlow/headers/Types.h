
#ifndef NETFLOW_TYPES
#define NETFLOW_TYPES

#include <string>
#include <ctime>
#include <vector>

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
        std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);

        std::chrono::duration<double> elapsed_seconds = endTime - startTime;

        return std::to_string(flow.protocol) + "\t"
                 + buffer + "\t"
                 + std::to_string(elapsed_seconds.count()) + "\t"
                 + flow.srcIP + "\t"
                 + flow.desIP + "\t"
                 + std::to_string(flow.srcPort) + "\t"
                 + std::to_string(flow.desPort) + "\t"
                 + std::to_string(flow.typeOfService) + "\t"
                 + std::to_string(flow.length) + "\t"
                 + std::to_string(packets);
    }
};

struct FlowDBRecord{
    std::string startTime, srcIP, desIP;
    double duration; // seconds
    int protocol;
    int srcPort, desPort;
    int typeOfService;
    int size;
    int packets;
    
    FlowDBRecord(const FlowRecord& fr);
    FlowDBRecord(const std::string& str);
};
FlowDBRecord::FlowDBRecord(const FlowRecord& fr){
    std::time_t start_time = std::chrono::system_clock::to_time_t(fr.startTime);
    std::tm * ptm = std::localtime(&start_time);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);

    std::chrono::duration<double> elapsed_seconds = fr.endTime - fr.startTime;

    this->protocol = fr.flow.protocol;
    this->startTime = buffer;
    this->srcIP = fr.flow.srcIP;
    this->desIP = fr.flow.desIP;
    this->duration = elapsed_seconds.count();
    this->srcPort = fr.flow.srcPort;
    this->desPort = fr.flow.desPort;
    this->typeOfService = fr.flow.typeOfService;
    this->size = fr.flow.length;
    this->packets = fr.packets;
}
FlowDBRecord::FlowDBRecord(const std::string& str){
    std::vector<std::string> splited;
    std::string tmp = "";
    for(int j = 0; j < str.size(); ++j){
        if(str[j] == '\t'){
            splited.push_back(tmp);
            tmp = "";
        }else
            tmp += str[j];
    }
    if(tmp != "")
        splited.push_back(tmp);
    
    this->protocol = std::stoi(splited[0]);
    this->startTime = splited[1];
    this->srcIP = splited[3];
    this->desIP = splited[4];
    this->duration = std::stod(splited[2]);
    this->srcPort = std::stoi(splited[5]);
    this->desPort = std::stoi(splited[6]);
    this->typeOfService = std::stoi(splited[7]);
    this->size = std::stoi(splited[8]);
    this->packets = std::stoi(splited[9]);
}

#endif