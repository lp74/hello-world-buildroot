#include <linux/can.h>
#include <linux/can/raw.h>

#include "include/msg.hpp"

Msg::Msg(can_frame* p_f)
{
    p_frame = p_f;
};

int Msg::destination()
{
    return ((p_frame->can_id & 0x1C000000) >> 26) | ((p_frame->can_id & 0x00001FFF) << 3);
};
int Msg::command()
{
    return (p_frame->can_id >> 18) & 0xFF;
}
int Msg::sender()
{
    return p_frame->data[0] & 0x0F;
    ;
}
int Msg::protocol()
{
    return (p_frame->can_id >> 13) & 0x0000001F;
};

int Msg::message()
{
    return (p_frame->data[0] & 0xF0) >> 4;
};
