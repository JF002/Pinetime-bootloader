#include <startup/nrf.h>
#include <stdio.h>
#include <segger_rtt/SEGGER_RTT.h>
#include <mcuboot_config/mcuboot_logging.h>
#include <bootutil/bootutil.h>
#include <assert.h>
#include <include/flash_map_backend/flash_map_backend.h>
#include <stdio.h>
#include "bootutil/image.h"
#include "bootutil/bootutil.h"
#include <arm_cleanup.h>

void InitClock();

static void jump_to_app(uint32_t startAddr) {
  // Adjust the current ISR vector address.
  *(uint32_t*)0x20000000 = startAddr;

  // Jump to the ISR vector of the SoftDevice.
  uint32_t *sd_isr = (uint32_t*)startAddr;
  uint32_t new_sp = sd_isr[0]; // load end of stack (_estack)
  uint32_t new_pc = sd_isr[1]; // load Reset_Handler
  __asm__ __volatile__(
  "mov sp, %[new_sp]\n" // set stack pointer to initial stack pointer
  "mov pc, %[new_pc]\n" // jump to SoftDevice Reset_Vector
  :
  : [new_sp]"r" (new_sp),
  [new_pc]"r" (new_pc));
  __builtin_unreachable();
}

static void do_boot(struct boot_rsp *rsp) {
  uintptr_t flash_base;
  void *start;
  int rc;

  rc = flash_device_base(rsp->br_flash_dev_id, &flash_base);
  assert(rc == 0);

  start = (void *)(flash_base + rsp->br_image_off +
                   rsp->br_hdr->ih_hdr_size);

  jump_to_app(start);
}

int main() {
  //SEGGER_RTT_Init();
  InitClock();

#if 1

  struct boot_rsp rsp;
  int rc;

  os_heap_init();

  rc = boot_go(&rsp);
  if (rc != 0) {
    MCUBOOT_LOG_ERR("Unable to find bootable image");

    while (1)
      ;
  }

  MCUBOOT_LOG_ERR("Bootloader chainload address offset: 0x%x", rsp.br_image_off);

  MCUBOOT_LOG_ERR("Jumping to the first image slot");
//  ZEPHYR_BOOT_LOG_STOP();

  for(int i = 0; i < 65536; i++) {
    asm("nop");
  }

  do_boot(&rsp);

  MCUBOOT_LOG_ERR("Never should get here");
  while (1)
    ;
#endif
  return 0;
}

void InitClock() {
  NRF_CLOCK->LFCLKSRC = 1;
}
