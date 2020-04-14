/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __FLASH_MAP_BACKEND_H__
#define __FLASH_MAP_BACKEND_H__

//#include <storage/flash_map.h> // the zephyr flash_map

#ifdef __cplusplus
extern "C" {
#endif

struct flash_area {
  uint8_t  fa_id;         /** The slot/scratch identification */
  uint8_t  fa_device_id;  /** The device id (usually there's only one) */
  uint16_t pad16;
  uint32_t fa_off;        /** The flash offset from the beginning */
  uint32_t fa_size;       /** The size of this sector */
};

/**
 * @brief Structure describing a sector within a flash area.
 *
 * Each sector has an offset relative to the start of its flash area
 * (NOT relative to the start of its flash device), and a size. A
 * flash area may contain sectors with different sizes.
 */
struct flash_sector {
  /**
   * Offset of this sector, from the start of its flash area (not device).
   */
  uint32_t fs_off;

  /**
   * Size of this sector, in bytes.
   */
  uint32_t fs_size;
};


/*< Opens the area for use. id is one of the `fa_id`s */
int     flash_area_open(uint8_t id, const struct flash_area **);
void    flash_area_close(const struct flash_area *);

/*< Reads `len` bytes of flash memory at `off` to the buffer at `dst` */
int     flash_area_read(const struct flash_area *, uint32_t off, void *dst, uint32_t len);

/*< Writes `len` bytes of flash memory at `off` from the buffer at `src` */
int     flash_area_write(const struct flash_area *, uint32_t off, const void *src, uint32_t len);

/*< Erases `len` bytes of flash memory at `off` */
int     flash_area_erase(const struct flash_area *, uint32_t off, uint32_t len);

/*< Returns this `flash_area`s alignment */
uint8_t flash_area_align(const struct flash_area *);

/*< What is value is read from erased flash bytes. */
uint8_t flash_area_erased_val(const struct flash_area *);

/*< Reads len bytes from off, and checks if the read data is erased. Returns
    1 if empty (that is containing erased value), 0 if not-empty, and -1 on
    failure. */
int     flash_area_read_is_empty(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);

/*< Given flash area ID, return info about sectors within the area. */
int     flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);

/*< Returns the `fa_id` for slot, where slot is 0 (primary) or 1 (secondary).
    `image_index` (0 or 1) is the index of the image. Image index is
    relevant only when multi-image support support is enabled */

int     flash_area_id_from_multi_image_slot(int image_index, int slot);

/*< Returns the slot (0 for primary or 1 for secondary), for the supplied
    `image_index` and `area_id`. `area_id` is unique and is represented by
    `fa_id` in the `flash_area` struct. */
int     flash_area_id_to_multi_image_slot(int image_index, int area_id);

__attribute__((deprecated))
int flash_area_to_sectors(int idx, int *cnt, struct flash_area *ret);

/*
 * Retrieve a memory-mapped flash device's base address.
 *
 * On success, the address will be stored in the value pointed to by
 * ret.
 *
 * Returns 0 on success, or an error code on failure.
 */
int flash_device_base(uint8_t fd_id, uintptr_t *ret);


#ifdef __cplusplus
}
#endif

#endif /* __FLASH_MAP_BACKEND_H__ */
