##########################################################################################
#                                                                                        #
# OpenSpace                                                                              #
#                                                                                        #
# Copyright (c) 2014-2025                                                                #
#                                                                                        #
# Permission is hereby granted, free of charge, to any person obtaining a copy of this   #
# software and associated documentation files (the "Software"), to deal in the Software  #
# without restriction, including without limitation the rights to use, copy, modify,     #
# merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     #
# permit persons to whom the Software is furnished to do so, subject to the following    #
# conditions:                                                                            #
#                                                                                        #
# The above copyright notice and this permission notice shall be included in all copies  #
# or substantial portions of the Software.                                               #
#                                                                                        #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,    #
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A          #
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT     #
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF   #
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE   #
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                          #
##########################################################################################

include(${PROJECT_SOURCE_DIR}/support/cmake/module_definition.cmake)

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/opensoundcontrolconnection.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/opensoundcontrolconnection.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

create_new_module(
  "OpenSoundControl"
  opensoundcontrol_module
  STATIC
  ${HEADER_FILES}
  ${SOURCE_FILES}
)

begin_dependency("oscpack")
add_subdirectory(ext/oscpack SYSTEM)
set_target_properties(oscpack PROPERTIES FOLDER "External")

if (MSVC)
  target_compile_options(oscpack PRIVATE "/W0")
  target_compile_definitions(oscpack PRIVATE "_SCL_SECURE_NO_WARNINGS")
else ()
  target_compile_options(oscpack PRIVATE "-w")
endif ()

target_include_directories(openspace-module-opensoundcontrol SYSTEM PRIVATE ext/oscpack)
target_link_libraries(openspace-module-opensoundcontrol PRIVATE oscpack)
end_dependency("oscpack")
