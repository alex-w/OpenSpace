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

#ifndef __OPENSPACE_MODULE_BASE___DASHBOARDITEMINPUTSTATE___H__
#define __OPENSPACE_MODULE_BASE___DASHBOARDITEMINPUTSTATE___H__

#include <openspace/rendering/dashboardtextitem.h>

#include <openspace/properties/scalar/boolproperty.h>

namespace openspace {

namespace properties { class Property; }
namespace documentation { struct Documentation; }

class DashboardItemInputState : public DashboardTextItem {
public:
    explicit DashboardItemInputState(const ghoul::Dictionary& dictionary);
    ~DashboardItemInputState() override = default;

    void update() override;

    static documentation::Documentation Documentation();

private:
    properties::BoolProperty _showWhenEnabled;
    properties::BoolProperty _showWhenDisabled;

    properties::BoolProperty _showKeyboard;
    properties::BoolProperty _showMouse;
    properties::BoolProperty _showJoystick;
};

} // namespace openspace

#endif // __OPENSPACE_MODULE_BASE___DASHBOARDITEMINPUTSTATE___H__
