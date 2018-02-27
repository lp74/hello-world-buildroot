#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "include/msg.hpp"


#include <map>
#include <bitset>

#include <fstream>

using namespace std;

typedef enum {
    HL_Float = 0,     // Float (single precision) value
    HL_UInt32 = 10,   // UInt (32 bit) value
    HL_UInt16 = 11,   // UInt (16 bit) value
    HL_UInt8 = 12,    // UInt (8 bit) value
    HL_DateTime = 20, // Seconds passed from 2000/00/00 - 00:00:00
    HL_IP_Addr = 30,  // IP address
    HL_Int32 = 100,   // Int (32 bit) value
    HL_Int16 = 101,   // Int (16 bit) value
    HL_Int8 = 102,    // Int (8 bit) value
    HL_String = 110,  // ASCII string (4 ch) value
    HL_Bool = 120,    // Bool value
} HL_DataType;

int main(int argc, char ** argv){

    ofstream myfile;
    myfile.open ("/home/db/example.txt");
    myfile << "Writing this to a file.\n";
    myfile.close();

    int senderF = atoi(argv[1]);
    int commandF = atoi(argv[2]);
    int indexF = atoi(argv[3]);

    std::map<int, string> cmd;
    cmd[0x00] = "Read State";
    cmd[0x01] = "Read Data";
    cmd[0x02] = "Read/Set Parameter";
    cmd[0x03] = "Read ID Data";
    cmd[0x04] = "Set ID Data";
    cmd[0x05] = "Presence";
    cmd[0x06] = "Reset";
    cmd[0x07] = "Aurora Protocol Message";
    cmd[0x08] = "Read inputs or Set single output";
    cmd[0x0A] = "Ping";
    cmd[0x0F] = "RS485 request";

    std::map<int, string> type;
    type[0x00] = "Message is a request for one or more devices";
    type[0x01] = "Message is an answer from one device (command executed)";
    type[0x02] = "Message is an answer from one device (parameter does not exist)";
    type[0x03] = "Message is an answer from one device (parameter value out of range)";
    type[0x04] = "Message is an answer from one device (command failed)";
    type[0x05] = "Message is an answer from one device (command cannot be executed)";
    type[0x06] = "Message is an answer from one device (command cannot be executed at this moment)";
    type[0x0E] = "Message is a shared request for one or more devices (no answer is needed)";
    type[0x0F] = "Message is a shared data frame for one or more devices";

    cout << "Hello World!" << endl;

    int s;
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    const char *ifname = "can0";

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error in socket bind");
        return -2;
    }

    int counter = 0;

    while (1)
    {
        struct can_frame frame;
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0)
        {
            perror("can raw socket read");
            return 1;
        }

        if (nbytes < sizeof(struct can_frame))
        {
            fprintf(stderr, "read: incomplete CAN frame\n");
            return 1;
        }

        if (nbytes > 0)
        {
            Msg msg = Msg(&frame);

            int destination_id = msg.destination();
            int command_id = msg.command();
            int protocol = msg.protocol();
            int sender = msg.sender();
            int msg_type = msg.message();

            if (
                sender = senderF
                && command_id == commandF
                && frame.data[1] == indexF  //Index //8, 9, 10, Pin1, Pin2, Ptot
                && frame.data[2] == 0       //Type-Group
                && frame.data[3] == 0       //Data Field
                ) 
            {
                counter++;

                int index = frame.data[1];
                int type = frame.data[2];
                int dataField = frame.data[3];

                unsigned char b[] = {frame.data[7], frame.data[6], frame.data[5], frame.data[4]};
                float value;
                memcpy(&value, &b, 4);
                printf("counter:%6d destination: %5d sender: %2d command: %2d type: %2d index: %3d type: %3d data filed: %3d value: %9.3f\n", 
                counter, 
                msg.destination(), 
                msg.sender(),
                msg.command(),
                msg.message(),
                index,
                type,
                dataField,
                value);
            }
        }
    }
    close(s);
    
    return 0;
}