
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_cache_writeback_esp32s3.S" "/workspace/build/bootloader/esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_cache_writeback_esp32s3.S.obj"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_longjmp.S" "/workspace/build/bootloader/esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_longjmp.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "BOOTLOADER_BUILD=1"
  "ESP_PLATFORM"
  "IDF_VER=\"v5.4.2\""
  "NON_OS_BUILD=1"
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3/include/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/log/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_common/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include/soc"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include/soc/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/dma/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/ldo/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/debug_probe/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/port/esp32s3/."
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/port/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/newlib/platform_include"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/include"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/deprecated_include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3/register"
  "/home/ubuntu/esp-idf-v5.4/components/hal/platform_port/include"
  "/home/ubuntu/esp-idf-v5.4/components/hal/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/hal/include"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_cache_esp32s2_esp32s3.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_cache_esp32s2_esp32s3.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_cache_esp32s2_esp32s3.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_crc.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_crc.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_crc.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_efuse.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_efuse.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_efuse.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_gpio.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_gpio.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_gpio.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_print.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_print.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_print.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_spiflash.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_spiflash.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_spiflash.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_sys.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_sys.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_sys.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_systimer.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_systimer.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_systimer.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_uart.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_uart.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_uart.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/patches/esp_rom_wdt.c" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_wdt.c.obj" "gcc" "esp-idf/esp_rom/CMakeFiles/__idf_esp_rom.dir/patches/esp_rom_wdt.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
