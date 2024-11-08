add_executable(priorityInversionTests test.c ../src/Threads.c unity_config.c)
add_executable(taskTest priority_tests.c ../src/task1_task2.c unity_config.c)

target_link_libraries(priorityInversionTests PRIVATE
  pico_stdlib
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  unity
)

target_link_libraries(taskTest PRIVATE
  pico_stdlib
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  unity
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    target_link_libraries(priorityInversionTests PRIVATE
        pico_cyw43_arch_none
    )
endif()

if(PICO_CYW43_SUPPORTED)
    target_link_libraries(taskTest PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(priorityInversionTests 0)
pico_enable_stdio_usb(priorityInversionTests 1)

pico_enable_stdio_uart(taskTest 0)
pico_enable_stdio_usb(taskTest 1)

# This will speed up the simulation, since data won't be copied from the flash controller.
# It does make the binary non-persistent on a real microcontroller.
# pico_set_binary_type(priorityInversionTests no_flash)

find_program(RENODE renode
# This is extra stuff so it can find the mac installation paths.
 NAMES macos_run.command Renode.exe
 PATHS /Applications/Renode.app/Contents/MacOS)

set(RENODE_FLAGS
  --disable-xwt
  --port -2
  --pid-file renode.pid
  --console
  )

add_test(NAME simulate_priorityInversionTests COMMAND
    ${RENODE}
     ${RENODE_FLAGS}
    -e "$ELF=@$<TARGET_FILE:priorityInversionTests>; $WORKING=@${CMAKE_SOURCE_DIR}; include @${CMAKE_SOURCE_DIR}/test/simulate.resc; start"
    )

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(priorityInversionTests)
pico_add_extra_outputs(taskTest)

add_custom_target(flash_test picotool load -f $<TARGET_FILE:priorityInversionTests> DEPENDS priorityInversionTests)
