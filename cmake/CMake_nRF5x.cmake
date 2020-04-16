# convert toolchain path to bin path
if(DEFINED ARM_NONE_EABI_TOOLCHAIN_PATH)
    set(ARM_NONE_EABI_TOOLCHAIN_BIN_PATH ${ARM_NONE_EABI_TOOLCHAIN_PATH}/bin)
endif()

# must be set in file (not macro) scope (in macro would point to parent CMake directory)
set(DIR_OF_nRF5x_CMAKE ${CMAKE_CURRENT_LIST_DIR})

macro(nRF5x_toolchainSetup)
    include(${DIR_OF_nRF5x_CMAKE}/arm-gcc-toolchain.cmake)
endmacro()

macro(nRF5x_setup)
    if(NOT DEFINED ARM_GCC_TOOLCHAIN)
        message(FATAL_ERROR "The toolchain must be set up before calling this macro")
    endif()
    # fix on macOS: prevent cmake from adding implicit parameters to Xcode
    set(CMAKE_OSX_SYSROOT "/")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "")

    # language standard/version settings
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_CXX_STANDARD 11)

    if(NOT DEFINED NRF5_LINKER_SCRIPT)
        set(NRF5_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/gcc_nrf52.ld")
    endif()

    set(CPU_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
    add_definitions(-DNRF52 -DNRF52832 -DNRF52832_XXAA -DNRF52_PAN_74 -DNRF52_PAN_64 -DNRF52_PAN_12 -DNRF52_PAN_58 -DNRF52_PAN_54 -DNRF52_PAN_31 -DNRF52_PAN_51 -DNRF52_PAN_36 -DNRF52_PAN_15 -DNRF52_PAN_20 -DNRF52_PAN_55 -DBOARD_PCA10040)
    add_definitions(-DSOFTDEVICE_PRESENT -DS132 -DSWI_DISABLE0 -DBLE_STACK_SUPPORT_REQD -DNRF_SD_BLE_API_VERSION=6)
    add_definitions(-DFREERTOS)
    add_definitions(-DDEBUG_NRF_USER)
    add_definitions(-D__STARTUP_CLEAR_BSS)
    add_definitions(-D__HEAP_SIZE=8192)
    add_definitions(-D__STACK_SIZE=2048)


    set(COMMON_FLAGS "-MP -MD -mthumb -mabi=aapcs -Wall -g3 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin --short-enums ${CPU_FLAGS} -Wreturn-type -Werror=return-type")

    # compiler/assambler/linker flags
    set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -DSTDIO_RTT_ENABLE_BLOCKING_STDOUT")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
    set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_ASM_FLAGS "-MP -MD -std=c99 -x assembler-with-cpp")
    set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mabi=aapcs -std=gnu++98 -std=c99 -L ${NRF5_SDK_PATH}/modules/nrfx/mdk -T${NRF5_LINKER_SCRIPT} ${CPU_FLAGS} -Wl,--gc-sections --specs=nano.specs -lc -lnosys -lm")
    # note: we must override the default cmake linker flags so that CMAKE_C_FLAGS are not added implicitly
    set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <LINK_FLAGS> <OBJECTS> -lstdc++ -o <TARGET> <LINK_LIBRARIES>")

endmacro(nRF5x_setup)

# adds a target for comiling and flashing an executable
macro(nRF5x_addExecutable EXECUTABLE_NAME SOURCE_FILES)
    # executable
    add_executable(${EXECUTABLE_NAME} ${SDK_SOURCE_FILES} ${SOURCE_FILES})
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES SUFFIX ".out")
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "-Wl,-Map=${EXECUTABLE_NAME}.map")

    # additional POST BUILD setps to create the .bin and .hex files
    add_custom_command(TARGET ${EXECUTABLE_NAME}
      POST_BUILD
      COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE_NAME}.out
      COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_NAME}.out "${EXECUTABLE_NAME}.bin"
      COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_NAME}.out "${EXECUTABLE_NAME}.hex"
      COMMENT "post build steps for ${EXECUTABLE_NAME}")

    if(MERGEHEX)
        add_custom_command(TARGET ${EXECUTABLE_NAME}
          POST_BUILD
          COMMAND ${MERGEHEX} --merge ${EXECUTABLE_NAME}.hex ${NRF5_SDK_PATH}/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex --output ${EXECUTABLE_NAME}-full.hex
          COMMENT "merging HEX files")

        if(USE_JLINK)
            add_custom_target("FLASH_MERGED_${EXECUTABLE_NAME}"
              DEPENDS ${EXECUTABLE_NAME}
              COMMAND ${NRFJPROG} --program ${EXECUTABLE_NAME}-full.hex -f ${NRF_TARGET} --sectorerase
              COMMAND sleep 0.5s
              COMMAND ${NRFJPROG} --reset -f ${NRF_TARGET}
              COMMENT "flashing ${EXECUTABLE_NAME}-full.hex"
              )
        elseif(USE_GDB_CLIENT)
            add_custom_target("FLASH_MERGED_${EXECUTABLE_NAME}"
              DEPENDS ${EXECUTABLE_NAME}
              COMMAND ${GDB_CLIENT_BIN_PATH} -nx --batch -ex 'target extended-remote ${GDB_CLIENT_TARGET_REMOTE}' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load' -ex 'kill'  ${EXECUTABLE_NAME}-full.hex
              COMMENT "flashing ${EXECUTABLE_NAME}-full.hex"
              )
        elseif(USE_OPENOCD)
            add_custom_target("FLASH_MERGED_${EXECUTABLE_NAME}"
              DEPENDS ${EXECUTABLE_NAME}
              COMMAND ${OPENOCD_BIN_PATH} -c "tcl_port disabled" -c "gdb_port 3333" -c "telnet_port 4444" -f interface/stlink.cfg -c 'transport select hla_swd' -f target/nrf52.cfg -c "program \"${EXECUTABLE_NAME}-full.hex\""  -c reset -c shutdown
              COMMENT "flashing ${EXECUTABLE_NAME}-full.hex"
              )
        endif()
    endif()

    # custom target for flashing the board
    if(USE_JLINK)
        add_custom_target("FLASH_${EXECUTABLE_NAME}"
          DEPENDS ${EXECUTABLE_NAME}
          COMMAND ${NRFJPROG} --program ${EXECUTABLE_NAME}.hex -f ${NRF_TARGET} --sectorerase
          COMMAND sleep 0.5s
          COMMAND ${NRFJPROG} --reset -f ${NRF_TARGET}
          COMMENT "flashing ${EXECUTABLE_NAME}.hex"
          )
    elseif(USE_GDB_CLIENT)
        add_custom_target("FLASH_${EXECUTABLE_NAME}"
          DEPENDS ${EXECUTABLE_NAME}
          COMMAND ${GDB_CLIENT_BIN_PATH} -nx --batch -ex 'target extended-remote ${GDB_CLIENT_TARGET_REMOTE}' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load' -ex 'kill'  ${EXECUTABLE_NAME}.hex
          COMMENT "flashing ${EXECUTABLE_NAME}.hex"
          )
    elseif(USE_OPENOCD)
        add_custom_target("FLASH_${EXECUTABLE_NAME}"
          DEPENDS ${EXECUTABLE_NAME}
          COMMAND ${OPENOCD_BIN_PATH} -c "tcl_port disabled" -c "gdb_port 3333" -c "telnet_port 4444" -f interface/stlink.cfg -c 'transport select hla_swd' -f target/nrf52.cfg -c "program \"${EXECUTABLE_NAME}.hex\""  -c reset -c shutdown
          COMMENT "flashing ${EXECUTABLE_NAME}.hex"
          )
    endif()

endmacro()
