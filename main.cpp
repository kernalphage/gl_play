#include "Definitions.h"
#include "Material.h"
#include "Processing.h"
#include "Triangulate.hpp"
#include <algorithm>

#include <iostream>

#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"

using namespace std;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// settings
Processing* ctx;
float t = 3;

void render(){
  std::vector<Tri> seed = { Tri{{0,.8},{-.8,-.8}, {.8, -.8}, 0} };
  ctx->polygon({{0,.8},{-.8,-.8}, {.8, -.8}}, true);
  Tri::triangulate(seed, t, ctx);

  ctx->flush();
}


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void drawCircle(int  numPts, float t, Processing* ctx );

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE){
    t++;
    //ctx->clear();
    drawCircle(t, t, ctx);

    ctx->flush();
    //render();
  }
}


int main() {

  glfwInit();
  cout<<glfwGetVersionString()<<endl;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, keyCallback);

 // build and compile our shader program

  	Material basic{"basic.vert", "basic.frag"};
  ctx = new Processing{};
  render();
  //drawCircle(t,t,ctx);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);


    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    bool show_demo_window = true;
    bool show_another_window = false;
    vec4 clear_color {0.45f, 0.55f, 0.60f, 1.00f};


  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        processInput(window);
        glClearColor(SPLAT4(clear_color));
        glClear(GL_COLOR_BUFFER_BIT);
      
        basic.use();
        ctx->render();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        
  } // end mainloop 
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}

void render(float t) {
  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

}

void drawCircle(int  numPts, float t, Processing* ctx ){
 vector<vec2> pts{size_t(numPts)};
  float di = 6.28f / numPts;
  int i=0;
  for(auto& p : pts){
    p = vec2{sin(i*di)*.8f, cos(i*di + .2 * t)*.8f};
    i++;
  }
  ctx->polygon(pts,true);
}