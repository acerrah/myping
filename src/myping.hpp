#pragma once

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
#include <sstream>
#include "arppacket.hpp"

class myping: public arp_packet{
    public:
        myping(const std::string interface): arp_packet(interface) {}
        void ping(const char* ipAddress);
        void ping(const char* ipAddress, const char* domain);
    private:
        const std::string interface;
};
