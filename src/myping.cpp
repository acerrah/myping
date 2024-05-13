#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <chrono>
#include <ctime>
#include <cstddef>
#include "arppacket.hpp"
#include "myping.hpp"

///////////////////////////////
// Gets ip of the dns
void domain_to_ip(const char* domain, char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(domain)) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return;
    }
}

///////////////////////////////
// Function to calculate checksum
unsigned short calculateChecksum(unsigned short *buffer, int length) {
    unsigned long sum = 0;
    while (length > 1) {
        sum += *buffer++;
        length -= 2;
    }
    if (length == 1) {
        sum += *(unsigned char *)buffer;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

///////////////////////////////
// Ping function
void myping::ping(const char* ipAddress) {
    arp_packet arp = arp_packet("wlan0");
    int sockfd = create_raw_socket();

    const struct sockaddr_ll sll = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = (int)if_nametoindex("wlan0"),
    };

    if (bind(sockfd, reinterpret_cast<const struct sockaddr*>(&sll), sizeof(sll)) < 0) {
        errexit(errno);
    }

    // ethheader
    struct ethhdr ethHeader;
    unsigned char destMac[ETH_ALEN];
    unsigned char srcMac[ETH_ALEN];
    if (ipAddress[0] == '1' && ipAddress[1] == '9' && ipAddress[2] == '2' && ipAddress[3] == '.' && ipAddress[4] == '1' && ipAddress[5] == '6' && ipAddress[6] == '8') {
        memcpy(destMac, arp.get_mac_address(ipAddress), ETH_ALEN);
    } else {
        memcpy(destMac, arp.get_mac_address("192.168.1.1"), ETH_ALEN);
    }
    memcpy(ethHeader.h_dest, destMac, ETH_ALEN);
    memcpy(srcMac, arp.get_local_mac_address(), ETH_ALEN);
    memcpy(ethHeader.h_source, srcMac, ETH_ALEN);
    ethHeader.h_proto = htons(ETH_P_IP);

    // IP header
    struct iphdr ipHeader;
    ipHeader.ihl = 5;
    ipHeader.version = 4;
    ipHeader.tos = 0;
    ipHeader.tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
    ipHeader.id = htons(11436);
    ipHeader.frag_off = 0;
    ipHeader.ttl = 64;
    ipHeader.protocol = IPPROTO_ICMP;
    ipHeader.saddr = inet_addr(arp.get_local_ip().c_str());
    ipHeader.daddr = inet_addr(ipAddress);


    // ICMP header
    struct icmphdr icmpHeader;
    icmpHeader.type = ICMP_ECHO;
    icmpHeader.code = 0;
    icmpHeader.un.echo.id = getpid();
    icmpHeader.un.echo.sequence = 0;
    icmpHeader.checksum = 0;
    icmpHeader.checksum = calculateChecksum((unsigned short *)&icmpHeader, sizeof(icmpHeader));

    // Construct the packet
    char packet[sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct icmphdr)];
    memcpy(packet, &ethHeader, sizeof(struct ethhdr));
    memcpy(packet + sizeof(struct ethhdr), &ipHeader, sizeof(struct iphdr));
    memcpy(packet + sizeof(struct ethhdr) + sizeof(struct iphdr), &icmpHeader, sizeof(struct icmphdr));

    std::cout << "Pinging " << ipAddress << std::endl;
    // Send the ICMP packet
    if (sendto(sockfd, packet, sizeof(packet), 0, reinterpret_cast<const struct sockaddr*>(&sll), sizeof(sll)) < 0) {
        errexit(errno);
    }
    std::cout << "Ping successful." << std::endl;
    close(sockfd);
}

void myping::ping(const char* ipAddress, const char* domain) {
    char *ip;
    domain_to_ip(domain, ip);
    ping(ip);
}