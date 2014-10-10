/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   
 ******************************************************************************
 **/

#include "flash.h"

#define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#define ProgramAddr        ((u32)0x0800F000)  //Page 60, hmmmm... daijyoubune?
#define ConfigurationAddr        ((u32)0x0800F400)  //Page 61


void Flash_WriteConfiguration(uint32_t configuration) {
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(ConfigurationAddr);
    FLASH_ProgramWord(ConfigurationAddr, configuration);
}


uint32_t Flash_ReadConfiguration() {
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    return (*(__IO uint32_t*) ConfigurationAddr);
}


void Flash_WriteData(LED5050 LED) {
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(ProgramAddr);
    FLASH_ProgramWord(ProgramAddr, LED.R << 16 | LED.G << 8 | LED.B);
}

void Flash_ReadData(LED5050 *LED) {
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    uint32_t LED_Data = (*(__IO uint32_t*) ProgramAddr);
    LED->R = LED_Data >> 16 & 0xFF;
    LED->G = LED_Data >> 8 & 0xFF;
    LED->B = LED_Data & 0xFF;
}

