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
  audiomodule.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  audiomodule.cpp
  audiomodule_lua.inl
)
source_group("Source Files" FILES ${SOURCE_FILES})

create_new_module(
  "Audio"
  audio_module
  STATIC
  ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES}
)

begin_dependency("SoLoud")
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "")
if (WIN32)
  set(SOLOUD_BACKEND_WINMM ON CACHE BOOL "")
elseif (UNIX AND NOT APPLE)
  set(SOLOUD_BACKEND_ALSA ON CACHE BOOL "")
elseif (APPLE)
  set(SOLOUD_BACKEND_COREAUDIO ON CACHE BOOL "")
endif ()
add_subdirectory(ext/soloud/contrib SYSTEM)

# Unfortunately, the soloud cmake tarket doesn't set the include directories correctly
target_include_directories(openspace-module-audio SYSTEM PRIVATE ext/soloud/include)
target_link_libraries(openspace-module-audio PRIVATE soloud)
set_property(TARGET soloud PROPERTY FOLDER "External")
end_dependency("SoLoud")
