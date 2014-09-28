/* 
 * File:   pid.h
 * Author: MartianZ
 *
 * Created on 2014年9月17日, 下午9:36
 */

#ifndef PID_H
#define	PID_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f0xx_it.h"

    typedef struct PID {
        int16_t SetPoint; // 设定目标Desired value 
        int8_t Proportion; // 比例常数Proportional Const 
        int8_t Integral; // 积分常数Integral Const 
        int8_t Derivative; // 微分常数Derivative Const 
        int16_t LastError; // Error[-1] 
        int16_t PrevError; // Error[-2] 
        int16_t SumError; // Sums of Errors
        uint8_t Counter;
        int8_t Enable;
    } PID;

#define   MAX_ERROR      3
#define   COUNTER_MATCH  90    

    void pid_mem_clear(PID *pp);
    int16_t pid_calc(PID *pp, int16_t NextPoint);
    void configuration_pid(PID *sPID, int8_t Proportion, int8_t Integral, int8_t Derivative, int16_t SetPoint);



#ifdef	__cplusplus
}
#endif

#endif	/* PID_H */

