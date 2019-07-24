set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_SRCDIRS .)

set(COMPONENT_PRIV_INCLUDEDIRS
    ${JERRYSCRIPT_SOURCE}/jerry-core/include
    ${JERRYSCRIPT_SOURCE}/jerry-ext/include
    ${JERRYSCRIPT_SOURCE}/jerry-port/default/include)

register_component()

# Xtensa processor architecture optimization
set(EXTERNAL_COMPILE_FLAGS -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -w)
string(REPLACE ";" "|" EXTERNAL_COMPILE_FLAGS_ALT_SEP "${EXTERNAL_COMPILE_FLAGS}")

externalproject_add(jerryscript_build
  PREFIX ${COMPONENT_DIR}
  SOURCE_DIR ${JERRYSCRIPT_SOURCE}
  BUILD_IN_SOURCE 0
  BINARY_DIR jerryscript
  INSTALL_COMMAND "" # Do not install to host
  LIST_SEPARATOR | # Use the alternate list separator
  CMAKE_ARGS
    -DJERRY_GLOBAL_HEAP_SIZE=${JERRY_GLOBAL_HEAP_SIZE}
    -DJERRY_CMDLINE=OFF
    -DENABLE_LTO=OFF # FIXME: This option must be turned off or the cross-compiler settings will be overwritten
    -DCMAKE_C_COMPILER_WORKS=true # cross-compiler
    -DCMAKE_SYSTEM_NAME=Generic
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
add_dependencies(${COMPONENT_NAME} jerryscript_build)

set(COMPONENT_BUILD_PATH ${CMAKE_BINARY_DIR}/${COMPONENT_NAME}/jerryscript)

target_link_libraries(${COMPONENT_NAME}
                      ${COMPONENT_BUILD_PATH}/lib/libjerry-core.a
                      ${COMPONENT_BUILD_PATH}/lib/libjerry-ext.a
                      ${COMPONENT_BUILD_PATH}/lib/libjerry-port-default-minimal.a)


