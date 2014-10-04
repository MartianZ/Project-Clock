/* 
 * File:   flash.h
 * Author: MartianZ
 *
 * Created on 2014年7月19日, 下午12:17
 */

#ifndef FLASH_H
#define	FLASH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f10x_flash.h"
#include "nixietube.h"

    void Flash_WriteData(LED5050 LED);
    void Flash_ReadData(LED5050 *LED);
    void Flash_WriteConfiguration(uint32_t configuration);
    uint32_t Flash_ReadConfiguration();

#ifdef	__cplusplus
}
#endif

#endif	/* FLASH_H */

