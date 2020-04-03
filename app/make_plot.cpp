#include <iostream>
#include "CSVParser.hpp"
#include "LeastSquaresFit.hpp"
#include <vector>
#include <math.h>

#include <atlas/glx/Buffer.hpp>
#include <atlas/glx/Context.hpp>
#include <atlas/glx/ErrorCallback.hpp>
#include <atlas/glx/GLSL.hpp>
#include <atlas/gui/GUI.hpp>
#include <atlas/utils/Cameras.hpp>
#include <atlas/utils/LoadObjFile.hpp>
#include <magic_enum.hpp>
#include <fmt/printf.h>

#include "Plot.hpp"
#include <fmt/printf.h>
#include <magic_enum.hpp>

using namespace magic_enum::bitwise_operators;
using namespace atlas;
using namespace atlas::utils;
void errorCallback(int code, char const* message)
{
    fmt::print("error ({}): {}\n", code, message);
}

void onError(int code, char const* message)
{
	fmt::print("error ({}): {}\n", code, message);
}

void onMouseDown(ProgramData& data, int button, int action, int mode,
                 double xPos, double yPos)
{
	using namespace atlas::utils;

	gui::mousePressedCallback(data.windowData, button, action, mode);
	if (action == GLFW_PRESS)
	{
		math::Point2 point{ xPos, yPos };

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			data.camera.movement = utils::MayaCamera::Movements::Tumble;
			MayaCamera::mouseDown(data.camera, point);
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			data.camera.movement = utils::MayaCamera::Movements::Track;
			MayaCamera::mouseDown(data.camera, point);
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			data.camera.movement = utils::MayaCamera::Movements::Dolly;
			MayaCamera::mouseDown(data.camera, point);
		}
	}
	else
	{
		MayaCamera::mouseUp(data.camera);
	}
}

void onMouseMove(ProgramData& data, double xPos, double yPos)
{
	utils::MayaCamera::mouseMove(data.camera, { xPos, yPos });
}

void onMouseScroll(ProgramData& data, double xOffset, double yOffset)
{
	gui::mouseScrollCallback(xOffset, yOffset);
	utils::MayaCamera::mouseScroll(data.camera, { xOffset, yOffset });
}

void onKeyPress(int key, int scancode, int action, int mods)
{
	gui::keyPressCallback(key, scancode, action, mods);
}

void onChar(unsigned int codepoint)
{
	gui::charCallback(codepoint);
}

void init(ProgramData& data)
{
	glx::WindowSettings settings;
	settings.size = { 1280, 720 };
	settings.title = "Assignment 3 Solution";

	glx::WindowCallbacks callbacks;
	callbacks.mousePressCallback = [&data](int button, int action, int mode,
		double xPos, double yPos) {
			onMouseDown(data, button, action, mode, xPos, yPos);
	};
	callbacks.mouseMoveCallback = [&data](double xPos, double yPos) {
		onMouseMove(data, xPos, yPos);
	};
	callbacks.mouseScrollCallback = [&data](double xOffset, double yOffset) {
		onMouseScroll(data, xOffset, yOffset);
	};
	callbacks.keyPressCallback = onKeyPress;
	callbacks.charCallback = onChar;

	glx::initializeGLFW(onError);
	data.window = glx::createGLFWWindow(settings);

	glx::bindWindowCallbacks(data.window, callbacks);
	glfwMakeContextCurrent(data.window);
	glfwSwapInterval(1);

	glx::createGLContext(data.window, settings.version);
	using namespace atlas::core;
	glx::initializeGLCallback(glx::ErrorSource::All, glx::ErrorType::All,
		glx::ErrorSeverity::High |
		glx::ErrorSeverity::Medium);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	gui::initializeGuiWindowData(data.windowData);
	gui::initializeGuiRenderData(data.renderData);
	gui::setGuiWindow(data.windowData, data.window);
}

int main()
{

	ProgramData programData;
	init(programData);


/*
    glx::WindowSettings settings;
    glx::WindowCallbacks callbacks;

    {
        settings.size.width  = 1280;
        settings.size.height = 720;
        settings.title = "Lab 01";
    }

    {
        if (!glx::initializeGLFW(errorCallback))
        {
            return 1;
        }

        window = glx::createGLFWWindow(settings);
        if (window == nullptr)
        {
            return 1;
        }

        glx::bindWindowCallbacks(window, callbacks);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!glx::createGLContext(window, settings.version))
        {
            return 1;
        }

        using namespace atlas::core;
        glx::initializeGLCallback(glx::ErrorSource::All, glx::ErrorType::All,
                                  glx::ErrorSeverity::High | glx::ErrorSeverity::Medium);

    }
*/
	
	bool showDemoWindow {true};
//    while (!glfwWindowShouldClose(window))
	while(!glfwWindowShouldClose(programData.window))
    {
        int width, height;
        glfwGetFramebufferSize(programData.window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui::newFrame(programData.windowData);
		
// Some sample code used to make a few other widgets.  I'm keeping this
// around as an example for now.
/***********************************************************
		ImGui::Begin("Filter Controls");
		if(ImGui::Button("Reset Camera"))
		{
			utils::MayaCamera::resetCamera(programData.camera);
		}

		// set up ImGui widget
		if (showDemoWindow) {
			ImGui::ShowDemoWindow(&showDemoWindow);
		}
		showDemoWindow = !showDemoWindow;
		// to make custom widget, wrap it between
		// ImGui::Begin() and ImGui::End() calls
		ImGui::Begin("Demo Controls");
		ImGui::Checkbox("Show demo window", &showDemoWindow);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
*************************************************************/

		ImGui::Begin("Data Plot");

		if(showDemoWindow) {
			ImGui::ShowDemoWindow(&showDemoWindow);
		}
		std::vector<float> lines{};
		for(int i {0}; i < 120; ++i) {
			lines.push_back((float)i);
		}
		ImGui::PlotLines("graph y = x", lines.data(), 120);
		ImGui::End();
		ImGui::Render();
		gui::endFrame(programData.windowData, programData.renderData);
        glfwSwapBuffers(programData.window);
        glfwPollEvents();
    } // end of while loop

	gui::destroyGuiRenderData(programData.renderData);
	gui::destroyGuiWindow(programData.windowData);
	ImGui::DestroyContext();

    glx::destroyGLFWWindow(programData.window);
    glx::terminateGLFW();

    return 0;
}
