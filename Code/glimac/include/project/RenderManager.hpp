#pragma once

#include <ft2build.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <sstream>
#include FT_FREETYPE_H

#include "glimac/Plane.hpp"
#include "glimac/Cube.hpp"
#include "glimac/Sphere.hpp"
#include <glimac/SDLWindowManager.hpp>
#include "glimac/Camera.hpp"

#include "project/Map.hpp"
#include "project/Controller.hpp"
#include "project/Texture.hpp"
#include "project/CubeMap.hpp"
#include "project/Text.hpp"

#include "project/GLSLProgram.hpp"
#include "project/GameManager.hpp"
#include "project/Menu.hpp"
#include "project/UI.hpp"

using namespace glimac;

struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};


class RenderManager {

private:
    // Window Manager
    SDLWindowManager* m_windowManager;
		SDL_Surface* m_screen;

    // Plane
    Plane m_plane;
    GLuint m_planeVBO;
    GLuint m_planeIBO;
    GLuint m_planeVAO;

    // Cube
    Cube m_cube;
    GLuint m_cubeVBO;
    GLuint m_cubeIBO;
    GLuint m_cubeVAO;

    // Sphere
    Sphere m_sphere;
    GLuint m_sphereVBO;
    GLuint m_sphereIBO;
    GLuint m_sphereVAO;

		// UI
		SDL_Surface* m_scoreSurface;
		GLuint m_scoreImg;
		SDL_Surface* m_timeSurface;
		GLuint m_timeImg;

		// Score Panel
		SDL_Surface* m_SP_titleSurface;
		SDL_Surface* m_SP_pointsSurface;
	  SDL_Surface* m_SP_timeSurface;
	  SDL_Surface* m_SP_pointsScoreSurface;
	  SDL_Surface* m_SP_timeScoreSurface;
		SDL_Surface* m_SP_Surface;
		GLuint m_SP_titleImg;
		GLuint m_SP_pointsImg;
	  GLuint m_SP_timeImg;
	  GLuint m_SP_pointsScoreImg;
	  GLuint m_SP_timeScoreImg;

    // Model3D
    Geometry m_model[4];
    GLuint m_modelVBO[4];
    GLuint m_modelIBO[4];
    GLuint m_modelVAO[4];

    // Matrix
    glm::mat4 m_ProjMatrix;
    glm::mat4 m_MVMatrix;
    glm::mat4 m_NormalMatrix;
		glm::mat4 m_OriginalMatrix;

    // Textures
    Texture* m_PacmanTexture;
    Texture* m_GhostTexture;
    Texture* m_WallTexture;
    Texture* m_GumTexture;
    Texture* m_SuperGumTexture;
    Texture* m_FruitTexture;
    CubeMap* m_SkyboxTexture;
    Texture* m_FloorTexture;
    Texture* m_RenderTargetTexture;

    // GLSL Programs
    ProgramList* m_programList;

    // Text
		TTF_Font* m_font;

		// Time
		std::string m_time;

    // Game Size Infos
    glm::vec2 m_gameSize;
    glm::vec2 m_gameCorner;

    // Skybox
    StaticObject* m_skybox;

    // Floor
    StaticObject* m_floor;

    // Mini-Map
    StaticObject* m_miniMap;

    // State (normal or super)
    GameManager::PacmanState m_state;

    // To make stop last rotate
    float m_stop;

public:
    // Constructor - SDLWindowManager for Ratio - Camera for viewMatrix - FilePath for Shaders
    RenderManager(SDLWindowManager* windowManager, Camera* camera, ProgramList* programList, glm::vec2 gameSize);
    // Destructor
    ~RenderManager();

    // Text
		void loadFont();
		SDL_Surface* createTextTexture(GLuint* textImg, std::string text, SDL_Color color);
		void drawText(SDL_Surface* textSurface, GLuint textImg, float size, float x, float y);
		void drawText(SDL_Surface* textSurface, GLuint textImg, float size, glm::mat4 matrix);

    // Skybox
    void initSkybox();
    Cube* getCubeSkyboxPtr();
    GLuint* getCubeSkyboxVAOPtr();

    // Cube getters
    Cube* getCubePtr();
    GLuint* getCubeVAOPtr();

    // Sphere getters
    Sphere* getSpherePtr();
    GLuint* getSphereVAOPtr();

    // Plane getters
    Plane* getPlanePtr();
    GLuint* getPlaneVAOPtr();

    // Rendering functions
    void bindCubeVAO();
    void bindTextVAO();
    void bindSphereVAO();
    void bindPlaneVAO();
    void debindVAO();

    // Matrix functions
    // Getters
    glm::mat4 getProjMatrix() const;
    glm::mat4 getMVMatrix() const;
    glm::mat4 getNormalMatrix() const;
    // Update
    void updateMVMatrix(Camera* camera);
    void updateMVMatrix(Camera* camera, Character* character, bool lost, uint32_t deltaTime);

    // Texture functions
    void loadTextures() const;
    void enableTexture(FS shader, Texture* texture, bool alpha);
    void disableTexture(FS shader, bool alpha);

		// Score panel
		void createScorePanel(int points);
		void drawScorePanel(int points);

    // GLSL Programs functions
    void useProgram(FS shader);

    // Matrix Transformations
    glm::mat4 transformMatrix(Object* object);
    void applyTransformations(FS shader, glm::mat4 matrix);

    // Material Transformations (for light shader)
    void materialTransformations(FS shader, float Kd, float Ks, float shininess);

    // Specific Transformations & Programs
    void drawPacman(Pacman* pacman, FS shader = NORMAL);
    void drawWalls(std::vector<Wall*>, FS shader = NORMAL);
    void drawGhosts(std::vector<Ghost*>, FS shader = NORMAL);
    void drawPacGommes(std::vector<Edible*>, FS shader = NORMAL);
    void drawSuperPacGommes(std::vector<Edible*>, FS shader = NORMAL);
    void drawFruits(std::vector<Edible*>, FS shader = NORMAL);
    void drawSkybox(Controller* controller);
    void drawFloor(FS shader);

    // Mini-map
    void drawMiniMap(FS shader);    // NOT WORKING ACTUALLY

    // Global
    void drawMap(Map* map, Controller* controller);

    // Build 3D Model
    void buildModel(Geometry*, GLuint* VBO, GLuint* IBO, GLuint* VAO, std::string, std::string);

    // Update state
    void updateState(GameManager::PacmanState);

    // MENU
    void drawMenu(Menu*);

    // UI
		std::string getTimeString(Uint32 final_time);
    void drawUI(UI*,Uint32 start_game_time, Uint32 pause_time, Uint32 total_time);
};
