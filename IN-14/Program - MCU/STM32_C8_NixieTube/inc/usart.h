/* 
 * File:   usart.h
 * Author: MartianZ
 *
 * Created on 2014年3月31日, 下午11:46
 */

#ifndef USART_H
#define	USART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
    
void USART_SendChar(USART_TypeDef* USARTx, char ch);
void USART_SendStr(USART_TypeDef* USARTx, char *str);
void USART_SendBuf(USART_TypeDef* USARTx, char *buf, unsigned int bufsize);
void USART_SendBufHex(USART_TypeDef* USARTx, char *buf, unsigned int bufsize);
void USART1_Init(void);
void USART_Terminal();

#ifdef	__cplusplus
}
#endif

#endif	/* USART_H */

