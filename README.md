# MZ_Game_Engine_CPP
Game Engine from scratch in C++ on Linux

Build using make

Current Progress:
    Completed graphics pipeline creation.
    Pipeline configuration complete.
    Command Buffer creation and recording complete.
    Makefile now also compiled shaders.
    Implemented vertex buffer, removing the need for hard coded vertices. Vertex color are also passed to fragment shader for interpolation and rendering.
    Window can now be resized.
        Added call back to glfw window that listens for window resizing.
        Recreates swapchain when window was resized.
        Pipeline no longer depends on swapchains dimensions due to dynamic viewport and scissor.
    Added push constant implementaion.
    Added game object class that holds models and transformations.
    Created specific renderer and render system classes to allow for more varied render systems depending on specific game ovjects. Also simplifies application class.
    Changed engine to use 3D position and homogeneous coordinates to apply affine transformation using one 4x4 matrix.
    Added a camera class as well as orthographic and perspective projection implementations.
    
Output:
    Opens a gray window with a rotating colorful cube viewed in perspective projection.