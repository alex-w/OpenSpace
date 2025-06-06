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

#include <modules/iswa/rendering/screenspacecygnet.h>

#include <modules/iswa/util/iswamanager.h>
#include <openspace/engine/globals.h>
#include <openspace/scripting/scriptengine.h>
#include <openspace/util/timemanager.h>

namespace {
    struct [[codegen::Dictionary(ScreenSpaceCygnet)]] Parameters {
        int cygnetId;
        int updateInterval;
    };
#include "screenspacecygnet_codegen.cpp"
} // namespace

namespace openspace {

documentation::Documentation ScreenSpaceCygnet::Documentation() {
    return codegen::doc<Parameters>("iswa_screenspace_cygnet");
}

ScreenSpaceCygnet::ScreenSpaceCygnet(const ghoul::Dictionary& dictionary)
    : ScreenSpaceImageOnline(dictionary)
{
    const Parameters p = codegen::bake<Parameters>(dictionary);

    _cygnetId = p.cygnetId;
    _updateTime = p.updateInterval;

    _downloadImage = true;
    _texturePath = IswaManager::ref().iswaUrl(
        _cygnetId,
        global::timeManager->time().j2000Seconds()
    );

    _openSpaceTime = global::timeManager->time().j2000Seconds();
    _lastUpdateOpenSpaceTime = _openSpaceTime;

    _realTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    _lastUpdateRealTime = _realTime;
    _minRealTimeUpdateInterval = 100;

    _delete.onChange([this]() {
        const std::string script = std::format(
            "openspace.iswa.removeScreenSpaceCygnet({});", _cygnetId
        );
        global::scriptEngine->queueScript(script);
    });
}

void ScreenSpaceCygnet::update() {
    _openSpaceTime = global::timeManager->time().j2000Seconds();
    _realTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    bool timeToUpdate = fabs(_openSpaceTime - _lastUpdateOpenSpaceTime) >= _updateTime &&
           (_realTime.count() - _lastUpdateRealTime.count()) > _minRealTimeUpdateInterval;

    if (timeToUpdate) {
        _texturePath = IswaManager::ref().iswaUrl(
            _cygnetId,
            global::timeManager->time().j2000Seconds()
        );
        _lastUpdateRealTime = _realTime;
        _lastUpdateOpenSpaceTime = _openSpaceTime;
    }
}

} // namespace openspace
