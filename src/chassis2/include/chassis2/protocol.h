#ifndef PROTOCOL_H
#define PROTOCOL_H

#include"zhongyun_def.h"
#include"control.h"


void canin_s(const struct can_frame *frame, void *ctx);
void can_do_packet_S(struct can_frame *frame, int *size, void *ctx);
void zhongyun_carset(struct can_frame *frame,cmd_str *Tocar_smd_str);
#endif