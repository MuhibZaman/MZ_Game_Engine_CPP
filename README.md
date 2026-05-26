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
    Opens a gray window with a rainbow sierpinski triangle.