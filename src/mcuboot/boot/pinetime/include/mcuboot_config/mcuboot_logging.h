/*
 * Copyright (c) 2018 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MCUBOOT_LOGGING_H__
#define __MCUBOOT_LOGGING_H__

#ifndef __BOOTSIM__
#include <segger_rtt/SEGGER_RTT.h>

/*
 * When building for targets running Zephyr, delegate to its native
 * logging subsystem.
 */

#define MCUBOOT_LOG_MODULE_DECLARE(domain)	LOG_MODULE_DECLARE(domain, CONFIG_MCUBOOT_LOG_LEVEL)
#define MCUBOOT_LOG_MODULE_REGISTER(domain)	LOG_MODULE_REGISTER(domain, CONFIG_MCUBOOT_LOG_LEVEL)

#define MCUBOOT_LOG_ERR(...) SEGGER_RTT_printf(0,__VA_ARGS__)
#define MCUBOOT_LOG_WRN(...) SEGGER_RTT_printf(0,__VA_ARGS__)
#define MCUBOOT_LOG_INF(...) SEGGER_RTT_printf(0,__VA_ARGS__)
#define MCUBOOT_LOG_DBG(...) SEGGER_RTT_printf(0,__VA_ARGS__)
#define MCUBOOT_LOG_SIM(...) IGNORE(__VA_ARGS__)

//#include <logging/log.h>

#endif /* !__BOOTSIM__ */

#endif /* __MCUBOOT_LOGGING_H__ */
