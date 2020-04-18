#include "nrf52_nvmc.h"
#include <startup/nrf.h>

static inline void nrf52_wait_for_flash_ready(void)
{
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {;}
}

void nrf52_nmvc_erase_page(uint32_t address) {
  // Enable erase.
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
  __ISB();
  __DSB();

  // Erase the page
  NRF_NVMC->ERASEPAGE = address;
  nrf52_wait_for_flash_ready();

  // Disable erase
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
  __ISB();
  __DSB();
}

void nrf52_nvmc_write_word(uint32_t address, uint32_t value) {
  // Enable write.
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
  __ISB();
  __DSB();

  // Write word
  *(uint32_t*)address = value;
  nrf52_wait_for_flash_ready();

  // Disable write
  NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
  __ISB();
  __DSB();
}

