#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

#include "project/RenderManager.hpp"

#include "glimac/TrackballCamera.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "project/Controller.hpp"

#include "project/Wall.hpp"
#include "project/GLSLProgram.hpp"

#include "project/Map.hpp"

using namespace glimac;

int main(int argc, char** argv) {
    // Default window size
    glm::vec2 defaultWindowSize = glm::vec2(1280,720);

    // Initialize SDL and open a window
    SDLWindowManager windowManager(defaultWindowSize.x, defaultWindowSize.y, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    // Create Programs (1 fragment shader = 1 program)
    FilePath applicationPath(argv[0]);
    NormalProgram normalProgram(applicationPath);
    ProgramList programList;
    programList.normalProgram = &normalProgram;

    // Enable GPU depth test for 3D rendering
    glEnable(GL_DEPTH_TEST);

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    Map map;
    map.setFileMap("/home/david/PROGRAMMATION/OpenGL3/IMACMAN/Code/data/mapTest.txt");
    map.load();
    //m.display();
    //m.play();

    // Game Infos
    glm::vec2 gameSize = glm::vec2(30,30);

    //TrackballCamera tbCamera = TrackballCamera(30,0,0.0f,1.57f);    // CAMERA VUE 2D
    TrackballCamera tbCamera = TrackballCamera(30,0,0.0f,1.0f);
    RenderManager renderManager = RenderManager(&windowManager, &tbCamera, &programList, gameSize);
    Controller controller = Controller(&windowManager);

    Wall wall1(0,0,1,1);
    Wall wall2(30,0,1,1);
    Wall wall3(0,30,1,1);
    Wall wall4(30,30,1,1);

    // Enable program
    renderManager.useProgram(NORMAL);

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT)
            {
                done = true; // Leave the loop after this iteration
            }
            
            // Update controller with key & mouse events each frame
            controller.updateController();
        }

        // Send the keys to the camera and the map
        tbCamera.cameraController(&controller);
        map.play(&controller);

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // On update la ViewMatrix à chaque tour de boucle
        renderManager.updateMVMatrix(&tbCamera);
        glm::mat4 viewMatrix = renderManager.getMVMatrix();

        // Matrix to do the transformations
        glm::mat4 transformationMatrix;

        // SPHERE
        // Bind Sphere VAO
        renderManager.bindSphereVAO();

        // On fait les transformations
        //transformationMatrix = renderManager.transformMatrix(&sphere);
        // On applique les transformations
        //renderManager.applyTransformations(NORMAL, viewMatrix);
        // We draw
        //renderManager.getSpherePtr()->drawSphere();

        renderManager.drawPacman(map.getPacman());

        // De-bind Sphere VAO
        renderManager.debindVAO();

        // WALL TEST
        renderManager.bindCubeVAO();
        
        // // Wall 1
        // transformationMatrix = renderManager.transformMatrix(&wall1);
        // renderManager.applyTransformations(NORMAL, transformationMatrix);
        // renderManager.getCubePtr()->drawCube();

        // // Wall 2
        // transformationMatrix = renderManager.transformMatrix(&wall2);
        // renderManager.applyTransformations(NORMAL, transformationMatrix);
        // renderManager.getCubePtr()->drawCube();

        // // Wall 3
        // transformationMatrix = renderManager.transformMatrix(&wall3);
        // renderManager.applyTransformations(NORMAL, transformationMatrix);
        // renderManager.getCubePtr()->drawCube();

        // // Wall 4
        // transformationMatrix = renderManager.transformMatrix(&wall4);
        // renderManager.applyTransformations(NORMAL, transformationMatrix);
        // renderManager.getCubePtr()->drawCube();


        //renderManager.drawWall(map.getStaticObjects());

        renderManager.debindVAO();

        // Update the display
        windowManager.swapBuffers();
    }

    // FREE RESSOURCES
    // See ~renderManager destructor

    return EXIT_SUCCESS;
}