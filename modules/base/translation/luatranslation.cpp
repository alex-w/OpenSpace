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

#include <modules/base/translation/luatranslation.h>

#include <openspace/documentation/documentation.h>
#include <openspace/documentation/verifier.h>
#include <openspace/engine/globals.h>
#include <openspace/scripting/scriptengine.h>
#include <openspace/util/updatestructures.h>
#include <ghoul/filesystem/file.h>
#include <ghoul/filesystem/filesystem.h>
#include <ghoul/format.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/lua/ghoul_lua.h>
#include <ghoul/lua/lua_helper.h>
#include <chrono>

namespace {
    constexpr openspace::properties::Property::PropertyInfo ScriptInfo = {
        "Script",
        "Script",
        "This value is the path to the Lua script that will be executed to compute the "
        "translation for this transformation. The script needs to define a function "
        "'translation' that takes the current simulation time in seconds past the J2000 "
        "epoch as the first argument, the simulation time in seconds past the J2000 "
        "epoch of the last frame as the second argument, and the current wall time as "
        "milliseconds past the J2000 epoch the third argument and computes the three "
        "translation values returned as a table.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    // This `Translation` type generates the translation values used to offset the
    // attached scene graph node's position by calling the provided Lua script. The script
    // must return three translation factors, one for each principal axis, each providing
    // in meters. The script parameter describes in greater detail how the Lua script file
    // should be constructed.
    struct [[codegen::Dictionary(LuaTranslation)]] Parameters {
        // [[codegen::verbatim(ScriptInfo.description)]]
        std::filesystem::path script;
    };
#include "luatranslation_codegen.cpp"
} // namespace

namespace openspace {

documentation::Documentation LuaTranslation::Documentation() {
    return codegen::doc<Parameters>("base_transform_translation_lua");
}

LuaTranslation::LuaTranslation(const ghoul::Dictionary& dictionary)
    : Translation(dictionary)
    , _luaScriptFile(ScriptInfo)
{
    const Parameters p = codegen::bake<Parameters>(dictionary);

    _luaScriptFile.onChange([this]() {
        requireUpdate();
        _fileHandle = std::make_unique<ghoul::filesystem::File>(_luaScriptFile.value());
        _fileHandle->setCallback([this]() {
            requireUpdate();
            notifyObservers();
        });
    });
    addProperty(_luaScriptFile);
    global::scriptEngine->initializeLuaState(_state);

    _luaScriptFile = p.script.string();
}

glm::dvec3 LuaTranslation::position(const UpdateData& data) const {
    ghoul::lua::runScriptFile(_state, _luaScriptFile.value());

    // Get the scaling function
    lua_getglobal(_state, "translation");
    const bool isFunction = lua_isfunction(_state, -1);
    if (!isFunction) {
        LERRORC(
            "LuaTranslation",
            std::format(
                "Script '{}' does not have a function 'translation'",
                _luaScriptFile.value()
            )
        );
        return glm::dvec3(0.0);
    }

    // First argument is the number of seconds past the J2000 epoch in ingame time
    ghoul::lua::push(_state, data.time.j2000Seconds());

    // Second argument is the number of seconds past the J2000 epoch of the last frame
    ghoul::lua::push(_state, data.previousFrameTime.j2000Seconds());

    // Third argument is the number of milliseconds past the J2000 epoch in wallclock
    using namespace std::chrono;
    const auto now = high_resolution_clock::now();
    ghoul::lua::push(_state, duration_cast<milliseconds>(now.time_since_epoch()).count());

    // Execute the scaling function
    const int success = lua_pcall(_state, 3, 1, 0);
    if (success != 0) {
        LERRORC(
            "LuaTranslation",
            std::format("Error executing 'translation': {}", lua_tostring(_state, -1))
        );
    }

    const glm::dvec3 translation = ghoul::lua::value<glm::dvec3>(_state);
    return translation;
}

} // namespace openspace
