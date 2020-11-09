/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2020                                                               *
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

#ifndef __OPENSPACE_MODULE_STATEMACHINE___STATE___H__
#define __OPENSPACE_MODULE_STATEMACHINE___STATE___H__

#include <modules/statemachine/include/statemachine.h>
#include <ghoul/misc/dictionary.h>
#include <string>

namespace openspace {

class StateMachine;

class State {
public:
    State(const ghoul::Dictionary& dictionary);
    ~State() = default;

    // What should be done entering the state, while in the state and exiting the state
    void enter(openspace::StateMachine* statemachine);
    void idle(openspace::StateMachine* statemachine);
    void exit(openspace::StateMachine* statemachine);
    bool isIdle() const;
    std::string name() const;

private:
    bool _isIdle;
    std::string _name;
    std::string _enter;
    std::string _exit;
};

} // namespace openspace

#endif __OPENSPACE_MODULE_STATEMACHINE___STATE___H__