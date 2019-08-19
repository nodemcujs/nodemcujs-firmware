cmake_minimum_required (VERSION 2.8.12)

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCDIRS .)

set(COMPONENT_PRIV_INCLUDEDIRS
    ${JERRYSCRIPT_SOURCE}/jerry-core/include
    ${JERRYSCRIPT_SOURCE}/jerry-ext/include
    ${JERRYSCRIPT_SOURCE}/jerry-port/default/include)

register_component()
