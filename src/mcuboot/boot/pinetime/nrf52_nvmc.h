#ifndef PINETIME_BOOTLOADER_NRF52_NVMC_H
#define PINETIME_BOOTLOADER_NRF52_NVMC_H

#include <stdint-gcc.h>

void nrf52_nmvc_erase_page(uint32_t address);
void nrf52_nvmc_write_word(uint32_t address, uint32_t value);


#endif //PINETIME_BOOTLOADER_NRF52_NVMC_H
