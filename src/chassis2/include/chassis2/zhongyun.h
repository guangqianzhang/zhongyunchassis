/*
 * zhongyun.h
 *
 *  Created on: 2021Äê3ÔÂ30ÈÕ
 *      Author: gongping.xie
 */

#ifndef SRC_ZHONGYUN_H_
#define SRC_ZHONGYUN_H_
#include <stdlib.h>
#include "zhongyun_def.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef void (*err_handler)(const int *error_code ,void *ctx);

void     zhongyun_can_register_err_handler(err_handler handle,void *ctx);











#ifdef __cplusplus
}
#endif

#endif /* SRC_ZHONGYUN_H_ */
