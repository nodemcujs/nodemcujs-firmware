cmake_minimum_required (VERSION 2.8.12)

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCDIRS .)

set(COMPONENT_REQUIRES "jerry-core" "jerry-ext")

register_component()
