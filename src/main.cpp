// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>
#include <ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.h>

#include <ImGuiPack/ImGuiPack.h>

#include <res/CustomFont.cpp>
#include <res/Roboto_Medium.cpp>

#include <panes/ChartPane.h>
#include <panes/ExprPane.h>

#include <core/EzExprManager.h>

#include <cstdio>
#include <sstream>
#include <fstream>
#include <clocale>
#include <string>
#include <iostream>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>  // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>  // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void ApplyOrangeBlueTheme() {
    ImGuiStyle style;
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.26f, 0.28f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.21f, 0.29f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.71f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.93f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.18f, 0.20f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.23f, 0.25f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.30f, 0.33f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.21f, 0.29f, 0.36f, 0.89f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.13f, 0.52f, 0.94f, 0.45f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.13f, 0.71f, 1.00f, 0.89f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.24f, 0.78f, 0.78f, 0.31f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(1.00f, 0.60f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(1.00f, 0.48f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(1.00f, 0.40f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.13f, 0.52f, 0.94f, 0.66f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.13f, 0.52f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.13f, 0.52f, 0.94f, 0.59f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.18f, 0.35f, 0.58f, 0.59f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.20f, 0.41f, 0.68f, 0.00f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_TabSelected]           = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_TabDimmed]             = ImVec4(0.20f, 0.41f, 0.68f, 0.00f);
    style.Colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.13f, 0.52f, 0.94f, 0.95f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.Colors[ImGuiCol_WindowBg].w   = 1.00f;
    style.Colors[ImGuiCol_ChildBg].w    = 0.00f;
    style.Colors[ImGuiCol_MenuBarBg]    = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Main
    style.WindowPadding     = ImVec2(4.00f, 4.00f);
    style.FramePadding      = ImVec2(4.00f, 4.00f);
    style.ItemSpacing       = ImVec2(4.00f, 4.00f);
    style.ItemInnerSpacing  = ImVec2(4.00f, 4.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing     = 8.00f;
    style.ScrollbarSize     = 10.00f;
    style.GrabMinSize       = 8.00f;

    // Borders
    style.WindowBorderSize = 0.00f;
    style.ChildBorderSize  = 0.00f;
    style.PopupBorderSize  = 1.00f;
    style.FrameBorderSize  = 0.00f;
    style.TabBorderSize    = 0.00f;

    // Rounding
    style.WindowRounding    = 2.00f;
    style.ChildRounding     = 2.00f;
    style.FrameRounding     = 2.00f;
    style.PopupRounding     = 2.00f;
    style.ScrollbarRounding = 2.00f;
    style.GrabRounding      = 2.00f;
    style.TabRounding       = 2.00f;

    // Alignment
    style.WindowTitleAlign         = ImVec2(0.50f, 0.50f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ColorButtonPosition      = ImGuiDir_Right;
    style.ButtonTextAlign          = ImVec2(0.50f, 0.50f);
    style.SelectableTextAlign      = ImVec2(0.00f, 0.50f);

    // Safe Area Padding
    style.DisplaySafeAreaPadding = ImVec2(3.00f, 3.00f);

    ImGui::GetStyle() = style;
}

bool m_ShowImGui = false;
bool m_ShowMetric = false;
bool m_ShowImPlot = false;

void drawMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        const auto& io = ImGui::GetIO();
        LayoutManager::Instance()->DisplayMenu(io.DisplaySize);

        ImGui::Spacing();

        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::BeginMenu("Styles")) {
                ImGuiThemeHelper::Instance()->DrawMenu();

                ImGui::Separator();

                ImGui::MenuItem("Show ImGui", "", &m_ShowImGui);
                ImGui::MenuItem("Show ImGui Metric/Debug", "", &m_ShowMetric);
                ImGui::MenuItem("Show ImPlot", "", &m_ShowImPlot);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        // ImGui Infos
        static char label[256];
        ImFormatString(label, 256, "Dear ImGui %s (Docking)", ImGui::GetVersion());
        const auto size                      = ImGui::CalcTextSize(label);
        static float s_translation_menu_size = 0.0f;

        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - s_translation_menu_size - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", label);

        ImGui::EndMainMenuBar();
    }
}

void drawMainStatusBar() {
    if (ImGui::BeginMainStatusBar()) {
        Messaging::Instance()->DrawStatusBar();

        //  ImGui Infos
        const auto& io  = ImGui::GetIO();
        static char fps[256];
        ImFormatString(fps, 256, "%.1f ms/frame (%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
        const auto size = ImGui::CalcTextSize(fps);
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", fps);

        // MainFrontend::sAnyWindowsHovered |= ImGui::IsWindowHovered();

        ImGui::EndMainStatusBar();
    }
}

int main(int, char**) {
#ifdef _MSC_VER
    // active memory leak detector
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    auto loc = std::setlocale(LC_ALL, ".UTF8");
    if (!loc) {
        printf("setlocale fail to apply with this compiler. it seems the unicode will be NOK\n");
    }
    std::setlocale(LC_NUMERIC, "C");

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // Decide GL+GLSL versions

#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "EzEzExpr Demo", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false;  // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoDecoration = false;              // no decoration, for avoid a resuize bug
    io.FontAllowUserScaling        = true;               // zoom wiht ctrl + mouse wheel

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    float font_size = 15.0f;

    // Fonts
    {
        ImFontConfig icons_config_0;
        icons_config_0.GlyphRanges = ImGui::GetIO().Fonts->GetGlyphRangesChineseFull();
        // ImGui::GetIO().Fonts->AddFontDefault(&icons_config_0);
    }
    {
        ImFontConfig icons_config_1;
        icons_config_1.GlyphRanges = ImGui::GetIO().Fonts->GetGlyphRangesChineseFull();
        // ImGui::GetIO().Fonts->AddFontFromFileTTF("NotoSansSCRegular.ttf", vFontSize, &icons_config_1);
    }
    {  // Boboto Font
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_RM, font_size);
    }
    {  // glyphs
        static const ImWchar icons_ranges_3[] = {ICON_MIN_IGFD, ICON_MAX_IGFD, 0};
        ImFontConfig icons_config_3;
        icons_config_3.MergeMode  = true;
        icons_config_3.PixelSnapH = true;
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, font_size, &icons_config_3, icons_ranges_3);
    }

    ImGui::CustomStyle::Init();
    ImGui::SetPUSHID(4577);

    ApplyOrangeBlueTheme();

    LayoutManager::Instance()->Init("Layouts", "Default Layout");
    LayoutManager::Instance()->AddPane(ChartPane::Instance(), "Chart", "", "CENTRAL", 0.0f, true, true);
    LayoutManager::Instance()->AddPane(ExprPane::Instance(), "Expressions", "", "RIGHT", 0.4f, true, false);
    LayoutManager::Instance()->InitPanes();

    EzExprManager::instance()->init();

    ImVec4 viewportRect;
    
    ImVec4 clear_color = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);

    const auto context_ptr = ImGui::GetCurrentContext();

    // Main loop
    int32_t display_w = 0;
    int32_t display_h = 0;
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::CustomStyle::ResetCustomId();

        drawMainMenuBar();
        drawMainStatusBar();

        if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
            LayoutManager::Instance()->EndDockSpace();
        }

        LayoutManager::Instance()->DrawPanes(0, context_ptr, {});

        ImGuiThemeHelper::Instance()->Draw();
        LayoutManager::Instance()->InitAfterFirstDisplay(io.DisplaySize);

        if (m_ShowImGui) {
            ImGui::ShowDemoWindow(&m_ShowImGui);
        }

        if (m_ShowImPlot) {
            ImPlot::ShowDemoWindow(&m_ShowImPlot);
        }

        if (m_ShowMetric) {
            ImGui::ShowMetricsWindow(&m_ShowMetric);
        }

#ifdef IMGUI_HAS_VIEWPORT
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            const auto viewport = ImGui::GetMainViewport();
            if (viewport) {
                const auto pos  = viewport->WorkPos;
                const auto size = viewport->WorkSize;
                viewportRect.x  = pos.x;
                viewportRect.y  = pos.y;
                viewportRect.z  = size.x;
                viewportRect.w  = size.y;
            }
        } else {
            viewportRect.x = 0;
            viewportRect.y = 0;
        }
#endif

        // Cpu Zone : prepare
        ImGui::Render();

        // GPU Zone : Rendering
        glfwMakeContextCurrent(window);

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifdef IMGUI_HAS_VIEWPORT
        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
#endif
        glfwSwapBuffers(window);
    }

    // Cleanup
    LayoutManager::Instance()->UnitPanes();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
