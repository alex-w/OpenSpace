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

#include <modules/cefwebgui/cefwebguimodule.h>

#include <modules/webbrowser/webbrowsermodule.h>
#include <modules/cefwebgui/include/guirenderhandler.h>
#include <modules/cefwebgui/include/guikeyboardhandler.h>
#include <modules/webbrowser/include/browserinstance.h>
#include <openspace/documentation/documentation.h>
#include <openspace/engine/globals.h>
#include <openspace/engine/globalscallbacks.h>
#include <openspace/engine/moduleengine.h>
#include <openspace/engine/windowdelegate.h>
#include <ghoul/format.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/misc/dictionary.h>
#include <ghoul/misc/profiling.h>

namespace {
    constexpr openspace::properties::Property::PropertyInfo EnabledInfo = {
        "Enabled",
        "Enabled",
        "This setting determines whether the browser should be enabled or not.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo ReloadInfo = {
        "Reload",
        "Reload",
        "Trigger this property to reload the browser.",
        openspace::properties::Property::Visibility::User
    };

    constexpr openspace::properties::Property::PropertyInfo VisibleInfo = {
        "Visible",
        "Is Visible",
        "This setting determines whether the browser should be visible or not.",
        openspace::properties::Property::Visibility::User
    };

    constexpr openspace::properties::Property::PropertyInfo GuiUrlInfo = {
        "GuiUrl",
        "GUI URL",
        "The URL of the webpage that is used to load the WebGUI from.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo GuiScaleInfo = {
        "GuiScale",
        "Gui Scale",
        "GUI scale multiplier.",
        openspace::properties::Property::Visibility::Always
    };

    struct [[codegen::Dictionary(CefWebGuiModule)]] Parameters {
        // [[codegen::verbatim(GuiScaleInfo.description)]]
        std::optional<float> guiScale;

        // [[codegen::verbatim(EnabledInfo.description)]]
        std::optional<bool> enabled;

        // [[codegen::verbatim(VisibleInfo.description)]]
        std::optional<bool> visible;

    };
#include "cefwebguimodule_codegen.cpp"
} // namespace

namespace openspace {

documentation::Documentation CefWebGuiModule::Documentation() {
    return codegen::doc<Parameters>("module_cefwebgui");
}

CefWebGuiModule::CefWebGuiModule()
    : OpenSpaceModule(CefWebGuiModule::Name)
    , _enabled(EnabledInfo, true)
    , _visible(VisibleInfo, true)
    , _reload(ReloadInfo)
    , _url(GuiUrlInfo, "")
    , _guiScale(GuiScaleInfo, 1.f, 0.1f, 3.f)
{
    addProperty(_enabled);
    addProperty(_visible);
    addProperty(_reload);
    addProperty(_url);
    addProperty(_guiScale);
}

void CefWebGuiModule::startOrStopGui() {
    ZoneScoped;

    WebBrowserModule* webBrowserModule = global::moduleEngine->module<WebBrowserModule>();

    const bool isMaster = global::windowDelegate->isMaster();

    if (_enabled && isMaster) {
        LDEBUGC("WebBrowser", std::format("Loading GUI from '{}'", _url.value()));

        if (!_instance) {
            _instance = std::make_unique<BrowserInstance>(
                new GUIRenderHandler,
                new GUIKeyboardHandler
            );
            _instance->initialize();
            _instance->reshape(static_cast<glm::ivec2>(
                static_cast<glm::vec2>(global::windowDelegate->guiWindowResolution()) *
                global::windowDelegate->dpiScaling()
            ));
            if (!_url.value().empty()) {
                _instance->loadUrl(_url);
            }
        }
        if (_visible) {
            webBrowserModule->attachEventHandler(_instance.get());
        }

        _instance->setZoom(_guiScale);

        webBrowserModule->addBrowser(_instance.get());
    }
    else if (_instance) {
        _instance->close(true);
        webBrowserModule->removeBrowser(_instance.get());
        webBrowserModule->detachEventHandler();
        _instance.reset();
    }
}

void CefWebGuiModule::internalInitialize(const ghoul::Dictionary& configuration) {
    ZoneScoped;

    WebBrowserModule* webBrowserModule =
        global::moduleEngine->module<WebBrowserModule>();

    const bool available = webBrowserModule && webBrowserModule->isEnabled();
    if (!available) {
        return;
    }

    _enabled.onChange([this]() {
        ZoneScopedN("CefWebGuiModule::enabled");

        startOrStopGui();
    });

    _url.onChange([this]() {
        ZoneScopedN("CefWebGuiModule::url");

        if (_instance) {
            _instance->loadUrl(_url);
        }
    });

    _reload.onChange([this]() {
        ZoneScopedN("CefWebGuiModule::reload");

        if (_instance) {
            _instance->reloadBrowser();
        }
    });

    _guiScale.onChange([this]() {
        ZoneScopedN("CefWebGuiModule::guiScale");

        if (_instance) {
            _instance->setZoom(_guiScale);
        }
    });

    _visible.onChange([this, webBrowserModule]() {
        ZoneScopedN("CefWebGuiModule::visible");

        if (_visible && _instance) {
            webBrowserModule->attachEventHandler(_instance.get());
        }
        else {
            webBrowserModule->detachEventHandler();
        }
    });

    // We need this to make sure that the browser is reloaded
    // once the endpoint comes online, on OpenSpace startup.

    // TODO: See if the hardcoded endpoint `gui` below can be removed.
    // Possible fix: Reload browser if cefwebgui is routed to localhost
    // and the same endpoint that just came online.
    WebGuiModule* webGuiModule = global::moduleEngine->module<WebGuiModule>();

    _endpointCallback = webGuiModule->addEndpointChangeCallback(
        [this](const std::string& endpoint, bool exists) {
            ZoneScopedN("CefWebGuiModule::endpointCallback");
            if (exists && endpoint == "gui" && _instance) {
                _instance->reloadBrowser();
            }
        }
    );

    const Parameters p = codegen::bake<Parameters>(configuration);
    _guiScale = p.guiScale.value_or(_guiScale);
    _enabled = p.enabled.value_or(_enabled);
    _visible = p.visible.value_or(_visible);

    global::callback::initializeGL->emplace_back([this]() {
        startOrStopGui();
    });

    global::callback::postDraw->emplace_back([this]() {
        bool windowChanged = global::windowDelegate->anyWindowHasResized();
        if (_instance && (windowChanged || _instance->_shouldReshape)) {
            const glm::ivec2 res = global::windowDelegate->guiWindowResolution();
            _instance->reshape(static_cast<glm::ivec2>(
                glm::vec2(res) * global::windowDelegate->dpiScaling()
            ));
            _instance->_shouldReshape = false;
        }
    });

    global::callback::draw2D->emplace_back([this](){
        ZoneScopedN("CefWebGuiModule");

        const bool isGuiWindow =
            global::windowDelegate->hasGuiWindow() ?
            global::windowDelegate->isGuiWindow() :
            global::windowDelegate->currentWindowId()
                == global::windowDelegate->firstWindowId();
        const bool isMaster = global::windowDelegate->isMaster();

        if (isGuiWindow && isMaster && _instance) {
            if (_visible) {
                _instance->draw();
            }
        }
    });

    global::callback::deinitializeGL->emplace_back([this]() {
        ZoneScopedN("CefWebGuiModule");

        if (_endpointCallback != -1) {
            WebGuiModule* m = global::moduleEngine->module<WebGuiModule>();
            m->removeEndpointChangeCallback(_endpointCallback);
            _endpointCallback = -1;
        }
        _enabled = false;
        startOrStopGui();
    });
}

} // namespace openspace
