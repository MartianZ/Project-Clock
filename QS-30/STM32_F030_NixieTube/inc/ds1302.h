/* 
 * File:   ds1302.h
 * Author: MartianZ
 *
 * Created on 2014年9月19日, 上午10:33
 */

#ifndef DS1302_H
#define	DS1302_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "stm32f0xx_it.h"


void ds1302_writetime(uint8_t *buf);
void ds1302_readtime(uint8_t *buf);
void configuration_ds1302(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DS1302_H */

