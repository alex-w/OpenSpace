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

namespace openspace::luascriptfunctions {

int printInternal(ghoul::logging::LogLevel level, lua_State* L) {
    const int nArguments = lua_gettop(L);
    for (int i = 1; i <= nArguments; i++) {
        log(level, "print", ghoul::lua::luaValueToString(L, i));
    }
    lua_pop(L, nArguments);
    return 0;
}

int printTrace(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Trace, L);
}

int printDebug(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Debug, L);
}

int printInfo(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Info, L);
}

int printWarning(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Warning, L);
}

int printError(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Error, L);
}

int printFatal(lua_State* L) {
    return printInternal(ghoul::logging::LogLevel::Fatal, L);
}

} // namespace openspace::luascriptfunctions

namespace {

/**
 * Passes the argument to FileSystem::absolutePath, which resolves occuring path tokens
 * and returns the absolute path.
 */
[[codegen::luawrap("absPath")]] std::filesystem::path absolutePath(std::string path) {
    std::filesystem::path result = absPath(path);
    return result;
}

/**
 * Registers the path token provided by the first argument to the path in the second
 * argument. If the path token already exists, it will be silently overridden.
 */
[[codegen::luawrap]] void setPathToken(std::string pathToken, std::filesystem::path path)
{
    FileSys.registerPathToken(
        std::move(pathToken),
        std::move(path),
        ghoul::filesystem::FileSystem::Override::Yes
    );
}

// Checks whether the provided file exists.
[[codegen::luawrap]] bool fileExists(std::string file) {
    const bool e = std::filesystem::is_regular_file(absPath(std::move(file)));
    return e;
}

// Reads a file from disk and return its contents.
[[codegen::luawrap]] std::string readFile(std::filesystem::path file) {
    std::filesystem::path p = absPath(file);
    if (!std::filesystem::is_regular_file(p)) {
        throw ghoul::lua::LuaError(std::format("Could not open file '{}'", file));
    }

    std::ifstream f(p);
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string contents = buffer.str();
    return contents;
}

// Reads a file from disk and return its as a list of lines.
[[codegen::luawrap]] std::vector<std::string> readFileLines(std::filesystem::path file) {
    std::filesystem::path p = absPath(file);
    if (!std::filesystem::is_regular_file(p)) {
        throw ghoul::lua::LuaError(std::format("Could not open file '{}'", file));
    }

    std::ifstream f = std::ifstream(p);
    std::vector<std::string> contents;
    while (f.good()) {
        std::string line;
        ghoul::getline(f, line);
        contents.push_back(std::move(line));
    }

    return contents;
}

// Checks whether the provided directory exists.
[[codegen::luawrap]] bool directoryExists(std::filesystem::path file) {
    const bool e = std::filesystem::is_directory(absPath(std::move(file)));
    return e;
}

/**
 * Creates a directory at the provided path, returns true if directory was newly created
 * and false otherwise. If `recursive` flag is set to true, it will automatically create
 * any missing parent folder as well
 */
[[codegen::luawrap]] bool createDirectory(std::filesystem::path path,
    bool recursive = false)
{
    if (recursive) {
        return std::filesystem::create_directories(std::move(path));
    }
    else {
        return std::filesystem::create_directory(std::move(path));
    }
}

/**
 * Walks a directory and returns the contents of the directory as absolute paths. The
 * first argument is the path of the directory that should be walked, the second argument
 * determines if the walk is recursive and will continue in contained directories. The
 * default value for this parameter is "false". The third argument determines whether the
 * table that is returned is sorted. The default value for this parameter is "false".
 */
[[codegen::luawrap]] std::vector<std::filesystem::path> walkDirectory(
                                                               std::filesystem::path path,
                                                                   bool recursive = false,
                                                                      bool sorted = false)
{
    if (!std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
        return std::vector<std::filesystem::path>();
    }

    namespace fs = std::filesystem;
    return ghoul::filesystem::walkDirectory(
        path,
        ghoul::filesystem::Recursive(recursive),
        ghoul::filesystem::Sorted(sorted),
        [](const fs::path& p) { return fs::is_directory(p) || fs::is_regular_file(p); }
    );
}

/**
 * Walks a directory and returns the files of the directory as absolute paths. The first
 * argument is the path of the directory that should be walked, the second argument
 * determines if the walk is recursive and will continue in contained directories. The
 * default value for this parameter is "false". The third argument determines whether the
 * table that is returned is sorted. The default value for this parameter is "false".
 */
[[codegen::luawrap]] std::vector<std::filesystem::path> walkDirectoryFiles(
                                                               std::filesystem::path path,
                                                                   bool recursive = false,
                                                                      bool sorted = false)
{
    if (!std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
        return std::vector<std::filesystem::path>();
    }

    namespace fs = std::filesystem;
    return ghoul::filesystem::walkDirectory(
        path,
        ghoul::filesystem::Recursive(recursive),
        ghoul::filesystem::Sorted(sorted),
        [](const fs::path& p) { return fs::is_regular_file(p); }
    );
}

/**
 * Walks a directory and returns the subfolders of the directory as absolute paths. The
 * first argument is the path of the directory that should be walked, the second argument
 * determines if the walk is recursive and will continue in contained directories. The
 * default value for this parameter is "false". The third argument determines whether the
 * table that is returned is sorted. The default value for this parameter is "false".
 */
[[codegen::luawrap]] std::vector<std::filesystem::path> walkDirectoryFolders(
                                                               std::filesystem::path path,
                                                                   bool recursive = false,
                                                                      bool sorted = false)
{
    if (!std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
        return std::vector<std::filesystem::path>();
    }

    namespace fs = std::filesystem;
    return ghoul::filesystem::walkDirectory(
        path,
        ghoul::filesystem::Recursive(recursive),
        ghoul::filesystem::Sorted(sorted),
        [](const fs::path& p) { return fs::is_directory(p); }
    );
}

/**
 * This function extracts the directory part of the passed path. For example, if the
 * parameter is 'C:\\OpenSpace\\foobar\\foo.txt', this function returns
 * 'C:\\OpenSpace\\foobar'.
 */
[[codegen::luawrap]] std::filesystem::path directoryForPath(std::filesystem::path file) {
    std::filesystem::path path = std::filesystem::path(std::move(file)).parent_path();
    return path;
}

/**
 * This function extracts the contents of a zip file. The first argument is the path to
 * the zip file. The second argument is the directory where to put the extracted files. If
 * the third argument is true, the compressed file will be deleted after the decompression
 * is finished.
 */
[[codegen::luawrap]] void unzipFile(std::string source, std::string destination,
                                    bool deleteSource = false)
{
    if (!std::filesystem::exists(source)) {
        throw ghoul::lua::LuaError("Source file was not found");
    }

    struct zip_t* z = zip_open(source.c_str(), 0, 'r');
    const bool is64 = zip_is64(z) == 1;
    zip_close(z);

    if (is64) {
        throw ghoul::lua::LuaError(std::format(
            "Error while unzipping '{}': Zip64 archives are not supported", source
        ));
    }

    int ret = zip_extract(source.c_str(), destination.c_str(), nullptr, nullptr);
    if (ret != 0) {
        throw ghoul::lua::LuaError(std::format(
            "Error while unzipping '{}': {}", source, ret
        ));
    }

    if (deleteSource && std::filesystem::is_regular_file(source)) {
        std::filesystem::remove(source);
    }
}

/**
 * This function registers another Lua script that will be periodically executed as long
 * as the application is running. The `identifier` is used to later remove the script. The
 * `script` is being executed every `timeout` seconds. This timeout is only as accurate as
 * the framerate at which the application is running. Optionally the `preScript` Lua
 * script is run when registering the repeated script and the `postScript` is run when
 * unregistering it or when the application closes.
 * If the `timeout` is 0, the script will be executed every frame.
 * The `identifier` has to be a unique name that cannot have been used to register a
 * repeated script before. A registered script is removed with the #removeRepeatedScript
 * function.
 */
[[codegen::luawrap]] void registerRepeatedScript(std::string identifier,
                                                 std::string script, double timeout = 0.0,
                                                 std::string preScript = "",
                                                 std::string postScript = "")
{
    openspace::global::scriptEngine->registerRepeatedScript(
        std::move(identifier),
        std::move(script),
        timeout,
        std::move(preScript),
        std::move(postScript)
    );
}

/**
 * Removes a previously registered repeated script (see #registerRepeatedScript)
 */
[[codegen::luawrap]] void removeRepeatedScript(std::string identifier) {
    openspace::global::scriptEngine->removeRepeatedScript(identifier);
}

/**
 * Schedules a `script` to be run in `delay` seconds. The delay is measured in wallclock
 * time, which is seconds that occur in the real world, not in relation to the in-game
 * time.
 */
[[codegen::luawrap]] void scheduleScript(std::string script, double delay) {
    openspace::global::scriptEngine->scheduleScript(std::move(script), delay);
}

#include "scriptengine_lua_codegen.cpp"

} // namespace
