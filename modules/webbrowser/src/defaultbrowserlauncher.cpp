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

#include <modules/webbrowser/include/defaultbrowserlauncher.h>

#include <ghoul/logging/logmanager.h>

#ifdef WIN32
#include <shellapi.h>
#endif // WIN32

namespace openspace {

bool DefaultBrowserLauncher::OnBeforePopup(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                                           [[maybe_unused]] const CefString& targetUrl,
                                           const CefString&,
                                           CefLifeSpanHandler::WindowOpenDisposition,
                                           bool, const CefPopupFeatures&, CefWindowInfo&,
                                           CefRefPtr<CefClient>&, CefBrowserSettings&,
                                           CefRefPtr<CefDictionaryValue>&,
                                           bool*)
{
    // never permit CEF popups, always launch in default browser
#ifdef WIN32
    std::string url = targetUrl.ToString();
    LDEBUGC("DefaultBrowserLauncher", "Launching default browser: " + url);
    ShellExecuteA(nullptr, nullptr, url.c_str(), nullptr, nullptr, SW_SHOW);
#endif // WIN32
    return true;
}

} // namespace openspace
