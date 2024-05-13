#include <myping.hpp>

int main(){
    myping ping("wlan0");
    ping.ping("192.168.68.1");
}