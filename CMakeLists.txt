
# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

if(DEFINED ENV{HOMEKIT_PATH})
  set(HOMEKIT_PATH $ENV{HOMEKIT_PATH})
else()
  message(FATAL_ERROR "Homekit SDK not found, please set HOMEKIT_PATH")
endif(DEFINED ENV{HOMEKIT_PATH})

set(EXTRA_COMPONENT_DIRS ${HOMEKIT_PATH}/components ${HOMEKIT_PATH}/components/homekit ${HOMEKIT_PATH}/examples/common $ENV{IDF_PATH}/examples/common_components/)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(TTS42)
