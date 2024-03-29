idf_component_register(
  INCLUDE_DIRS "include"
  REQUIRES "xtensa" "spi_flash" "spiffs"
)

cmake_minimum_required(VERSION 3.5)

set(NODEMCUJS_LIB nodemcujs-main)
set(ROOT_DIR ${CMAKE_SOURCE_DIR}/main)
set(NODEMCUJS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/main/src)

partition_table_get_partition_info(STORAGE_OFFSET "--partition-type data --partition-subtype spiffs" "offset")
idf_build_get_property(PYTHON PYTHON)
idf_build_get_property(IDF_PATH IDF_PATH)
idf_build_get_property(IDF_TARGET IDF_TARGET)

message("JERRYSCRIPT_ROOT: ${JERRYSCRIPT_ROOT}")

aux_source_directory(src NODEMCUJS_SRCS)
aux_source_directory(src/modules NODEMCUJS_SRCS_MODULES)
list(APPEND NODEMCUJS_SRCS ${NODEMCUJS_SRCS_MODULES})

# generate nodemcujs js module: nodemcujs_js.c nodemcujs_js.h
set(NODEMCUJS_JS_MODULES)
set(NODEMCUJS_JS_MODULES_PARAM)

file(GLOB NODEMCUJS_JS_FILES ${NODEMCUJS_SOURCE_DIR}/js/*.js)
foreach(FILE_PATH ${NODEMCUJS_JS_FILES})
  STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${FILE_PATH})
  list(APPEND NODEMCUJS_JS_MODULES ${FILE_NAME})
endforeach(FILE_PATH)

message("Configure nodemcujs js module:")
foreach(MODULE ${NODEMCUJS_JS_MODULES})
  message("-- ENABLE_JS_MODULE_${MODULE}= ON")
  list(APPEND NODEMCUJS_JS_MODULES_PARAM "${MODULE}=${NODEMCUJS_SOURCE_DIR}/js/${MODULE}.js")
endforeach()
message("Configure nodemcujs js module end\n")

# Run js2c
set(JS2C_RUN_MODE "release")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(JS2C_RUN_MODE "debug")
endif()

add_custom_command(
  OUTPUT ${NODEMCUJS_SOURCE_DIR}/nodemcujs_js.c ${NODEMCUJS_SOURCE_DIR}/nodemcujs_js.h
  COMMAND ${PYTHON} ${ROOT_DIR}/tools/js2c.py
  ARGS --buildtype=${JS2C_RUN_MODE}
       --modules '${NODEMCUJS_JS_MODULES_PARAM}'
       ${JS2C_SNAPSHOT_ARG}
  DEPENDS ${ROOT_DIR}/tools/js2c.py
          ${NODEMCUJS_SOURCE_DIR}/js/*.js
)

list(APPEND NODEMCUJS_SRCS ${NODEMCUJS_SOURCE_DIR}/nodemcujs_js.c ${NODEMCUJS_SOURCE_DIR}/nodemcujs_js.h)

# generate nodemcujs buildin module: nodemcujs_module_inl.h
message("Configure nodemcujs native module:")
set(NODEMCUJS_MODULE_INL_H "/*
 * Copyright 2021-present nodemcujs and other contributors
 *
 * This file is generated by CMakeLists.txt
 * Do not modify this.
 */

#include \"nodemcujs_module.h\"
")
set(NODEMCUJS_MODULE_ENTRIES)
set(NODEMCUJS_MODULE_INITIALIZERS)
set(NODEMCUJS_MODULE_OBJECTS)
file(GLOB NODEMCUJS_NATIVE_FILES ${NODEMCUJS_SOURCE_DIR}/modules/*.c)
list(LENGTH NODEMCUJS_NATIVE_FILES NODEMCUJS_MODULE_COUNT)
foreach(NATIVE_MODULE ${NODEMCUJS_NATIVE_FILES})
  string(REPLACE "${NODEMCUJS_SOURCE_DIR}/modules/" "" NATIVE_NAME "${NATIVE_MODULE}")
  string(REPLACE "nodemcujs_module_" "" NATIVE_NAME "${NATIVE_NAME}")
  string(REPLACE ".c" "" NATIVE_NAME "${NATIVE_NAME}")
  set(NODEMCUJS_MODULE_INITIALIZERS "${NODEMCUJS_MODULE_INITIALIZERS}
extern jerry_value_t nodemcujs_module_init_${NATIVE_NAME}();")
  set(NODEMCUJS_MODULE_ENTRIES "${NODEMCUJS_MODULE_ENTRIES}
  { \"${NATIVE_NAME}\", nodemcujs_module_init_${NATIVE_NAME} },")
  set(NODEMCUJS_MODULE_OBJECTS "${NODEMCUJS_MODULE_OBJECTS}
  { 0 },")
  message("-- ENABLE_NATIVE_MODULE_${NATIVE_NAME}= ON")
endforeach(NATIVE_MODULE)

file(WRITE ${NODEMCUJS_SOURCE_DIR}/nodemcujs_module_inl.h "${NODEMCUJS_MODULE_INL_H}
const unsigned nodemcujs_modules_count = ${NODEMCUJS_MODULE_COUNT};
${NODEMCUJS_MODULE_INITIALIZERS}

const nodemcujs_module_t nodemcujs_modules[] = {${NODEMCUJS_MODULE_ENTRIES}
};

nodemcujs_module_objects_t nodemcujs_module_objects[] = {${NODEMCUJS_MODULE_OBJECTS}
};
")
message("Configure nodemcujs native module end\n")
message("File Write: ${NODEMCUJS_SOURCE_DIR}/nodemcujs_module_inl.h")

list(APPEND NODEMCUJS_SRCS ${NODEMCUJS_SOURCE_DIR}/nodemcujs_module_inl.h)

add_library(${NODEMCUJS_LIB} STATIC ${NODEMCUJS_SRCS} nodemcujs_main.c)

# build jerryscript project

# Xtensa processor architecture optimization
set(EXTERNAL_COMPILE_FLAGS -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -Wno-frame-address)
string(REPLACE ";" "|" EXTERNAL_COMPILE_FLAGS_ALT_SEP "${EXTERNAL_COMPILE_FLAGS}")

externalproject_add(jerryscript_build
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${JERRYSCRIPT_ROOT}
  BUILD_IN_SOURCE 0
  BINARY_DIR jerryscript
  INSTALL_COMMAND "" # Do not install to host
  LIST_SEPARATOR | # Use the alternate list separator
  CMAKE_ARGS
    -DJERRY_GLOBAL_HEAP_SIZE=${JERRY_GLOBAL_HEAP_SIZE}
    -DJERRY_ERROR_MESSAGES=${JERRY_ERROR_MESSAGES}
    -DJERRY_CMDLINE=OFF
    -DENABLE_LTO=OFF # FIXME: This option must be turned off or the cross-compiler settings will be overwritten
    -DCMAKE_C_COMPILER_WORKS=true # cross-compiler
    -DCMAKE_SYSTEM_NAME=esp-idf
    -DCMAKE_SYSTEM_PROCESSOR=xtensa
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DEXTERNAL_COMPILE_FLAGS=${EXTERNAL_COMPILE_FLAGS_ALT_SEP}
    -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_LINKER=${CMAKE_LINKER}
    -DCMAKE_AR=${CMAKE_AR}
    -DCMAKE_NM=${CMAKE_NM}
    -DCMAKE_RANLIB=${CMAKE_RANLIB}
    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER
)

add_dependencies(${COMPONENT_LIB} jerryscript_build)

set(COMPONENT_BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/jerryscript)
message("jerryscript output: ${COMPONENT_BUILD_PATH}")

add_prebuilt_library(libjerry-core ${COMPONENT_BUILD_PATH}/lib/libjerry-core.a REQUIRES newlib PRIV_REQUIRES ${COMPONENT_NAME})
add_prebuilt_library(libjerry-ext ${COMPONENT_BUILD_PATH}/lib/libjerry-ext.a PRIV_REQUIRES ${COMPONENT_NAME})
add_prebuilt_library(libjerry-port-default-minimal ${COMPONENT_BUILD_PATH}/lib/libjerry-port-default-minimal.a PRIV_REQUIRES ${COMPONENT_NAME})

target_link_libraries(${NODEMCUJS_LIB} INTERFACE libjerry-core)
target_link_libraries(${NODEMCUJS_LIB} INTERFACE libjerry-ext)
target_link_libraries(${NODEMCUJS_LIB} INTERFACE libjerry-port-default-minimal)

target_include_directories(${NODEMCUJS_LIB} PUBLIC ${JERRYSCRIPT_ROOT}/jerry-core/include)
target_include_directories(${NODEMCUJS_LIB} PUBLIC ${JERRYSCRIPT_ROOT}/jerry-ext/include)

target_include_directories(${NODEMCUJS_LIB} PUBLIC include)
target_include_directories(${NODEMCUJS_LIB} PUBLIC src)

target_link_libraries(${NODEMCUJS_LIB} PRIVATE idf::spi_flash idf::spiffs idf::list idf::nvs_flash idf::esp_http_client)

target_link_libraries(${COMPONENT_LIB} INTERFACE ${NODEMCUJS_LIB})

# build spiffs image

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)

add_custom_command(
  OUTPUT flash_storage_cmd
  COMMAND ${PYTHON} ${IDF_PATH}/components/esptool_py/esptool/esptool.py
  ARGS --chip ${IDF_TARGET}
       write_flash
       -z ${STORAGE_OFFSET}
       ${CMAKE_BINARY_DIR}/storage.bin
  DEPENDS spiffs_storage_bin
)
add_custom_target(flash-storage DEPENDS flash_storage_cmd)