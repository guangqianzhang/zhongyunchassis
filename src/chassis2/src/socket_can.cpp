#include <iostream>
#include "chassis2/socket_can.h"

SocketCAN::SocketCAN(const char *CANPort, const int bitrate)
{
    set_port(CANPort);
    set_bitrate(bitrate);
    
}

SocketCAN::~SocketCAN()
{
    if(-1 != skt)
    CANClose(skt);
}

void SocketCAN::set_bitrate(const int bitrate)
{
    static int defaultCANbitrate = 500000;

    if (bitrate != 0)
    {
        _CAN_bitrate = bitrate;
    }
    else
        _CAN_bitrate = defaultCANbitrate;

    printf("CANbitrate:%d\n", _CAN_bitrate);
}
void SocketCAN::set_port(const char *CANPort)
{
    static char defaultCANNPort[] = "can0";
    if (CANPort != NULL)
    {
        _CANPort = CANPort;
    }
    else
        _CANPort = defaultCANNPort;
    printf("CANport:%s\n", _CANPort);
}
int filter(int skt, int num, int *mask)
{
    struct can_filter rfilter_r[num - 1];
    for (; 0 < num; num--)
    {
        rfilter_r[num].can_id = mask[num];
        rfilter_r[num].can_mask = CAN_EFF_MASK;
    }
    (void)setsockopt(skt, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter_r, sizeof(rfilter_r));
return 0;
}
int SocketCAN::can_socket(__u8 num, unsigned int *mask)
{
    struct sockaddr_can addr;
    struct ifreq ifr;

    skt = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (skt < 0)
    {
        cerr << "Failed open CAN socket!" << _CANPort << endl;
        return (-1);
    }
    // strncpy(ifr.ifr_name,"can0",(sizeof(ifr.ifr_name)-1));
    strcpy(ifr.ifr_name, _CANPort);

    if (ioctl(skt, SIOCGIFINDEX, &ifr) < 0)
    { //指定设备
        cerr << "Failed CAN ioctl:" << ifr.ifr_name << endl;
        return (-1);
    }
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cerr << "Failed CAN bind" << endl;
        return (-1);
    }
    //过滤设置
    if (NULL != mask && 0 != num)
    {
        // filter(skt,num,mask);
        struct can_filter rfilter_r[num - 1];
        for (; 0 < num; num--)
        {
            rfilter_r[num].can_id = mask[num];
            rfilter_r[num].can_mask = CAN_SFF_MASK;
        }
        printf(" can set filter \n");
        (void)setsockopt(skt, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter_r, sizeof(rfilter_r));
    }

    //----- Set socket for non-blocking 非阻塞
    // flags = fcntl(skt, F_GETFL, 0);
    // if (flags < 0)
    // {
    //     cerr << "Failed CAN flag fetch" << endl;
    //     return (-1);
    // }

    // if (fcntl(skt, F_SETFL, flags | O_NONBLOCK) < 0)
    // {
    //     cerr << "Failed CAN flag set" << endl;
    //     return (-1);
    // }

    return skt;
}
int SocketCAN::can_set()
{
    // set_port(CANPort);
    sprintf(can_Command, "sudo ip link set %s down", _CANPort);
    system(can_Command);

    // set_bitrate(bitrate);
    memset(can_Command, 0, sizeof(can_Command));
    sprintf(can_Command, "sudo ip link set %s type can bitrate %d restart-ms 100", _CANPort, _CAN_bitrate);
    system(can_Command);

    memset(can_Command, 0, sizeof(can_Command));
    sprintf(can_Command, "sudo ifconfig %s up", _CANPort);
    system(can_Command);
    return 0;
}
bool SocketCAN::CANSendFrame( __u32 id, const __u8 *buf, __u8 len, bool wait_send)
{
    struct can_frame frame_wr;
    // static int i = 0;
    bzero(&frame_wr, sizeof(frame_wr));
    if (len > 8)
    {
        printf("canSnd buf length is wrong.\n");
        return -1;
    }
    frame_wr.can_id = id | CAN_EFF_FLAG;
    frame_wr.can_dlc = len;
    if (NULL != buf)
    {
        memcpy(frame_wr.data, buf, 8);
        frame_wr.can_dlc = len;
    }
    else
        frame_wr.can_dlc = 0x0;

    int ret = write(skt, &frame_wr, sizeof(frame_wr)) == sizeof(frame_wr);
    if (ret == -1)
    {
        perror("write data:");
    }
    printf("send can id = \\\
    %x can[%x %x %x %x %x %x %x %x ]\n" ,
    frame_wr.can_id,frame_wr.data[0], frame_wr.data[1],
    frame_wr.data[2],frame_wr.data[3],frame_wr.data[4],
    frame_wr.data[5],frame_wr.data[6],frame_wr.data[7]);
if(wait_send)usleep(2000);
    return (ret); // Send this frame out to the socketCAN handler

    // socketCAN works to keeping all packets in-order, so
    // no need to do anything special for wait-sent
}

bool SocketCAN::CANGetFrame(__u32 &id, __u8 *buf, __u8 &len)
{
    struct can_frame frame_rd;
    struct timeval tv = {0, 0}; // Non-blocking timout when checking
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(skt, &fds);

    if (select((skt + 1), &fds, NULL, NULL, &tv) < 0) // Is there a FD with something to read out there?
        return false;

    if (FD_ISSET(skt, &fds))
    { // Was it our CAN file-descriptor that is ready?
        if (read(skt, &frame_rd, sizeof(frame_rd)) > 0)
        {
            memcpy(buf, frame_rd.data, 8);
            len = frame_rd.can_dlc;
            id = frame_rd.can_id;
            return true;
        }
    }
    return false;
}
bool SocketCAN::CANClose(int skt)
{
    close(skt);
    return true;
}
