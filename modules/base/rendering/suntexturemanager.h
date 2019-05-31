/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2019                                                               *
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

#ifndef __OPENSPACE_MODULE_BASE___SUNTEXTUREMANAGER___H__
#define __OPENSPACE_MODULE_BASE___SUNTEXTUREMANAGER___H__

//#include <thread>

#include <ghoul/opengl/texture.h>
#include <thread>
#include <queue>
#include <atomic>

namespace ghoul::opengl {
    class Texture;
} // namespace ghoul::opengl

namespace openspace {

class SunTextureManager {
public:
    SunTextureManager();
    
    void update(std::unique_ptr<ghoul::opengl::Texture>& texture);
    
    void initialDownload(std::unique_ptr<ghoul::opengl::Texture>& texture);
    void checkFilesInDirectory();
    void getNextTexture(std::string current, float dir, std::string * toReturn);

private:
    
    void initialDownloadBatch();
    void startUploadTextures();
    void startDownloadTexture(std::string textureId);

    void downloadTexture(std::string textureId);
 
    void uploadTexturesFromList(std::vector<std::string>& filelist);
    void uploadTextureFromName(std::string);
    void trimGPUList();
    std::string parseMagnetogramDate(std::string name);
    bool checkServerAliveness();
    std::string checkNextTextureId(std::string current, float dir);


    
    std::string getOpenSpaceDateTime();
    
    int _counter = 0;
    int _counter2 = 0;
    bool connected = true;
    // The _working atomic, is used to notify if a thread is being used and is currently being worked on
    std::atomic_bool _working = false;

    /* The _stage atomic, is to describe in what stage the suntexture manager is in,
       Kind of like a queue system if you will:
            Stage = 0  - Nothing going on right now, at this stage it is allowed to check what to do next
            Stage = 1  - Is or has downloaded a texture
            Stage = 2  - Is or has uploaded a texture from file to GPU
            Stage = 3  - Is ready to swap texture to a texture that has been uploaded to the gpu
    */ 
    std::atomic_size_t _stage = 0;
    mutable std::mutex _GPUListBlock;
    std::string _next = "";
    std::string _current;
    std::thread _dldthread;
    std::string _activeTextureDate = "NODATE";
    
    const unsigned int _maxTexturesOnGPU = 5; //every texture is around 250kb in size
    
    std::vector<std::string> _textureListDisk;
    std::unordered_map<std::string, std::unique_ptr<ghoul::opengl::Texture>> _textureListGPU;
    std::queue<std::string> _textureQueueGPU;

};

} // namespace openspace

#endif // __OPENSPACE_MODULE_BASE___SUNTEXTUREMANAGER___H__