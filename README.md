# MZ_Game_Engine_CPP
Game Engine from scratch in C++ on Linux
Following tutorial by Brendan Galea : https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR

Build Instructions:
Install VulkanSDK, GLFW, GLM, and GLSLC following vulkan-tutorial: https://vulkan-tutorial.com/Development_environment

Linux:
Simply run unixBuild.sh.
*Project was only tested on Pop!OS 24.04 running COSMIC Wayland.

Windows:
Make sure .env.cmake variables point to your installation paths.
From the MZ_Game_Engine+CPP directory, run:
Initial set up:
    mkdir build-windows
    cd build-windows
    cmake ..
Build:
    cd build-windows
    cmake --build . --config Release
    & "D:\Game Engine CPP\MZ_Game_Engine_CPP\build-windows\Release\LveEngine.exe"