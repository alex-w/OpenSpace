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

#ifndef __OPENSPACE_CORE___UVEC4PROPERTY___H__
#define __OPENSPACE_CORE___UVEC4PROPERTY___H__

#include <openspace/properties/numericalproperty.h>

#include <ghoul/glm.h>
#include <limits>

namespace openspace::properties {

class UVec4Property : public NumericalProperty<glm::uvec4> {
public:
    UVec4Property(Property::PropertyInfo info, glm::uvec4 value = glm::uvec4(0),
        glm::uvec4 minValue = glm::uvec4(std::numeric_limits<unsigned int>::lowest()),
        glm::uvec4 maxValue = glm::uvec4(std::numeric_limits<unsigned int>::max()),
        glm::uvec4 stepValue = glm::uvec4(1));

    std::string_view className() const override final;
    ghoul::lua::LuaTypes typeLua() const override final;

    void getLuaValue(lua_State* state) const override final;

    std::string stringValue() const override final;
    using TemplateProperty<glm::uvec4>::operator=;

private:
    glm::uvec4 toValue(lua_State* state) const override final;
};

} // namespace openspace::properties

#endif // __OPENSPACE_CORE___UVEC4PROPERTY___H__
