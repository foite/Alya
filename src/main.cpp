#include "gui/gui.hpp"
#include "manager/manager.hpp"
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace types;

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void init_config() {
  if (!std::filesystem::exists("config.json")) {
    std::ofstream file("config.json");
    nlohmann::json j;
    j["bots"] = nlohmann::json::array();
    j["timeout"] = 15;

    file << j.dump(2);
    file.close();
    spdlog::info("Config file created");
  }
}

int main() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  init_config();
  Manager manager{};

#if defined(IMGUI_IMPL_OPENGL_ES2)
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

  GLFWwindow *window = glfwCreateWindow(800, 400, "Alya", nullptr, nullptr);
  if (window == nullptr)
    return 1;
  glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  GUI::set_theme();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  std::string selected_bot;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(display_w), static_cast<float>(display_h));

    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("Alya", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::Text("Alya");
    ImGui::SameLine();
    if (ImGui::Button("Add bot"))
      ImGui::OpenPopup("Add bot");

    if (ImGui::BeginPopupModal("Add bot", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      static char username[64];
      static char password[64];
      static char recovery_code[64];
      static int method = 0;

      ImGui::InputText("Username", username, IM_ARRAYSIZE(username));
      ImGui::InputText("Password", password, IM_ARRAYSIZE(password));
      ImGui::InputText("Recovery code", recovery_code,
                       IM_ARRAYSIZE(recovery_code));
      ImGui::Combo("Method", &method, "Legacy\0Google\0Apple\0Ubisoft\0");

      if (ImGui::Button("Add")) {
        manager.add_bot(username, password, recovery_code, static_cast<ELoginMethod>(method),
                        true);
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Columns(2, "bots", false);
    ImGui::SetColumnWidth(0, 150);
    ImGui::BeginChild("scrolling", ImVec2(0, 0), true);
    for (auto &[username, bot_thread_tuple] : manager.bots) {
      auto &[bot, thread] = bot_thread_tuple;
      if (ImGui::Selectable(username.c_str())) {
        selected_bot = username;
      }
    }
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::BeginChild("info");
    if (!selected_bot.empty()) {
      auto bot = manager.get_bot(selected_bot);
      if (bot != nullptr) {
        ImGui::Text("Display name: %s", bot->info.display_name.c_str());
        ImGui::Text("Status: %s", bot->info.status.c_str());
        ImGui::SameLine();
        ImGui::Text("Timeout: %d", bot->info.timeout);
        ImGui::Text("Token: %s", bot->info.token.c_str());
        ImGui::Text("World: %s", bot->world.name.c_str());
        ImGui::Text("Position: %f, %f", bot->position.x / 32,
                    bot->position.y / 32);
        ImGui::Text("Ping: %d", bot->info.ping);

        static char world_name[64];
        ImGui::InputText("World name", world_name, IM_ARRAYSIZE(world_name));
        if (ImGui::Button("Warp")) {
          bot->warp(world_name);
        }
      }
      ImGui::Columns(3, nullptr, false);
      ImGui::SetColumnWidth(0, 60);
      ImGui::SetColumnWidth(1, 60);
      ImGui::SetColumnWidth(2, 60);

      ImGui::NextColumn();
      if (ImGui::Button("Up", ImVec2(50, 50))) {
        auto bot = manager.get_bot(selected_bot);
        if (bot != nullptr) {
          bot->walk(0, -1);
        }
      }
      ImGui::NextColumn();
      ImGui::NextColumn();

      if (ImGui::Button("Left", ImVec2(50, 50))) {
        auto bot = manager.get_bot(selected_bot);
        if (bot != nullptr) {
          bot->walk(-1, 0);
        }
      }
      ImGui::NextColumn();
      ImGui::NextColumn();
      if (ImGui::Button("Right", ImVec2(50, 50))) {
        auto bot = manager.get_bot(selected_bot);
        if (bot != nullptr) {
          bot->walk(1, 0);
        }
      }
      ImGui::NextColumn();

      ImGui::NextColumn();
      if (ImGui::Button("Down", ImVec2(50, 50))) {
        auto bot = manager.get_bot(selected_bot);
        if (bot != nullptr) {
          bot->walk(0, 1);
        }
      }
      ImGui::NextColumn();
      ImGui::NextColumn();

      ImGui::Columns(1);
    }

    ImGui::EndChild();
    ImGui::Columns(1);
    ImGui::End();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
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