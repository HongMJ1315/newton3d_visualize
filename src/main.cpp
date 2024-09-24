#include<iostream>
#include<algorithm>
#include<vector>
#include<utility>
#include<glm/glm.hpp>

#include<iomanip>

#include<fstream>
#include"newton2d.h"
#include"visualize.h"
#include"glsl.h"
#include "GLinclude.h"

// ImGui 头文件
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// f(x, y) = 1/9 * x^2 + 1/4 * y^2 - 1
// g(x, y) = x^2 - y - 4
// key: 'x', 'y', 'c'（c for constant）
// value: vector of terms
Polynomial f = {
    {'x', {{1.0/9, 2}}},
    {'y', {{1.0/4, 2}}},
    {'c', {{-1, 0}}}
};
Polynomial g = {
    {'x', {{1, 2}}},
    {'y', {{-1, 1}}},
    {'c', {{-4, 0}}}
};
// Polynomial f = {
//     {'x', {{1.0, 2}}},
//     {'y', {{-5, 1}}},
//     {'c', {{-6, 0}}}
// };
// Polynomial g = {
//     {'x', {{3, 1}}},
//     {'y', {{-1, 2}}},
//     {'c', {{-8, 0}}}
// };


int width = 800;
int height = 600;

void reshape(GLFWwindow *window, int w, int h){
    width = w;
    height = h;
}
int main(int argc, char **argv){
    glutInit(&argc, argv);
    if(!glfwInit()){
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(width, height, "Hw1", nullptr, nullptr);
    if(!window){
        glfwTerminate();
        return -1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, reshape);
    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "GLSL version: " << glslVersion << std::endl;
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;
    
    auto vao = set_vao();
    auto pointVAOVBO = set_point_vao();
    unsigned int pointVao = pointVAOVBO.first;
    unsigned int pointVbo = pointVAOVBO.second;
    auto func_shader = setGLSLshaders("shader/func_vert.glsl", "shader/func_frag.glsl");
    auto point_shader = setGLSLshaders("shader/point_vert.glsl", "shader/point_frag.glsl");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    static char x_input[64] = "0.001";
    static char y_input[64] = "1";
/*
    std::vector<glm::vec2> pointList[9][7];
    for(int i = -4; i < 5; i++){
        for(int j = -3; j < 4; j++){
            Vector2d start = {i, j};
            // if(i == 0) start = {0.0001, j};
            auto newtonResult = newton2d(f, g, start);
            std::cout<< "(" << i << ", " << j << ") => " << newtonResult.size() << " steps" << std::endl; 
            for(auto k:newtonResult){
                std::cout << std::fixed << std::setprecision(7) << "( " << k[0] << ", " << k[1]  << " )"<< std::endl;        
                pointList[i + 4][j + 3].push_back({k[0], k[1]});
            }
            std::cout << "====================" << std::endl;
        }
    }
*/
    int showPoint = 1;

    int nxt = 0;

    // get now time
    float lastTime = glfwGetTime();
    std::vector<glm::vec2> pointList;
    std::string res;

    while(!glfwWindowShouldClose(window)){
        float nowTime = glfwGetTime();

/*
        int pointListI = nxt / 7, pointListJ = nxt % 7;
        if(nowTime - lastTime > 0.05){
            if(showPoint < pointList[pointListI][pointListJ].size()) {
                showPoint++;
                
            }
            else{
                if(nxt < 62){
                    nxt++, showPoint = 1;
                    pointListI = nxt / 7, pointListJ = nxt % 7;
                }
                else{
                    nxt= 0;
                    for(int i = -4; i < 5; i++){
                        for(int j = -3; j < 4; j++){
                            pointList[i + 4][j + 3].clear();
                            Vector2d start = {i, j};
                            if(i == 0) start = {0.0001, j};
                            auto newtonResult = newton2d(f, g, start);
                            std::cout<< "(" << i << ", " << j << ") => " << newtonResult.size() << " steps" << std::endl; 
                            for(auto k:newtonResult){
                                pointList[i + 4][j + 3].push_back({k[0], k[1]});
                            }
                            std::cout << "====================" << std::endl;
                        }
                    }

                }
            }
            lastTime = nowTime;
        }
*/        

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Input Window"); 

            ImGui::InputText("X", x_input, IM_ARRAYSIZE(x_input));
            ImGui::InputText("Y", y_input, IM_ARRAYSIZE(y_input));

            float x = 0, y = 0;


            if (ImGui::Button("Run")) {
                std::string x_value(x_input);
                std::string y_value(y_input);

                x = std::stod(x_value), y = std::stof(y_value);
                showPoint = 0;
                Vector2d start = {x, y};
                auto newtonResult = newton2d(f, g, start);
                pointList.clear();
                for(auto i:newtonResult)
                    pointList.push_back({i[0], i[1]});
            }


            ImGui::Text("(%lf, %lf) => step: %d", x, y, (int)pointList.size());
            for(int i = 0; i < pointList.size(); i++){
                ImGui::Text("(%lf, %lf)", pointList[i].x, pointList[i].y);

            }

            ImGui::End();
        }

        if(nowTime - lastTime > 1){
            if(showPoint < pointList.size()) {
                lastTime = nowTime;
                showPoint++;
            }  
        }


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(func_shader);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(point_shader);

        glBindVertexArray(pointVao);

        // for(int i = 0; i < 63; i++){
        //     int pointListI = i / 7, pointListJ = i % 7;

        //     std::vector<Vertex> vertices = update_vbo(pointVbo, pointList[pointListI][pointListJ], -1); 
        //     glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

        //     glPointSize(8.0f);
        //     glDrawArrays(GL_POINTS, 0, vertices.size());
        // }
        if(pointList.size() != 0){
            std::vector<Vertex> vertices = update_vbo(pointVbo, pointList, showPoint); 

            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

            glPointSize(8.0f);
            glDrawArrays(GL_POINTS, 0, vertices.size());

            glBindVertexArray(0);
        }
        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}