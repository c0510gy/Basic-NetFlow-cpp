/* original - https://pyukcho.tistory.com/entry/100050251838 */

// Read packet as a filtering rule and print it's information

#include <cstdio>
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

// filtering rule
#define FILTER_RULE "host 168.131.161.1"
#define PCAP_SNAPSHOT 1024
#define PCAP_TIMEOUT 100

void packet_view(unsigned char *, const struct pcap_pkthdr *, const unsigned char *);

int main(int argc, char *argv[]){
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 net;
    bpf_u_int32 netmask;
    struct in_addr net_addr, mask_addr;
    pcap_t *pd;
    struct bpf_program fcode;
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
    if(pcap_compile(pd, &fcode, FILTER_RULE, 0, netmask) < 0){
        perror(pcap_geterr(pd));
        exit(1);
    }
    // set filtering rule
    if(pcap_setfilter(pd, &fcode) < 0){
        perror(pcap_geterr(pd));
        exit(1);
    }
    // read packet forever
    if(pcap_loop(pd, -1, packet_view, 0) < 0){
        perror(pcap_geterr(pd));
        exit(1);
    }
    pcap_close(pd);
    return 1;
}

// packet_view: print packets
void packet_view(unsigned char *user, const struct pcap_pkthdr *h, const unsigned char *p){
    struct ip *iph;
    struct ether_header *etherh;
    unsigned short e_type;
    // p가 ip header를 가리키도록 ethernet header의 크기만큼 증가 시킴
    etherh = (struct ether_header *)p;
    e_type = ntohs(etherh->ether_type);
    if( e_type == ETHERTYPE_IP ){
        printf("ip \n");
        p += sizeof(struct ether_header);
        iph = (struct ip *) p;
        // ip header의 정보를 출력
        printf("SRC = %s\n", inet_ntoa(iph->ip_src));
        printf("DST = %s\n", inet_ntoa(iph->ip_dst));
        printf("IP Version = %d\n", iph->ip_v);
    }else if( e_type == ETHERTYPE_ARP ){
        printf("arp \n");
    }else if( e_type == ETHERTYPE_REVARP ){
        printf("rarp \n");
    }else{
        printf("no such type -> type : %d\n", e_type);
    }
}