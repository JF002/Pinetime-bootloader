/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <startup/nrf.h>

void cleanup_arm_nvic(void) {
	/* Allow any pending interrupts to be recognized */
	__ISB();
	__disable_irq();

	/* Disable NVIC interrupts */
	for (uint8_t i = 0; i < 8; i++) {
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
	/* Clear pending NVIC interrupts */
	for (uint8_t i = 0; i < 0; i++) {
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}
}
