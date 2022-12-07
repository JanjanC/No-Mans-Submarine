//add glad and glfw libaries
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

//define tinyobjloader implementation
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//define stbimage implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//libraries to load shaders
#include <string>
#include <iostream>

//glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//width and height of the window
#define WIDTH 600.0f
#define HEIGHT 600.0f

/* Model Class */
#include "Classes/Models/Model.h"
#include "Classes/Models/Player.h"
#include "Classes/Models/Skybox.h"

/* Camera Classes */
#include "Classes/Cameras/PerspectiveCamera.h"
#include "Classes/Cameras/OrthoCamera.h"

/* Light Classes */
#include "Classes/Light/DirectionalLight.h"
#include "Classes/Light/SpotLight.h"

class Environment {

public:
    Player* mainModel;
    Model* otherModel;
    Skybox *skybox;
    SpotLight* spotLight;
    DirectionalLight* directionalLight;
    Shader* mainShader;
    Shader* skyboxShader;
    PerspectiveCamera* thirdPerspectiveCamera;
    PerspectiveCamera* firstPerspectiveCamera;
    OrthoCamera* orthoCamera;
    MyCamera* activeCamera;
    int cameraIndex = 0;

    //constructor for the environment class which initializes the objects necessary to render the program such as the models, lights, shaders, and cameras
    Environment() {

        //load the shader for the main object
        mainShader = new Shader("Shaders/player.vert", "Shaders/player.frag");

        //load the shader for the skybox
        skyboxShader = new Shader("Shaders/skybox.vert", "Shaders/skybox.frag");

        //load the main model and its textures
        //3D model taken from Free3D.com by user printable_models (link to creation: https://free3d.com/3d-model/bird-v1--875504.html)
        mainModel = new Player("3D/bird.obj", glm::vec3(0, 0, 0), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
        mainModel->loadTexture("3D/bird.jpg", *mainShader, "tex0");
        mainModel->loadTexture("3D/bird_normal.jpg", *mainShader, "norm_tex");

        otherModel = new Model("3D/bird.obj", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
        otherModel->loadTexture("3D/ayaya.png", *mainShader, "tex0");
        //otherModel->loadTexture("3D/bird_normal.jpg", *mainShader, "norm_tex");

        skybox = new Skybox("Skybox/rainbow_rt.png", "Skybox/rainbow_lf.png", "Skybox/rainbow_up.png", "Skybox/rainbow_dn.png", "Skybox/rainbow_ft.png", "Skybox/rainbow_bk.png");

        //create a point light and load the sphere object
        //3D model for sphere taken from the MIT website (http://web.mit.edu/djwendel/www/weblogo/shapes/basic-shapes/sphere/sphere.obj)
        spotLight = new SpotLight(0.5, 1.0f, 16.0f, glm::vec3(1, 1, 1), 1.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), 12.5);

        //create a directional light with a position of (4, 11, -3)
        directionalLight = new DirectionalLight(0.5, 1.0f, 16.0f, glm::vec3(1, 1, 1), 1.0f, glm::vec3(0, -1, 0));

        //create a third person perspective camera
        thirdPerspectiveCamera = new PerspectiveCamera(mainModel->position - 5.0f * mainModel->direction, mainModel->position, glm::vec3(0, 1.0f, 0));

        //create a first person perspective camera
        firstPerspectiveCamera = new PerspectiveCamera(mainModel->position, mainModel->position + 5.0f * mainModel->direction, glm::vec3(0, 1.0f, 0));

        //create a orthographic camera
        orthoCamera = new OrthoCamera(glm::vec3(0, 10.0f, 1.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1.0f, 0));

        activeCamera = thirdPerspectiveCamera;

        /* print the initial info of the submarine */
        std::cout << "Submarine system initialization... COMPLETE\n";
        std::cout << "Preparing for underwater exploration...\n\n";
        std::cout << "[SUBMARINE STATUS]\n";
        std::cout << "Current ocean depth: " << mainModel->position.y;
    }
    
    //destructor for the environment class
    ~Environment() {
        //deallocates the objects from the memory
        delete mainModel;
        delete spotLight;
        delete directionalLight;
        delete mainShader;
        delete thirdPerspectiveCamera;
        delete orthoCamera;
    }

    //updates the uniform values of the shader files and draws the objects on the screen
    void update() {

        thirdPerspectiveCamera->updateFields(mainModel->position - 5.0f * mainModel->direction, mainModel->position);
        firstPerspectiveCamera->updateFields(mainModel->position, mainModel->position + 5.0f * mainModel->direction);

        //updates the uniform values
        activeCamera->setViewMatrix(*mainShader);
        activeCamera->setProjectionMatrix(*mainShader);
        activeCamera->setCameraPosition(*mainShader);

        spotLight->updateFields(mainModel->position, mainModel->direction);
        spotLight->setAmbientStr(*mainShader);
        spotLight->setSpecStr(*mainShader);
        spotLight->setSpecPhong(*mainShader);
        spotLight->setLightColor(*mainShader);
        spotLight->setLightIntensity(*mainShader);
        spotLight->setLightPosition(*mainShader);
        spotLight->setLightDirection(*mainShader);
        spotLight->setAttenuationConstants(*mainShader);
        spotLight->setCutoff(*mainShader);

        directionalLight->setAmbientStr(*mainShader);
        directionalLight->setSpecStr(*mainShader);
        directionalLight->setSpecPhong(*mainShader);
        directionalLight->setLightColor(*mainShader);
        directionalLight->setLightIntensity(*mainShader);
        directionalLight->setLightDirection(*mainShader);

        skybox->setViewMatrix(*skyboxShader, activeCamera->viewMatrix);
        skybox->setProjectionMatrix(*skyboxShader, activeCamera->projectionMatrix);

        //draws the objects on the screens
        mainModel->draw(*mainShader);
        otherModel->draw(*mainShader);
        skybox->draw(*skyboxShader);
    }
};

//----------GLOBAL VARIABLES----------
Environment* environment; //pointer to the environment object

//----------CALLBACK FUNCTIONS----------
//callback function for key presses
void Key_Callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

    //model rotation and light movement
    /*
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {

        //rotate the point light when the point light is selected
        if (!environment->spotLight->isActive) {
            environment->mainModel->processKeyboard(key);
        }

        //rotate the main model when the point light is not selected
        if (environment->spotLight->isActive) {
            environment->spotLight->processKeyboard(key);
        }
    }
    */

    /* Player Control Movement */
    if ((key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_REPEAT) {
        /* Insert a flag for the camera being used e.g. camera is 1st person or 3rd person */
        if (environment->activeCamera == environment->thirdPerspectiveCamera || environment->activeCamera == environment->firstPerspectiveCamera) {
            environment->mainModel->processKeyboard(key);
        }
    }

    //select and deselect the point light
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        environment->spotLight->processKeyboard(key);
    }

    //increase and decrease the light intensity
    if ((key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        environment->spotLight->processKeyboard(key);
    }

    //change the camera view
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        printf("im in");
        if (environment->activeCamera == environment->firstPerspectiveCamera) {
            printf("first");
            environment->activeCamera = environment->thirdPerspectiveCamera;
        } 
        else
        if (environment->activeCamera == environment->thirdPerspectiveCamera) {
            printf("third");
            environment->activeCamera = environment->firstPerspectiveCamera;
        }
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        environment->activeCamera = environment->orthoCamera;
    }

    /* Escaping the game */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

//callback function for cursor movement
void Mouse_Callback(GLFWwindow* window, double xPos, double yPos) {
    if (environment->activeCamera == environment->thirdPerspectiveCamera) {
        environment->thirdPerspectiveCamera->processMouse(xPos, yPos);
    }
}

int main(void)
{
    GLFWwindow* window;

    //initialize the library
    if (!glfwInit())
        return -1;

    //create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "Programming Challenge 2 - John Vincent Chua", NULL, NULL);

    //terminate the program if a window is not created
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //make the window's context current
    glfwMakeContextCurrent(window);

    //load the glad library
    gladLoadGL();

    //set callbacks for key presses and cursor movement
    glfwSetKeyCallback(window, Key_Callback);
    //glfwSetCursorPosCallback(window, Mouse_Callback);

    //prevent the mouse from going out the window
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //create an environment object which stores the models, lights, shaders, and cameras
    environment = new Environment();

    //set the size of the viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    //loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update the uniform values in the shaders and draw the object on the screen
        environment->update();

        //swap front and back buffers
        glfwSwapBuffers(window);

        //poll for and process events
        glfwPollEvents();
    }

    delete environment; //deallocate the memory for environment

    glfwTerminate();

    return 0;
}
