#ifndef _MSG_HPP
#define _MSG_HPP

#include <linux/can.h>
#include <linux/can/raw.h>

class Msg
{
  public:
    Msg(can_frame* p_f);
    int destination();
    int command();
    int sender();
    int protocol();
    int message();
  private:
    can_frame* p_frame;
};

#endif
