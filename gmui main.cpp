
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include "StockFlowApp.h"

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 800,
        "StockFlow — Online  Store Management",
        nullptr, nullptr);

    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(16, 16);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.18f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.20f, 0.45f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.30f, 0.65f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.48f, 0.38f, 0.78f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.20f, 0.45f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.30f, 0.65f, 0.80f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.12f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.30f, 0.65f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.24f, 0.55f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.10f, 0.18f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.22f, 0.22f, 0.32f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.58f, 0.48f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.40f, 0.85f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.30f, 0.55f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.38f, 0.30f, 0.65f, 0.50f);
    colors[ImGuiCol_Separator] = ImVec4(0.22f, 0.22f, 0.32f, 1.00f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    StockFlowApp app;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        app.Render();

        ImGui::Render();
        int dw, dh;
        glfwGetFramebufferSize(window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.08f, 0.08f, 0.10f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
