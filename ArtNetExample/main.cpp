#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "artnet/artnet.h"

static artnet_node node;
typedef unsigned char dmx_t;

using namespace std;

void CHECK(void *p)
{
    if (p == NULL)
    {
        fprintf(stderr, "could not alloc\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int verbose = 1;
    //char ip_addr[] = "169.254.86.114";
    char* ip_addr = nullptr;
    
    int optc = 0;
    int port_addr = 0x00;
    int subnet_addr = 0x00;
    int bcast_limit = 0;
    int an_sd;
    int MAXCHANNELS = 512;
    dmx_t *dmx;
    
    /* alloc */
    dmx = (dmx_t*)calloc(MAXCHANNELS + 10, sizeof(dmx_t)); /* 10 bytes security, for file IO routines, will be optimized and checked later */
    CHECK(dmx);
    
    if (argc > 1)
    {
        ip_addr = argv[1];
    }
    else
    {
        printf("Unable to set IP address");
        return 1;
    }
    
    /* set up artnet node */
    node = artnet_new(ip_addr, verbose);
    
    if (node == NULL) {
        printf("Unable to set up artnet node: %s\n", artnet_strerror());
        return 1;
    }
    
    // set names and node type
    artnet_set_short_name(node, "DMX Console ");
    artnet_set_long_name(node, "ArtNet DMX Console for Linux");
    artnet_set_node_type(node, ARTNET_SRV);
    
    artnet_set_subnet_addr(node, subnet_addr);
    
    // enable the first input port (1 universe only)
    artnet_set_port_type(node, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
    artnet_set_port_addr(node, 0, ARTNET_INPUT_PORT, port_addr);
    artnet_set_bcast_limit(node, bcast_limit);
    
    printf("artnet_get_universe_addr: %.2X", artnet_get_universe_addr(node, 0, ARTNET_INPUT_PORT));
    
    //start the node
    artnet_start(node);
    
    // store the sds
    an_sd = artnet_get_sd(node);
    
    // clear everething
    memset(dmx, 0, MAXCHANNELS);
    artnet_send_dmx(node, 0, MAXCHANNELS, dmx);
    
    
    int ID = 0;
    while (ID < 255)
    {
        
        // Send DMX data
        {
            // First Light
            dmx[0] = ID; // Red
            dmx[1] = ID; // Green
            dmx[2] = ID; // Green
            
            // Second Light
            dmx[7] = ID; // Red
            dmx[8] = ID; // Green
            dmx[9] = ID; // Green
        }
        
        artnet_send_dmx(node, 0, MAXCHANNELS, dmx);
        this_thread::sleep_for(std::chrono::milliseconds(10));
        
        cout << ID << " artnet_send_dmx" << endl;
        
        ID++;
    }
    
    memset(dmx, 0, MAXCHANNELS);
    artnet_send_dmx(node, 0, MAXCHANNELS, dmx);
    
    this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}
