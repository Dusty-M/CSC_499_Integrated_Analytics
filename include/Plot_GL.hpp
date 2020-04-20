#ifndef PLOT_HPP
#define PLOT_HPP

#include <atlas/glx/Buffer.hpp>
#include <atlas/glx/Context.hpp>
#include <atlas/glx/ErrorCallback.hpp>
#include <atlas/glx/GLSL.hpp>
#include <atlas/gui/GUI.hpp>
#include <atlas/utils/Cameras.hpp>
#include <atlas/utils/LoadObjFile.hpp>

using namespace atlas;

struct ProgramData
{
    utils::MayaCamera::CameraData camera{};
    gui::GuiWindowData windowData{};
    gui::GuiRenderData renderData{};
    GLFWwindow* window{nullptr};
};

void onError(int code, char const* message);
void onMouseDown(ProgramData& data, int button, int action, int mode,
                 double xPos, double yPos);
void onMouseMove(ProgramData& data, double xPos, double yPos);
void onMouseScroll(ProgramData& data, double xOffset, double yOffset);
void onKeyPress(int key, int scancode, int action, int mods);
void onChar(unsigned int codepoint);

void init(ProgramData& data);

#endif
