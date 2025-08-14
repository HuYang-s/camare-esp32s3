
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/xtensa_context.S" "/workspace/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_context.S.obj"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/xtensa_intr_asm.S" "/workspace/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr_asm.S.obj"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/xtensa_vectors.S" "/workspace/build/esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_vectors.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "ESP_PLATFORM"
  "IDF_VER=\"v5.4.2\""
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  "_POSIX_READER_WRITER_LOCKS"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/include"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/deprecated_include"
  "/home/ubuntu/esp-idf-v5.4/components/newlib/platform_include"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/config/include"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/config/include/freertos"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/config/xtensa/include"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/FreeRTOS-Kernel/include"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/FreeRTOS-Kernel/portable/xtensa/include"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/FreeRTOS-Kernel/portable/xtensa/include/freertos"
  "/home/ubuntu/esp-idf-v5.4/components/freertos/esp_additions/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include/soc"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/include/soc/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/dma/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/ldo/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/debug_probe/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/port/esp32s3/."
  "/home/ubuntu/esp-idf-v5.4/components/esp_hw_support/port/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/heap/include"
  "/home/ubuntu/esp-idf-v5.4/components/heap/tlsf"
  "/home/ubuntu/esp-idf-v5.4/components/log/include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/soc/esp32s3/register"
  "/home/ubuntu/esp-idf-v5.4/components/hal/platform_port/include"
  "/home/ubuntu/esp-idf-v5.4/components/hal/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/hal/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3/include/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/esp_rom/esp32s3"
  "/home/ubuntu/esp-idf-v5.4/components/esp_common/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_system/include"
  "/home/ubuntu/esp-idf-v5.4/components/esp_system/port/soc"
  "/home/ubuntu/esp-idf-v5.4/components/esp_system/port/include/private"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/include"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/include/apps"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/include/apps/sntp"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/lwip/src/include"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/port/include"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/port/freertos/include"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/port/esp32xx/include"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/port/esp32xx/include/arch"
  "/home/ubuntu/esp-idf-v5.4/components/lwip/port/esp32xx/include/sys"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/eri.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/eri.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/eri.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/xt_trax.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xt_trax.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xt_trax.c.obj.d"
  "/home/ubuntu/esp-idf-v5.4/components/xtensa/xtensa_intr.c" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr.c.obj" "gcc" "esp-idf/xtensa/CMakeFiles/__idf_xtensa.dir/xtensa_intr.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
