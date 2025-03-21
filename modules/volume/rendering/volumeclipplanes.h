/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2025                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#ifndef __OPENSPACE_MODULE_VOLUME___VOLUMECLIPPLANES___H__
#define __OPENSPACE_MODULE_VOLUME___VOLUMECLIPPLANES___H__

#include <openspace/properties/propertyowner.h>

#include <modules/volume/rendering/volumeclipplane.h>
#include <openspace/properties/scalar/intproperty.h>
#include <vector>

namespace ghoul { class Dictionary; }

namespace openspace::volume {

class VolumeClipPlane;

class VolumeClipPlanes : public properties::PropertyOwner {
public:
    explicit VolumeClipPlanes(const ghoul::Dictionary& dictionary);
    ~VolumeClipPlanes() override = default;

    void initialize();

    std::vector<glm::vec3> normals();
    std::vector<glm::vec2> offsets();

private:
    properties::IntProperty _nClipPlanes;
    std::vector<VolumeClipPlane> _clipPlanes;
};

} // namespace openspace::volume

#endif // __OPENSPACE_MODULE_VOLUME___VOLUMECLIPPLANES___H__
