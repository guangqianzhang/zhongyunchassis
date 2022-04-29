/*
 * canif.h
 *
 *  Created on: 2021Äê3ÔÂ30ÈÕ
 *      Author: gongping.xie
 */

#ifndef SRC_CANIF_H_
#define SRC_CANIF_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <errno.h>
#include <poll.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>



typedef void (*canin)(const struct can_frame *can ,void *ctx);
typedef void (*can_do_packet)(struct can_frame *can ,int *size , void *ctx);
typedef struct
{
	can_do_packet cb_pack;
	void *pack_ctx;
	canin cb_canin ;
	void *can_in_ctx;
	int flag_in;
	int flag_out;
	int socket_tx;
	pthread_t task_send;
	pthread_t task_recv;
	struct can_frame frame_data[6];
	struct can_frame *pframe_data;
	int send_size;

}canif_t;
int canif_init();
void canif_deinit();
int canif_start();
void canif_stop();

void register_can_in_handler(canin handler ,void *ctx);
void register_get_canframe(can_do_packet pack , void *ctx);

#endif /* SRC_CANIF_H_ */
