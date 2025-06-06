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

#ifndef __OPENSPACE_CORE___SCENEINITIALIZER___H__
#define __OPENSPACE_CORE___SCENEINITIALIZER___H__

#include <openspace/util/threadpool.h>
#include <unordered_set>
#include <vector>

namespace openspace {

class SceneGraphNode;

/**
 * This class is responsible for initializing nodes, in a multithreaded fashion if needed,
 * that are passed into it. The constructor takes the number of extra separate threads
 * that are used to initialize nodes. Passing `0` for the number of threads results in
 * nodes being initialized on the main thread instead.
 */
class SceneInitializer {
public:
    SceneInitializer(unsigned int nThreads = 0);

    void initializeNode(SceneGraphNode* node);
    std::vector<SceneGraphNode*> takeInitializedNodes();
    bool isInitializing() const;

private:
    std::vector<SceneGraphNode*> _initializedNodes;
    std::unordered_set<SceneGraphNode*> _initializingNodes;
    const unsigned int _nThreads;
    ThreadPool _threadPool;
    mutable std::mutex _mutex;
};

} // namespace openspace

#endif // __OPENSPACE_CORE___SCENEINITIALIZER___H__
