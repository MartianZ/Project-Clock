#include "pid.h"
#include <string.h>
#include <math.h>
#define  MAX_SUM_ERROR   1000
#define  MIN_SUM_ERROR  -1000

int16_t pid_calc(PID *pp, int16_t NextPoint) {
    int16_t dError, Error;
    Error = pp->SetPoint - NextPoint; // 偏差
    if (fabs(Error) < MAX_ERROR) {
        if (pp->Counter < COUNTER_MATCH) (pp->Counter)++;
    } else {
        pp->Counter = 0;
    }
    pp->SumError += Error; // 积分 	
    /*
    if(pp->SumError > MAX_SUM_ERROR)
    {
        pp->SumError = MAX_SUM_ERROR;
    }
    else if(pp->SumError < MIN_SUM_ERROR) 
    {
        pp->SumError = MIN_SUM_ERROR;
    }
     */
    dError = pp->LastError - pp->PrevError; //当前微分
    pp->PrevError = pp->LastError;
    pp->LastError = Error;
    return (pp->Proportion * Error // 比例项 
            + pp->Integral * pp->SumError // 积分项 
            + pp->Derivative * dError // 微分项 
            );
}

void pid_mem_clear(PID *pp) {
    memset(pp, 0, sizeof (PID));
}

void configuration_pid(PID *sPID, int8_t Proportion,int8_t Integral, int8_t Derivative, int16_t SetPoint) {
    pid_mem_clear(sPID); // Initialize Structure 
    sPID->Proportion = Proportion; // Set PID Coefficients 
    sPID->Integral = Integral; 
    sPID->Derivative = Derivative; //0.0; 
    sPID->SetPoint = SetPoint; // Set PID Setpoint 
    sPID->Enable = 1; //Enable PID
}