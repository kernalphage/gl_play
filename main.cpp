#include "Definitions.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "Material.hpp"
#include "Plotter.hpp"
#include "proc/Blob.hpp"
#include "proc/Cellular.hpp"
#include "proc/Flame.hpp"
#include "proc/flowmap.hpp"
#include "proc/proc_map.hpp"
#include "proc/Streamline.hpp"
#include "proc/Streamline.hpp"
#include "proc/Triangulate.hpp"
#include "proc/DifferentialGrowth.hpp"
#include "Processing.hpp"
#include "RenderTarget.hpp"
#include "sketches.hpp"
#include "Window.hpp"
#include <fmt/format.h>
#include <langinfo.h>
#include <rapidjson/rapidjson.h>
#include "proc/cached_flowmap.hpp"
#include "proc/Superformula.hpp"


using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <proc/SlimeMold.hpp>

Material* currentMaterial;


// settings
Processing *ctx;

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

int main() {
  Window mainWin;
  mainWin.init(1000,1000);

  glEnable(GL_DEBUG_OUTPUT);
  glfwSetErrorCallback(error_callback);

SlimeMold slime;
  slime.setup();

  vec4 clear_color{0.00f, 0.0f, 0.0f, 1.00f};
/// Game loop 
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
    //ImGui_ImplGlfwGL3_NewFrame();

    //ImGui::ColorEdit4("clear_color", (float *)&clear_color);
    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,mainWin._height, mainWin._height);
    if (glfwGetKey(mainWin.window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(mainWin.window, true);
      }
    slime.feedbackStep();
   // ImGui::Render();
   // ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(mainWin.window);

  } // end mainloop
  // Cleanup
  //ImGui_ImplGlfwGL3_Shutdown();
 // ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}
