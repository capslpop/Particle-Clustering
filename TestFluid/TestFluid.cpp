#include "main.h"

#include "Graphics\Window.h"
#include "Graphics\RenderPass.h"
#include "Graphics\Camera.h"
#include "Graphics\pallet.h"
#include "Graphics\FileHelper.h"


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(2.0, 0.0, 0.0), (float)SCR_WIDTH / (float)SCR_HEIGHT);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    camera.updateAspectRatio((float)width / (float)height);
    glViewport(0, 0, width, height);
}

unsigned int createPallet(int bindingNumber)
{
    // load 3D texture here:
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + bindingNumber);
    glBindTexture(GL_TEXTURE_1D, texture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLint interalformat = GL_RGBA8;
    GLenum format = GL_RGBA;

    glTexImage1D(GL_TEXTURE_1D, 0, interalformat, 256, 0, format, GL_UNSIGNED_BYTE, pallet);

    return texture;
}

unsigned int create3DTexture(glm::ivec3 size, int bindingNumber, GLint interalformat, GLenum format)
{
    // load 3D texture here:
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + bindingNumber);
    glBindTexture(GL_TEXTURE_3D, texture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // for regular use
    glTexImage3D(GL_TEXTURE_3D, 0, interalformat, size.x, size.y, size.z, 0, format, GL_UNSIGNED_BYTE, nullptr);

    // for compute shader use
    glBindImageTexture(0 + bindingNumber, texture, 0, GL_FALSE, 0, GL_READ_WRITE, interalformat);

    return texture;
}

int main()
{
    // for networking because I dont need crazy performance I will use
    // LUA to do that because I just need to send a few small packets
    // every so often -- also this will help with modding so it will be
    // a lot easier

    glfwInit();

    Window window(framebuffer_size_callback, SCR_WIDTH, SCR_HEIGHT);

    RenderPass renderPass;
    renderPass.addVertexShader(readFile("Shaders/main.vert").c_str());
    renderPass.addFragmentShader(readFile("Shaders/main.frag").c_str());
    renderPass.linkShaders();

    RenderPass computeFluid;
    computeFluid.addComputeShader(readFile("Shaders/fluid.comp").c_str());
    computeFluid.linkShaders();

    int boundTextures = 0;

    int sizeOf3Dtex = 256;

    // also make sure that the shader knows where this texture is:
    renderPass.use();

    int palletBinding = boundTextures;
    createPallet(boundTextures++);

    int fluidParticlesAtex = boundTextures;
    create3DTexture(glm::ivec3(sizeOf3Dtex), boundTextures++, GL_RGBA32F, GL_RGBA);
    
    int fluidParticlesBtex = boundTextures;
    create3DTexture(glm::ivec3(sizeOf3Dtex), boundTextures++, GL_RGBA32F, GL_RGBA);

    int fluidFlowAtex = boundTextures;
    create3DTexture(glm::ivec3(sizeOf3Dtex), boundTextures++, GL_RGBA8, GL_RGBA);

    int fluidFlowBtex = boundTextures;
    create3DTexture(glm::ivec3(sizeOf3Dtex), boundTextures++, GL_RGBA8, GL_RGBA);

    int fluidDummytex = boundTextures;
    create3DTexture(glm::ivec3(sizeOf3Dtex), boundTextures++, GL_R32UI, GL_RED);

    int fluidParticlesA = glGetUniformLocation(renderPass.shaderProgram, "fluidParticlesA");
    glUniform1i(fluidParticlesA, fluidParticlesAtex);

    int fluidParticlesB = glGetUniformLocation(renderPass.shaderProgram, "fluidParticlesB");
    glUniform1i(fluidParticlesB, fluidParticlesBtex);

    int fluidFlowA = glGetUniformLocation(renderPass.shaderProgram, "fluidFlowA");
    glUniform1i(fluidFlowA, fluidFlowAtex);

    int fluidFlowB = glGetUniformLocation(renderPass.shaderProgram, "fluidFlowB");
    glUniform1i(fluidFlowB, fluidFlowBtex);

    glUniform1i(glGetUniformLocation(renderPass.shaderProgram, "pallet"), palletBinding);

    int camMat = glGetUniformLocation(renderPass.shaderProgram, "camMat"); 
    int camPos = glGetUniformLocation(renderPass.shaderProgram, "camPos");

    // set up compute varables
    computeFluid.use();
    int fluidParticlesAComp = glGetUniformLocation(computeFluid.shaderProgram, "fluidParticlesA");
    glUniform1i(fluidParticlesAComp, fluidParticlesAtex);

    int fluidParticlesBComp = glGetUniformLocation(computeFluid.shaderProgram, "fluidParticlesB");
    glUniform1i(fluidParticlesBComp, fluidParticlesBtex);

    int fluidFlowAComp = glGetUniformLocation(computeFluid.shaderProgram, "fluidFlowA");
    glUniform1i(fluidFlowAComp, fluidFlowAtex);

    int fluidFlowBComp = glGetUniformLocation(computeFluid.shaderProgram, "fluidFlowB");
    glUniform1i(fluidFlowBComp, fluidFlowBtex);

    int fluidDummyComp = glGetUniformLocation(computeFluid.shaderProgram, "fluidDummy");
    glUniform1i(fluidDummyComp, fluidFlowBtex);

    renderPass.use();


    // you could just render the from sides of the cube and then rotate them twards the camera
    // this way you have half of the work to do
    // just use 1
    float off = 1.0f;
    // Vertices of a cube for rendering with triangles (36 vertices total for 12 triangles)
    GLfloat vertices[] = {
        // Front face
        // Triangle 1
        0.0f, 0.0f,  off,  // Bottom left
         off, 0.0f,  off,  // Bottom right
         off,  off,  off,  // Top right
         // Triangle 2
         0.0f, 0.0f,  off,  // Bottom left
          off,  off,  off,  // Top right
         0.0f,  off,  off,  // Top left

         // Right face
         // Triangle 3
         off, 0.0f,  off,  // Bottom front
         off, 0.0f, 0.0f,  // Bottom back
         off,  off, 0.0f,  // Top back
         // Triangle 4
         off, 0.0f,  off,  // Bottom front
         off,  off, 0.0f,  // Top back
         off,  off,  off,  // Top front

         // Back face
         // Triangle 5
         off, 0.0f, 0.0f,  // Bottom right
         0.0f, 0.0f, 0.0f,  // Bottom left
         0.0f,  off, 0.0f,  // Top left
         // Triangle 6
         off, 0.0f, 0.0f,  // Bottom right
         0.0f,  off, 0.0f,  // Top left
         off,  off, 0.0f,  // Top right

         // Left face
         // Triangle 7
         0.0f, 0.0f, 0.0f,  // Bottom back
         0.0f, 0.0f,  off,  // Bottom front
         0.0f,  off,  off,  // Top front
         // Triangle 8
         0.0f, 0.0f, 0.0f,  // Bottom back
         0.0f,  off,  off,  // Top front
         0.0f,  off, 0.0f,  // Top back

         // Bottom face
         // Triangle 9
         0.0f, 0.0f, 0.0f,  // Back left
         off, 0.0f, 0.0f,  // Back right
         off, 0.0f,  off,  // Front right
         // Triangle 10
         0.0f, 0.0f, 0.0f,  // Back left
         off, 0.0f,  off,  // Front right
         0.0f, 0.0f,  off,  // Front left

         // Top face
         // Triangle 11
         0.0f,  off,  off,  // Front left
         off,  off,  off,  // Front right
         off,  off, 0.0f,  // Back right
         // Triangle 12
         0.0f,  off,  off,  // Front left
          off,  off, 0.0f,  // Back right
         0.0f,  off, 0.0f   // Back left
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glm::vec2 lastMousePos = glm::vec2(0.0);

    double previosTime = 0.0;
    double currentTime = glfwGetTime();
    double deltaTime = 0.0;

    // sensativity -- make options for this in the menu
    float senMovement = 1.0f;
    float senMouse = 0.01f;

    bool evenFrame = false;

    int frameCount = 0;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // TODO: remeber to cull data
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable face culling and specify to cull back faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // don't need
    glDisable(GL_MULTISAMPLE);

    // Render Loop
    while (!window.isClosed())
    {
        frameCount++;

        if (evenFrame)
        {
            evenFrame = false;
        }
        else
        {
            evenFrame = true;
        }

        glCheckError();

        // exit out of cursor mode
        if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetInputMode(window.window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
        // enter back into cursor mode:
        if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetWindowAttrib(window.window, GLFW_HOVERED))
        {
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        // get delta time
        previosTime = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - previosTime;

        // update Camera with 
        double xpos, ypos;
        glfwGetCursorPos(window.window, &xpos, &ypos);
        glm::vec2 changeInMousePos = lastMousePos - glm::vec2(xpos, ypos);
        camera.rotate(changeInMousePos * senMouse); // no need to multiply by delta time here
        lastMousePos = glm::vec2(xpos, ypos);

        // update camera with key inputs
        glm::vec3 cameraDirection = camera.getDirection();
        // forward
        if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS)
        {
            glm::vec2 normCam = glm::normalize(glm::vec2(cameraDirection.x, cameraDirection.z)) * (float)deltaTime;
            camera.position.x += normCam.x * senMovement;
            camera.position.z += normCam.y * senMovement;
        }
        // backwards
        if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS)
        {
            glm::vec2 normCam = glm::normalize(glm::vec2(cameraDirection.x, cameraDirection.z)) * (float)deltaTime;
            camera.position.x -= normCam.x * senMovement;
            camera.position.z -= normCam.y * senMovement;
        }
        // Right
        if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glm::vec2 normCam = glm::normalize(glm::vec2(cameraDirection.x, cameraDirection.z)) * (float)deltaTime;
            camera.position.x -= normCam.y * senMovement;
            camera.position.z += normCam.x * senMovement;
        }
        // Left
        if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS)
        {
            glm::vec2 normCam = glm::normalize(glm::vec2(cameraDirection.x, cameraDirection.z)) * (float)deltaTime;
            camera.position.x += normCam.y * senMovement;
            camera.position.z -= normCam.x * senMovement;
        }
        // up
        if (glfwGetKey(window.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            camera.position.y += senMovement * (float)deltaTime;
        }
        // down
        if (glfwGetKey(window.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera.position.y -= senMovement * (float)deltaTime;
        }

        glCheckError();

        // main window
        renderPass.use();

        glUniformMatrix4fv(camMat, 1, GL_FALSE, glm::value_ptr(camera.getCameraMat()));
        glUniform3f(camPos, camera.position.x, camera.position.y, camera.position.z);

        glCheckError();

        // set the correct buffer for this frame
        if (evenFrame)
        {
            glUniform1i(fluidParticlesA, fluidParticlesBtex);
            glUniform1i(fluidParticlesB, fluidParticlesAtex);

            glUniform1i(fluidFlowA, fluidFlowBtex);
            glUniform1i(fluidFlowB, fluidFlowAtex);
        }
        else
        {
            glUniform1i(fluidParticlesB, fluidParticlesBtex);
            glUniform1i(fluidParticlesA, fluidParticlesAtex);

            glUniform1i(fluidFlowB, fluidFlowBtex);
            glUniform1i(fluidFlowA, fluidFlowAtex);
        }

        // Now compute the fluid
        computeFluid.use();

        if (evenFrame)
        {
            glUniform1i(fluidParticlesAComp, fluidParticlesBtex); // read
            glUniform1i(fluidParticlesBComp, fluidParticlesAtex); // write

            glUniform1i(fluidFlowAComp, fluidFlowBtex); // read
            glUniform1i(fluidFlowBComp, fluidFlowAtex); // write
        }
        else
        {
            glUniform1i(fluidParticlesBComp, fluidParticlesBtex); // write
            glUniform1i(fluidParticlesAComp, fluidParticlesAtex); // read

            glUniform1i(fluidFlowBComp, fluidFlowBtex); // write
            glUniform1i(fluidFlowAComp, fluidFlowAtex); // read
        }

        glDispatchCompute(sizeOf3Dtex / 4, sizeOf3Dtex / 4, sizeOf3Dtex / 4);

        // TODO: do the correct memory barrior here
        glMemoryBarrier(GL_ALL_BARRIER_BITS); // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT

        renderPass.use();
        glClearColor(0.6, 0.8, 0.9, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1);
        glBindVertexArray(0); // TODO: remove

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::DockSpaceOverViewport();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("FPS");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        // TODO: updatedframe resize call back here

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        window.swapBuffers();
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}