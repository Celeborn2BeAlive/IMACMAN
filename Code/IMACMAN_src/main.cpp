#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <ft2build.h>
#include <GL/gl.h>
#include FT_FREETYPE_H

#include "project/RenderManager.hpp"
#include "project/GameManager.hpp"
#include "project/AudioManager.hpp"
#include "project/Controller.hpp"
#include "glimac/TrackballCamera.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "project/Menu.hpp"
#include "project/UI.hpp"

#include "project/GLSLProgram.hpp"

using namespace glimac;

int main(int argc, char** argv) {

    /* -------------
    *   INIT WINDOW
    *  ------------- */

    // Initialize SDL and open a window
    SDLWindowManager windowManager("MAGMAN");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    // Enable GPU depth test for 3D rendering
    glEnable(GL_DEPTH_TEST);

    /* -------------
    *   INIT PROGRAMS
    *  ------------- */

    // Create Programs (1 fragment shader = 1 program)
    FilePath applicationPath(argv[0]);
    NormalProgram normalProgram(applicationPath);
    TextureProgram textureProgram(applicationPath);
    CubeMapProgram cubemapProgram(applicationPath);
    DirectionnalLightProgram directionnalLightProgram(applicationPath);
    PointLightProgram pointLightProgram(applicationPath);
    BlackAndWhiteProgram bwProgram(applicationPath);

    ProgramList programList;
    programList.normalProgram = &normalProgram;
    programList.textureProgram = &textureProgram;
    programList.cubemapProgram = &cubemapProgram;
    programList.directionnalLightProgram = &directionnalLightProgram;
    programList.pointLightProgram = &pointLightProgram;
    programList.bwProgram = &bwProgram;

    /* --------------------------
    *   INIT GAME and CONTROLLER
    *  -------------------------- */

    Map map;
    map.setFileMap("classicMap.txt");

    GameManager gameManager = GameManager(&map);
    gameManager.load(true);
    map.initialization();
    glm::vec2 gameSize = glm::vec2(gameManager.getMap()->getNbX(),gameManager.getMap()->getNbY());

    Controller controller = Controller(&windowManager);

    /* --------------
    *   INIT CAMERAS
    *  -------------- */

    //TrackballCamera tpsCamera = TrackballCamera(gameSize.x,0,0.0f,0.0f);    // CAMERA VIEW "2D"
    TrackballCamera tpsCamera = TrackballCamera(gameSize.x,0,0.0f,-0.4f);
    FreeflyCamera fpsCamera = FreeflyCamera();
    Camera* camera = &tpsCamera;

    /* ---------------------
    *   INIT RENDER MANAGER
    *  --------------------- */

    RenderManager renderManager = RenderManager(&windowManager, camera, &programList, gameSize);

    // Load Textures
    renderManager.loadTextures();
    // initialize Skybox
    renderManager.initSkybox();
    // initialize Text
    renderManager.loadFont();

    /* ---------------------
    *   INIT AUDIO | START MUSIC
    *  --------------------- */

    AudioManager audioManager = AudioManager();
    audioManager.addMusic(audioManager.createMusic("../Code/assets/audio/mainTheme.mp3"));
    audioManager.addMusic(audioManager.createMusic("../Code/assets/audio/mainThemeFast.mp3"));
    audioManager.fillSounds();
    audioManager.playMusic(0);

    /* ---------------
    *   UI
    * --------------- */
    UI ui = UI(gameManager.getPlayer());

    /* -------------
    *   MENU | PAUSE MENU
    *  ------------- */

    Menu menu = Menu(true);
    Menu menuPause = Menu(false);

    /* ------------------------------------------------------------
    *   MENU APPLICATION LOOP | 1.EVENTS | 2. RENDERING
    *  ------------------------------------------------------------ */

    bool game = true;
    bool play = false;
    while(game)
    {
        // --- EVENTS --- //
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT)
            {
                game = false; // Leave the loop after this iteration
            }
            // Update controller with key events each frame
            controller.updateInterfaceAction(e);
        }
        menu.selectButton(&controller, &audioManager);

        if (controller.getInterfaceAction() == Controller::Key::ENTER)
        {
            if ((menu.getButton() == Menu::Button::PLAY) || (menu.getButton() == Menu::Button::CONTINUE)) // PLAY OR CONTIUE
            {
                play = true;
                game = false;

                audioManager.playMusic(1);  // Change Music

                if (menu.getButton() == Menu::Button::PLAY)
                    gameManager.load(true);
                else
                    gameManager.load(false);

                /* -------------
                *   INIT TIME
                *  ------------- */

                // initialize the timers
                gameManager.setTimers();
                // Time & Delta Time
                windowManager.updateDeltaTime();
                gameManager.setStartTime(SDL_GetTicks());
            }
            else if (menu.getButton() == Menu::Button::EXIT)    // EXIT
                game = false;
        }

        // --- RENDERING --- //

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render the menu
        renderManager.drawMenu(&menu);
        // Update the display
        windowManager.swapBuffers();

        /* -----------------------------------------------------------------
        *   GAME APPLICATION LOOP | 1.EVENTS | 2.GAME ENGINE | 3. RENDERING
        *  ----------------------------------------------------------------- */

        while(play) {
            /* ------------------
            *   UPDATE DELTA TIME
            *  ------------------ */

            windowManager.updateDeltaTime();
            gameManager.updateSpeed(windowManager.getDeltaTime());

            /* ------------------
            *   EVENT LOOP
            *  ------------------ */

            SDL_Event e;
            while(windowManager.pollEvent(e)) {
                if(e.type == SDL_QUIT)
                {
                    play = false; // Leave the loop after this iteration
                }

                // Update controller with key & mouse events each frame
                controller.updateController(gameManager.getMap()->getPacman(), e);
            }

            // If game paused, use menuPause
            if(gameManager.isPause())
            {
                menuPause.selectButton(&controller, &audioManager);

                gameManager.setPauseStartTime(SDL_GetTicks());

                if (controller.getInterfaceAction() == Controller::Key::ENTER)
                {
                    if (menuPause.getButton() == Menu::Button::PLAY) // RESTART
                    {
                        Mix_RewindMusic();
                        gameManager.restart();
                    }
                    else if (menuPause.getButton() == Menu::Button::CONTINUE) // SAVE
                    {
                        audioManager.playSound(8,1);
                        gameManager.save();
                        SDL_Delay(1000);
                    }
                    else if (menuPause.getButton() == Menu::Button::EXIT)    // EXIT
                    {
                        play = false;
                    }
                }
            }


            /* ------------------
            *   MANAGE CAMERAS
            *  ------------------ */

            // Send the keys to the camera and the map
            tpsCamera.cameraController(&controller, windowManager.getDeltaTime());
            // Ask the camera to track pacman
            fpsCamera.setCameraOnCharacter(gameManager.getMap()->getPacman(), gameSize, windowManager.getDeltaTime());
            // Switch Camera FPS / TPS if C button is pressed
            if (controller.getInterfaceAction() == Controller::C)
            {
                if(camera == &fpsCamera)
                {
                    camera = &tpsCamera;
                    controller.setFPS(false);
                }
                else
                {
                    camera = &fpsCamera;
                    controller.setFPS(true);
                }
                controller.setInterfaceAction(Controller::NONE);
            }
            if (gameManager.lost())
            {
                camera = &tpsCamera;
                controller.setFPS(false);
            }

            /* --------------------------------------------------------------------------
            *   PLAY FUNCTION : Move characters, Check for collision, Update Player infos
            *  -------------------------------------------------------------------------- */

            gameManager.play(&controller, &audioManager);
            renderManager.updateState(gameManager.getState());

            /* ------------------
            *   RENDERING CODE
            *  ------------------ */

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update The View Matrix each time we enter the while loop
            renderManager.updateMVMatrix(camera, gameManager.getMap()->getPacman(), gameManager.lost(), windowManager.getDeltaTime());

            // Render the map (objects, skybox and ground)
            renderManager.drawMap(gameManager.getMap(), &controller);

            // Render UI (life, score)
            if(!gameManager.isLost() && !gameManager.isPause())
            {
                renderManager.drawUI(&ui, gameManager.getStartTime(), gameManager.getPauseTime(), gameManager.getTotalTime());
            }


            // Render the pause menu if the game is paused
            if(gameManager.isPause())
            {
                renderManager.drawMenu(&menuPause);
            }
            if(gameManager.isLost())
            {
                renderManager.drawScorePanel(gameManager.getPlayer()->getPoints());
            }

            // Update the display
            windowManager.swapBuffers();

            gameManager.setPauseTime(SDL_GetTicks());
        }
    }

    return EXIT_SUCCESS;

    return 0;
}
