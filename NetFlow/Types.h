
#ifndef NETFLOW_TYPES
#define NETFLOW_TYPES

#include <string>

struct Flow{
    std::string srcIP, desIP;
    int protocol;
    int srcPort, desPort;
    int typeOfService;
    int length; // Bytes

    Flow(){
        srcIP = ""; desIP = "";
        protocol = srcPort = desPort = typeOfService = length = -1;
    }
};

#endif