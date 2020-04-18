cmake_minimum_required(VERSION 3.10)

project(pinetime-bootloader C CXX ASM)

# define some variables just for this example to determine file locations
set(NRF_PROJECT_NAME pinetime-bootloader)
set(NRF_BOARD pca10040)
#set(NRF_SOFTDEVICE s132)

nRF5x_toolchainSetup()
nRF5x_setup()

#nRF5x_addAppScheduler()
#nRF5x_addAppFIFO()
#nRF5x_addAppTimer()
#nRF5x_addAppUART()
#nRF5x_addAppButton()
#nRF5x_addBSP(FALSE FALSE FALSE)
#nRF5x_addAppGpiote()
#nRF5x_addBLEGATT()
#
#nRF5x_addBLEService(ble_lbs)

add_definitions(-DCONFIG_GPIO_AS_PINRESET)
add_definitions(-DDEBUG)

# Path to mbed-tls' asn1 parser library.
set(MBEDTLS_ASN1_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ext/mbedtls-asn1")
set(TINYCRYPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ext/tinycrypt/lib")

include_directories(.)

set(STARTUP_SOURCE_FILES
  startup/gcc_startup_nrf52.S
  startup/system_nrf52.c
  startup/system_nrf52.h
  startup/nrf.h
  startup/nrf52.h
  startup/nrf52_bitfields.h
  startup/nrf51_to_nrf52.h
  startup/nrf52_name_change.h
  startup/compiler_abstraction.h
  )

set(MCUBOOT_SOURCE_FILES
  ../bootutil/src/loader.c
  ../bootutil/src/swap_misc.c
  ../bootutil/src/swap_scratch.c
  ../bootutil/src/swap_move.c
  ../bootutil/src/bootutil_misc.c
  ../bootutil/src/image_validate.c
  ../bootutil/src/encrypted.c
  ../bootutil/src/image_rsa.c
  ../bootutil/src/image_ec256.c
  ../bootutil/src/image_ed25519.c
  ../bootutil/src/caps.c
  ../bootutil/src/tlv.c
  )

set(MBED_TLS_SOURCE_FILES
  ${MBEDTLS_ASN1_DIR}/src/asn1parse.c
  ${MBEDTLS_ASN1_DIR}/src/platform_util.c
  )

set(TINYCRIPT_SOURCE_FILES
  ${TINYCRYPT_DIR}/source/ecc.c
  ${TINYCRYPT_DIR}/source/ecc_dsa.c
  ${TINYCRYPT_DIR}/source/sha256.c
  ${TINYCRYPT_DIR}/source/utils.c
  )

list(APPEND SOURCE_FILES
  ${STARTUP_SOURCE_FILES}
  ${MCUBOOT_SOURCE_FILES}
  ${MBED_TLS_SOURCE_FILES}
  ${TINYCRIPT_SOURCE_FILES}

  segger_rtt/SEGGER_RTT.c
  segger_rtt/SEGGER_RTT_Syscalls_GCC.c
  segger_rtt/SEGGER_RTT_printf.c

  main.c
  os.c
  flash_map_backend.c
  arm_cleanup.c
  nrf52_nvmc.c
  nrf52_nvmc.h
)

set(INCLUDE_FILES

)

include_directories(
  startup/cmsis/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../bootutil/include
  include/
  ${MBEDTLS_ASN1_DIR}/include
  ${TINYCRYPT_DIR}/include

)

link_directories(
  ../../../../
)

nRF5x_addExecutable(pinetime-bootloader "${SOURCE_FILES}" ${INCLUDE_FILES})
