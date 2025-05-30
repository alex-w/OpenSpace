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

#ifndef __OPENSPACE_MODULE_ISWA___RENDERABLETEXTUREPLANE___H__
#define __OPENSPACE_MODULE_ISWA___RENDERABLETEXTUREPLANE___H__

#include <modules/iswa/rendering/renderabletexturecygnet.h>

#include <ghoul/opengl/ghoul_gl.h>

namespace openspace {

namespace documentation { struct Documentation; }

/**
 * TexturePlane is a "concrete" IswaCygnet with texture as its input source. It handles
 * the creation, destruction and rendering of a plane geometry. It also specifies which
 * shaders to use and the uniforms that it needs.
 */
class RenderableTexturePlane : public RenderableTextureCygnet {
public:
    explicit RenderableTexturePlane(const ghoul::Dictionary& dictionary);
    virtual ~RenderableTexturePlane() = default;

    void initializeGL() override;
    void deinitializeGL() override;

    static documentation::Documentation Documentation();

private:
    bool createGeometry() override;
    void setUniforms() override;
    bool destroyGeometry() override;
    void renderGeometry() const override;

    GLuint _quad = 0;
    GLuint _vertexPositionBuffer = 0;
};

 } // namespace openspace

#endif // __OPENSPACE_MODULE_ISWA___RENDERABLETEXTUREPLANE___H__
