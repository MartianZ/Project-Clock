/* 
 * File:   nixietube.h
 * Author: MartianZ
 *
 * Created on 2014年9月18日, 下午4:33
 */

#ifndef NIXIETUBE_H
#define	NIXIETUBE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f0xx_it.h"

    void configuration_nixietube(void);
    void configuration_led(void);
    void nixietube_send_595_new_number(uint8_t v1, uint8_t v2);

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIETUBE_H */

