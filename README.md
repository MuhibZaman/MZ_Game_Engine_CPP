# MZ_Game_Engine_CPP
Game Engine from scratch in C++ on Linux
Following tutorial by Brendan Galea : https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR

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
    Added camera view transormation.
    Added keyboard input for moving camera.
    [CUSTOM]Added mouse input for rotating camera.
    Added the ability to supply model with an index buffer. Also updated device buffer storing to local storage using a staging buffer.
    Added the ability to load wavefront .obj files using tiny_obj_loader.
    Added directional lighting with diffuse and ambient models to vertex shader.
    Set up uniform buffers as an expansion to push constants. Set up descriptor struct for render system to be able to recieve frame information.
    Added vulkan descriptor sets that can be created and bound to pipeline during pipeline creation.
    Added point lighting in vertex shader.

Output:
    Opens a gray window with a 2 vases and a floot being lit by a point light. User can move around using keybaord input and look around using mouse movement. Press q to quit.