cmake_minimum_required (VERSION 2.8.12)

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCDIRS .)

set(COMPONENT_REQUIRES "list" "jerry-core" "jerry-ext")
set(COMPONENT_PRIV_REQUIRES list)

register_component()
