#ifndef __BH1750_H
#define __BH1750_H

#include "i2c.h"

void BH1750_Init(void);
void BH1750_Start(uint8_t mode);
uint16_t BH1750_Read(void);

#endif
