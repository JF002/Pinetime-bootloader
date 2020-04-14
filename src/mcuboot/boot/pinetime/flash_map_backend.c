#include <stdint-gcc.h>
#include <bootutil/bootutil_log.h>
#include <memory.h>
#include "flash_map_backend/flash_map_backend.h"

struct flash_area flash_areas[4] = {
        {1, 0, 0, 0x60000, 0x3000},
        {2, 1, 0, 0x70000, 0x3000},
        {3, 0, 0, 0x50000, 0x3000}
};

struct flash_sector flash_sectors_1[1] = {
        {0, 0x3000}
};

struct flash_sector flash_sectors_2[1] = {
        {0, 0x3000}
};

struct flash_sector flash_sectors_3[1] = {
        {0, 0x3000}
};

int flash_area_open(uint8_t id, const struct flash_area **fa) {
  MCUBOOT_LOG_ERR("** flash_area_open %d", id);
  switch(id) {
    case 1: {
      *fa = &flash_areas[0];
      return 0;
    }
    case 2: {
      *fa = &flash_areas[1];
      return 0;
    }
    case 3: {
      *fa = &flash_areas[2];
      return 0;
    }
  }
  return -1;
}

void flash_area_close(const struct flash_area *fa) {
  MCUBOOT_LOG_ERR("** flash_area_close");
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
  MCUBOOT_LOG_ERR("** flash_area_read. FA = %d, offset = %d, len = %d", fa->fa_id, off, len);
  memcpy(dst, fa->fa_off + off, len);
  return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len) {
  MCUBOOT_LOG_ERR("** flash_area_write");
  return -1;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
  MCUBOOT_LOG_ERR("** flash_area_erase");
  return -1;
}

uint8_t flash_area_align(const struct flash_area *fa) {
  MCUBOOT_LOG_ERR("** flash_area_align. FA = %d", fa->fa_id);
  return 1;
}

uint8_t flash_area_erased_val(const struct flash_area *fa) {
  MCUBOOT_LOG_ERR("** flash_area_erased_val");
  return -1;
}

int flash_area_read_is_empty(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
  MCUBOOT_LOG_ERR("** flash_area_read_is_empty. FA = %d, offset = %d, length = %d", fa->fa_id, off, len);
  if(fa->fa_id == 1)
    return 0;
  return 1;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors) {
  MCUBOOT_LOG_ERR("** flash_area_get_sectors. FA = %d", fa_id);
  switch(fa_id) {
    case 1: { // PRIMARY
      *count = 1;
      sectors = flash_sectors_1;
      return 0;
    }
    case 2: { // SECONDARY
      *count = 1;
      sectors = flash_sectors_2;
      return 0;
    }

    case 3: { //  SCRATCH
      *count = 1;
      sectors = flash_sectors_3;
      return 0;
    }
  }

  return -1;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot) {
  MCUBOOT_LOG_ERR("** flash_area_id_from_multi_image_slot. Image = %d, slot = %d", image_index, slot);
  switch(slot) {
    case 0: return 1;
    case 1: return 2;
  }

  return -1;
}

int flash_area_id_to_multi_image_slot(int image_index, int area_id) {
  MCUBOOT_LOG_ERR("** flash_area_id_to_multi_image_slot");
  return -1;
}

// Return the number of sector in flash_areas[idx]
int flash_area_to_sectors(int idx, int *cnt, struct flash_area *ret) {
  MCUBOOT_LOG_ERR("** flash_area_to_sectors. Idx = %d, FA ID = %d", idx, ret->fa_id);
  cnt = 128;
  return 0;
}

/*
 * Retrieve a memory-mapped flash device's base address.
 *
 * On success, the address will be stored in the value pointed to by
 * ret.
 *
 * Returns 0 on success, or an error code on failure.
 */
int flash_device_base(uint8_t fd_id, uintptr_t *ret) {
  MCUBOOT_LOG_ERR("** flash_device_base");
  *ret = 0x00;
  return 0x00;
}