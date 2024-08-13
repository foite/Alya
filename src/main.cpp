#include "bot/item/item.hpp"
#include "bot/world/world.hpp"
#include "gui/gui.hpp"
#include "manager/manager.hpp"
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

using namespace types;

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

ImU32 hex_to_imcolor(uint32_t hex) {
  unsigned int r = (hex >> 24) & 0xFF;
  unsigned int g = (hex >> 16) & 0xFF;
  unsigned int b = (hex >> 8) & 0xFF;
  unsigned int a = hex & 0xFF;

  return IM_COL32(r, g, b, a);
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
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  GUI::set_theme();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  std::string selected_bot;
  std::string menu = "bots";

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize =
        ImVec2(static_cast<float>(display_w), static_cast<float>(display_h));

    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("Alya", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::Text("Alya");
    ImGui::SameLine();
    if (ImGui::Button("Bots")) {
      menu = "bots";
    }
    ImGui::SameLine();
    if (ImGui::Button("World")) {
      menu = "world";
    }
    ImGui::SameLine();
    if (ImGui::Button("Inventory")) {
      menu = "inventory";
    }
    if (menu == "bots") {
      ImGui::SameLine();
      if (ImGui::Button("Add bot")) {
        ImGui::OpenPopup("Add bot");
      }
    }
    if (!selected_bot.empty() && menu == "bots") {
      ImGui::SameLine();
      if (ImGui::Button("Remove bot")) {
        if (!selected_bot.empty()) {
          manager.remove_bot(selected_bot);
          selected_bot.clear();
        }
      }
    }

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
        manager.add_bot(username, password, recovery_code,
                        static_cast<ELoginMethod>(method), true);
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (menu == "bots") {
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
          ImGui::Text("| Timeout: %d", bot->info.timeout);
          ImGui::Text("Token: %s", bot->info.token.c_str());
          ImGui::Text("World: %s", bot->world.name.c_str());
          ImGui::Text("Position: %d, %d",
                      static_cast<int>(bot->position.x / 32),
                      static_cast<int>(bot->position.y / 32));
          ImGui::Text("Ping: %d", bot->info.ping);
          ImGui::Text("RID: %s", bot->info.login_info.rid.c_str());

          static char world_name[64];
          ImGui::InputText("World name", world_name, IM_ARRAYSIZE(world_name));
          if (ImGui::Button("Warp")) {
            bot->warp(world_name);
          }
        }
      }
      ImGui::EndChild();
      ImGui::Columns(1);
    }

    if (menu == "world") {
      ImGui::BeginChild("world");
      if (!selected_bot.empty()) {
        auto bot = manager.get_bot(selected_bot);
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 min = ImVec2(p.x, p.y);
        ImVec2 max = ImVec2(p.x + size.x, p.y + size.y);
        draw_list->AddRectFilled(min, max, IM_COL32(255, 255, 255, 255));

        float cell_width = size.x / bot->world.width;
        float cell_height = size.y / bot->world.height;

        for (int y = 0; y < bot->world.height; y++) {
          for (int x = 0; x < bot->world.width; x++) {
            if (y * bot->world.width + x >= bot->world.tiles.size()) {
              break;
            }
            Tile tile = bot->world.tiles[y * bot->world.width + x];
            if (bot->item_db->items.find(tile.foreground_item_id) ==
                bot->item_db->items.end()) {
              continue;
            }
            Item item = bot->item_db->items.at(tile.foreground_item_id);
            ImVec2 cell_min =
                ImVec2(min.x + x * cell_width, min.y + y * cell_height);
            ImVec2 cell_max =
                ImVec2(cell_min.x + cell_width, cell_min.y + cell_height);

            if (tile.foreground_item_id == 0 && tile.background_item_id == 0) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(0, 255, 255, 255));
            }
            if (item.name.find("Dirt") != std::string::npos) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(139, 69, 19, 255));
            }
            if (item.name.find("Bedrock") != std::string::npos) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(128, 128, 128, 255));
            }
            if (item.name.find("Rock") != std::string::npos) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(105, 105, 105, 255));
            }
            if (item.name.find("Lava") != std::string::npos) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(255, 0, 0, 255));
            }
            if (item.name.find("Door") != std::string::npos) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(139, 69, 19, 255));
            }
            if (bot->position.x / 32 == x && bot->position.y / 32 == y) {
              draw_list->AddRectFilled(cell_min, cell_max,
                                       IM_COL32(255, 0, 0, 255));
            }

            if (ImGui::IsMouseHoveringRect(cell_min, cell_max)) {
              ImGui::BeginTooltip();
              ImGui::Text("Position: %s|%s\nItem name: %s",
                          std::to_string(x).c_str(), std::to_string(y).c_str(),
                          item.name.c_str());
              ImGui::EndTooltip();
            }
          }
        }
      }
      ImGui::EndChild();
      if (!selected_bot.empty()) {
        ImGui::SetNextWindowPos(ImVec2(20, 45), ImGuiCond_Once);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
                              ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
        ImGui::Begin("World", nullptr,
                     ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize);
        auto bot = manager.get_bot(selected_bot);
        ImGui::Text("Name: %s", bot->world.name.c_str());
        ImGui::Text("Floating items: %s",
                    std::to_string(bot->world.dropped.items_count).c_str());
        ImGui::End();
        ImGui::PopStyleColor();
      }
      if (!selected_bot.empty()) {
        ImGui::SetNextWindowPos(ImVec2(20, 120), ImGuiCond_Once);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
                              ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
        ImGui::Begin("Movement", nullptr,
                     ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize);
        auto bot = manager.get_bot(selected_bot);
        if (ImGui::Button("Up")) {
          bot->walk(0, -1);
        }
        ImGui::SameLine();
        if (ImGui::Button("Down")) {
          bot->walk(0, 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("Left")) {
          bot->walk(-1, 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Right")) {
          bot->walk(1, 0);
        }
        ImGui::End();
        ImGui::PopStyleColor();
      }
    }

    if (menu == "inventory") {
      ImGui::BeginChild("inventory");
      if (!selected_bot.empty()) {
        auto bot = manager.get_bot(selected_bot);
        for (auto &[id, count] : bot->inventory.items) {
          ImGui::Text("%dx %s ", count,
                      bot->item_db->items.at(id).name.c_str());
        }
      }
      ImGui::EndChild();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}