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
  rendering/iswabasegroup.h
  rendering/iswadatagroup.h
  rendering/iswakameleongroup.h
  rendering/renderabledatacygnet.h
  rendering/renderabledataplane.h
  rendering/renderabledatasphere.h
  rendering/renderableiswacygnet.h
  rendering/renderablekameleonplane.h
  rendering/renderabletexturecygnet.h
  rendering/renderabletextureplane.h
  rendering/screenspacecygnet.h
  util/dataprocessor.h
  util/dataprocessorjson.h
  util/dataprocessorkameleon.h
  util/dataprocessortext.h
  util/iswamanager.h
  util/iswamanager_lua.inl
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  util/dataprocessor.cpp
  util/dataprocessorjson.cpp
  util/dataprocessorkameleon.cpp
  util/dataprocessortext.cpp
  util/iswamanager.cpp
  rendering/iswabasegroup.cpp
  rendering/iswadatagroup.cpp
  rendering/iswakameleongroup.cpp
  rendering/renderabledatacygnet.cpp
  rendering/renderabledataplane.cpp
  rendering/renderabledatasphere.cpp
  rendering/renderablekameleonplane.cpp
  rendering/renderableiswacygnet.cpp
  rendering/renderabletexturecygnet.cpp
  rendering/renderabletextureplane.cpp
  rendering/screenspacecygnet.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

set(SHADER_FILES
  shaders/dataplane_fs.glsl
  shaders/dataplane_vs.glsl
  shaders/textureplane_fs.glsl
  shaders/textureplane_vs.glsl
)

source_group("Shader Files" FILES ${SHADER_FILES})

create_new_module(
  "Iswa"
  iswa_module
  STATIC
  ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES}
)
target_precompile_headers(${iswa_module} PRIVATE
  <openspace/json.h>
  <future>
)
