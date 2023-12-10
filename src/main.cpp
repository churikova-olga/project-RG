#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void renderTerrain();

unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
float heightScale = 0.1;
bool isLamp = true;
bool isLampPress = false;
// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    float constant;
    float linear;
    float quadratic;
};


struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
//    glm::vec3 backpackPosition = glm::vec3(0.0f);

    glm::vec3 housePosition = glm::vec3(14.0f, -0.5f, -21.0f);
    float houseRotation = -90.0f;
    float houseScale = 80.0f;

    glm::vec3 fencePosition = glm::vec3(-2.0f, -0.5f, -1.0f);
    float fenceRotation = 180.0f;
    float fenceScale = 0.07f;

    glm::vec3 scarecrowPosition = glm::vec3(0.0f, -0.5f, -2.0f);
    float scarecrowRotation = 180.0f;
    float scarecrowScale = 40.0f;

    glm::vec3 lampPosition = glm::vec3(12.0f, -0.5f, -11.0f);
    float lampRotation = 140.0f;
    float lampScale = 100.0f;


    //cactus positions
    vector<glm::vec3> cactus1
            {
                    glm::vec3(-12.5f, 0.0f, -8.48f),
                    glm::vec3(13.5f, 0.0f, -7.51f),
                    glm::vec3(0.0f, 0.0f, -10.7f),
                    glm::vec3(8.7f, 0.0f, -26.3f),
                    glm::vec3(-10.5f, 0.0f, -0.6f)
    };


    float cactus1Rotation[5] = {10, 189, 69, 234, 48};
    float cactus1Scale = 2.0f;

    vector<glm::vec3> cactus2
            {
                    glm::vec3(2.5f, 0.0f, 6.48f),
                    glm::vec3(15.5f, 0.0f, -3.51f),
                    glm::vec3(8.0f, 0.0f, 2.7f),
                    glm::vec3(12.3f, 0.0f, -24.3f),
                    glm::vec3(24.5f, 0.0f, -13.6f)
            };


    float cactus2Rotation[5] = {13, 45, 69, 234, 45};
    float cactus2Scale = 2.0f;

    glm::vec3 roaPosition = glm::vec3(-6.5f, 0.0f, -12.0f);
    float roadRotation = 0.0f;
    float roadScale = 5.0f;
    float backpackScale = 1.0f;

    PointLight pointLight;
    DirLight dirLight;
    SpotLight spotLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 1.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // enabling back face culling
    // -----------------------------
    glEnable(GL_CULL_FACE);


    // build and compile shaders
    // -------------------------
    Shader LightShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader terrainShader("resources/shaders/terrain.vs", "resources/shaders/terrain.fs");
    Shader grassShader("resources/shaders/grass.vs", "resources/shaders/grass.fs");


    //load models
    Model houseModel("resources/objects/house/source/house.obj");
    houseModel.SetShaderTextureNamePrefix("material.");

    Model fenceModel("resources/objects/fence/fence.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    Model scarecrowModel("resources/objects/scarecrow/scarecrow.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    Model lampModel("resources/objects/lamp/source/lamp.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    Model cactusModel1("resources/objects/cactus/source/cactus1.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    Model cactusModel2("resources/objects/cactus/source/cactus2.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    Model roadModel("resources/objects/road/source/road.obj");
    fenceModel.SetShaderTextureNamePrefix("material.");

    //positions skybox
    //--------
    float skyboxVertices[] = {
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

    };

    //positions transparent
    //--------
    float transparentVertices[] = {
            // coordinate       // texture
            0.0f, 2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

            0.0f, 2.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
            1.0f, 2.0f, 0.0f, 1.0f, 0.0f,
    };



    //Dir light
    DirLight& dirLight = programState->dirLight;
    dirLight.direction = glm::vec3(0.2f, 120.0f, 25.0f);
    dirLight.ambient =   glm::vec3(.0f, 0.0f, 0.0f);
    dirLight.diffuse =   glm::vec3( 0.2f, 0.2f, 0.7f);
    dirLight.specular =  glm::vec3(0.7f, 0.7f, 0.7f);

    //Point light
    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(10.0f, 4.0, -12.0);
    pointLight.ambient = glm::vec3(1.5, 1.5, 1.5);
    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    //Spotlight
    SpotLight& spotLight = programState->spotLight;
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;


    // grass positioning
    int amount = 100;
    glm::vec3 vegetations[amount];
    srand(glfwGetTime());
    glm::vec3 vegetation;
    vegetation.y = 0.0f;
    for(int i = 0; i < amount; i++){
        vegetation.x = float(rand() % 15) - 7;
        vegetation.z = float(rand() % 14) - 8;
        vegetations[i] = vegetation;
    }


    //VAO grass
    //---------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*amount, &vegetations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER ,0);

    //VAO skybox
    //---------
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    // VAO transparent
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2,3, GL_FLOAT,GL_FALSE, 3*sizeof (float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glVertexAttribDivisor(2, 1);

    //load texture skybox
    vector<std::string> faces{
        FileSystem::getPath("resources/textures/skybox/left.jpeg"),
        FileSystem::getPath("resources/textures/skybox/right.jpeg"),
        FileSystem::getPath("resources/textures/skybox/top.jpeg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpeg"),
        FileSystem::getPath("resources/textures/skybox/back.jpeg"),
        FileSystem::getPath("resources/textures/skybox/front.jpeg"),

    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //load texture
    unsigned int terrainDiffuse = loadTexture("resources/textures/terrain/terrain_diffuse.jpg");
    unsigned int terrainNormal = loadTexture( "resources/textures/terrain/terrain_normal.jpg");
    unsigned int terrainHeight = loadTexture( "resources/textures/terrain/terrain_height.jpg");

    stbi_set_flip_vertically_on_load(false);
    unsigned int transparentTexture = loadTexture("resources/textures/grassDry.png");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    LightShader.use();
    LightShader.setInt("material.diffuse", 0);


    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    //shaderTransparent
    grassShader.use();
    grassShader.setInt("grass", 0);

    terrainShader.use();
    terrainShader.setInt("terrainDiffuse", 0);
    terrainShader.setInt("terrainNormal", 1);
    terrainShader.setInt("terrainHeight", 2);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        LightShader.use();
        LightShader.setBool("isLamp", isLamp);
        // Directional light
        LightShader.setVec3("dirLight.direction", dirLight.direction);
        LightShader.setVec3("dirLight.ambient", dirLight.ambient);
        LightShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        LightShader.setVec3("dirLight.specular", dirLight.specular);

        // Point light
        LightShader.setVec3("pointLight.position", pointLight.position);
        LightShader.setVec3("pointLight.ambient", pointLight.ambient);
        LightShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        LightShader.setVec3("pointLight.specular", pointLight.specular);
        LightShader.setFloat("pointLight.constant", pointLight.constant);
        LightShader.setFloat("pointLight.linear", pointLight.linear);
        LightShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        // Spotlight
        LightShader.setVec3("spotLight.position", programState->camera.Position);
        LightShader.setVec3("spotLight.direction", programState->camera.Front);
        LightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        LightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        LightShader.setVec3("spotLight.ambient", 3.0f, 3.0f, 3.0f);
        LightShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        LightShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        LightShader.setFloat("spotLight.constant", spotLight.constant);
        LightShader.setFloat("spotLight.linear", spotLight.linear);
        LightShader.setFloat("spotLight.quadratic", spotLight.quadratic);


        LightShader.setVec3("viewPos", programState->camera.Position);
        LightShader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        LightShader.setMat4("projection", projection);
        LightShader.setMat4("view", view);

        // render the loaded model
        //house
        model = glm::translate(model,
                               programState->housePosition);
        model = glm::rotate(model, glm::radians(programState->houseRotation), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(programState->houseScale));
        LightShader.setMat4("model", model);
        houseModel.Draw(LightShader);

        //fence
        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->fencePosition);
        model = glm::rotate(model, glm::radians(programState->fenceRotation), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(programState->fenceScale));
        LightShader.setMat4("model", model);
        fenceModel.Draw(LightShader);


        //scarecrow
        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->scarecrowPosition);
        model = glm::rotate(model, glm::radians(programState->scarecrowRotation), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(programState->scarecrowScale));
        LightShader.setMat4("model", model);
        scarecrowModel.Draw(LightShader);

        //lamp
        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->lampPosition);
        model = glm::rotate(model, glm::radians(programState->lampRotation), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(programState->lampScale));
        LightShader.setMat4("model", model);
        lampModel.Draw(LightShader);


        //cactus
        for(int i = 0; i < programState->cactus1.size(); i++ ){
            model = glm::mat4(1.0f);
            model = glm::translate(model,
                                   programState->cactus1[i]);
            model = glm::rotate(model, glm::radians(programState->cactus1Rotation[i]), glm::vec3(0,1,0));
            model = glm::scale(model, glm::vec3(programState->cactus1Scale));
            LightShader.setMat4("model", model);
            cactusModel1.Draw(LightShader);
        }

        for(int i = 0; i < programState->cactus2.size(); i++ ){
            model = glm::mat4(1.0f);
            model = glm::translate(model,
                                   programState->cactus2[i]);
            model = glm::rotate(model, glm::radians(programState->cactus2Rotation[i]), glm::vec3(0,1,0));
            model = glm::scale(model, glm::vec3(programState->cactus2Scale));
            LightShader.setMat4("model", model);
            cactusModel2.Draw(LightShader);
        }

        //road
        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               programState->roaPosition);
        model = glm::rotate(model, glm::radians(programState->roadRotation), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(programState->roadScale));
        LightShader.setMat4("model", model);
        roadModel.Draw(LightShader);

        //render terrain
        terrainShader.use();

        // Directional light
        terrainShader.setVec3("dirLight.direction", dirLight.direction);
        terrainShader.setVec3("dirLight.ambient", dirLight.ambient);
        terrainShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        terrainShader.setVec3("dirLight.specular", dirLight.specular);

        // Point light
        terrainShader.setVec3("pointLight.position", pointLight.position);
        terrainShader.setVec3("pointLight.ambient", pointLight.ambient);
        terrainShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        terrainShader.setVec3("pointLight.specular", pointLight.specular);
        terrainShader.setFloat("pointLight.constant", pointLight.constant);
        terrainShader.setFloat("pointLight.linear", pointLight.linear);
        terrainShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        // Spotlight
        terrainShader.setVec3("spotLight.position", programState->camera.Position);
        terrainShader.setVec3("spotLight.direction", programState->camera.Front);
        terrainShader.setVec3("spotLight.ambient", 4.0f, 4.0f, 4.0f);
        terrainShader.setVec3("spotLight.diffuse", 2.0f, 2.0f, 2.0f);
        terrainShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("spotLight.constant", 1.0f);
        terrainShader.setFloat("spotLight.linear", 0.09f);
        terrainShader.setFloat("spotLight.quadratic", 0.032f);
        terrainShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        terrainShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        terrainShader.setVec3("viewPos", programState->camera.Position);



        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        terrainShader.setMat4("model", model);
        terrainShader.setFloat("heightScale", heightScale);
        terrainShader.setBool("isLamp", isLamp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, terrainNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, terrainHeight);
        renderTerrain();


        glDisable(GL_CULL_FACE);
        //render transparent
        grassShader.use();

        // Directional light
        grassShader.setVec3("dirLight.direction", dirLight.direction);
        grassShader.setVec3("dirLight.ambient", dirLight.ambient);
        grassShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        grassShader.setVec3("dirLight.specular", dirLight.specular);

        grassShader.setMat4("view", view);
        grassShader.setMat4("projection", projection);
        grassShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
        glBindVertexArray(0);


        //render skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        if (programState->ImGuiEnabled)
            DrawImGui(programState);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &transparentVAO);
    glDeleteBuffers(1, &transparentVBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !isLampPress){
        isLamp = !isLamp;
        isLampPress = true;
    };

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE){
        isLampPress = false;
    };
}

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

//Normal mapping for terrain
unsigned int terrainVAO = 0;
unsigned int terrainVBO;
void renderTerrain(){

    if (terrainVAO == 0)
    {

        // positions
        glm::vec3 pos1(60.0f,  0.0f, 60.0f);
        glm::vec3 pos2(60.0f, 0.0f, -60.0f);
        glm::vec3 pos3( -60.0f, 0.0f, -60.0f);
        glm::vec3 pos4( -60.0f,  0.0f, 60.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 20.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(20.0f, 0.0f);
        glm::vec2 uv4(20.0f, 20.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float terrainVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };

        // configure terrain VAO
        glGenVertexArrays(1, &terrainVAO);
        glGenBuffers(1, &terrainVBO);
        glBindVertexArray(terrainVAO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), &terrainVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}





unsigned int loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    for(unsigned int i=0; i < faces.size(); i++){
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "Cubemap failed to load at path" << faces[i] << std::endl;
            stbi_image_free(data);
        }

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        ImGui::Begin("settings");
//        ImGui::Text("Hello text");
//        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
//        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
//        ImGui::DragFloat3("Backpack position", (float*)&programState->backpackPosition);
        //ImGui::DragFloat("Backpack scale", &programState->backpackScale, 0.05, 0.1, 4.0);

        ImGui::SliderFloat("pointLight.constant", &programState->pointLight.constant, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
        ImGui::Checkbox("on/off flashlight", &isLamp);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
