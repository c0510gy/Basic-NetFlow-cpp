#ifndef PACKET_CAPTURE
#define PACKET_CAPTURE

#include <cstdlib>
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include <chrono>
#include <ctime>
#include <queue>
#include "Types.h"

#define PCAP_CNT_MAX 1000
#define PCAP_SNAPSHOT 1024
#define PCAP_TIMEOUT 100

namespace pckcap{
    std::queue<Flow> *flowQ;

    void packet_view(unsigned char *user, const struct pcap_pkthdr *h, const unsigned char *p);
    void initPCAP(std::queue<Flow>& flowQ);
};

void pckcap::initPCAP(std::queue<Flow>& flowQ){
    pckcap::flowQ = &flowQ;

    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 net;
    bpf_u_int32 netmask;
    struct in_addr net_addr, mask_addr;
    pcap_t *pd;
    if(!(dev = pcap_lookupdev(errbuf))){
        perror(errbuf);
        exit(1);
    }
    if(pcap_lookupnet(dev, &net, &netmask, errbuf) < 0){
        perror(errbuf);
        exit(1);
    }
    net_addr.s_addr = net;
    mask_addr.s_addr = netmask;
    printf("Device : %s\n", dev);
    printf("Net Address : %s\n", inet_ntoa(net_addr));
    printf("Netmask : %s\n", inet_ntoa(mask_addr));

    if((pd = pcap_open_live(dev, PCAP_SNAPSHOT, 1, PCAP_TIMEOUT, errbuf)) == NULL){
        perror(errbuf);
        exit(1);
    }
    if(pcap_loop(pd, PCAP_CNT_MAX, packet_view, 0) < 0){
        perror(pcap_geterr(pd));
    }

    pcap_close(pd);
}

void pckcap::packet_view(unsigned char *user, const struct pcap_pkthdr *pkthdr, const unsigned char *packet){
    Flow flow;
    flow.timeStamp = std::chrono::system_clock::now();
    
    struct ip *iph; // IP header 구조체
    struct tcphdr *tcph; // TCP header 구조체
    struct ether_header *etherh;
    unsigned short e_type;

    int length = pkthdr->len;
    flow.length = length;

    // p가 ip header를 가리키도록 ethernet header의 크기만큼 증가 시킴
    etherh = (struct ether_header *)packet;
    e_type = ntohs(etherh->ether_type);
    if( e_type == ETHERTYPE_IP ){
        packet += sizeof(struct ether_header);
        iph = (struct ip *) packet;

        flow.srcIP = inet_ntoa(iph->ip_src);
        flow.desIP = inet_ntoa(iph->ip_dst);
        flow.protocol = iph->ip_p;
        /*
            IPPROTO_TCP = 6
            IPPROTO_UDP = 17
            IPPROTO_ICMP = 1
            IPPROTO_IP = 0
        */
        flow.typeOfService = iph->ip_tos;

        // TCP
        if(iph->ip_p == IPPROTO_TCP){
            tcph = (struct tcphdr *)(packet + iph->ip_hl * 4);
            flow.srcPort = ntohs(tcph->th_sport);
            flow.desPort = ntohs(tcph->th_dport);
        }

        flowQ->push(flow);
    }else if(e_type == ETHERTYPE_ARP){
        //printf("arp \n");
    }else if(e_type == ETHERTYPE_REVARP){
        //printf("rarp \n");
    }else{
        //printf("no such type -> type : %d\n", e_type);
    }
}

#endif