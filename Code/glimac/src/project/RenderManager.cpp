#include <iostream>

#include "project/RenderManager.hpp"

using namespace glimac;

// Constructor
RenderManager::RenderManager(SDLWindowManager* windowManager, Camera* camera, ProgramList* programList, glm::vec2 gameSize)
{
    // Window Manager
    m_windowManager = windowManager;

<<<<<<< HEAD
    // Camera
    // m_ffCamera = camera;

    // Skybox
    m_skybox = new StaticObject('K', 15.f, 20.f, 250.f, 250.f);
=======
    // Plane
    m_plane = Plane();
    m_planeVBO = m_plane.getVBO();
    m_planeIBO = m_plane.getIBO();
    m_planeVAO = m_plane.getVAO(&m_planeIBO, &m_planeVBO);
>>>>>>> 316b78aa06d985d9aa217cc91f28c6ce105b6266

    // Cube
    m_cube = Cube();
    m_cubeVBO = m_cube.getVBO();
    m_cubeIBO = m_cube.getIBO();
    m_cubeVAO = m_cube.getVAO(&m_cubeIBO, &m_cubeVBO);

    // Sphere
    m_sphere = Sphere();
    m_sphereVBO = m_sphere.getVBO();
    m_sphereIBO = m_sphere.getIBO();
    m_sphereVAO = m_sphere.getVAO(&m_sphereIBO, &m_sphereVBO);

    // Matrices
    // Projection Matrix (world) : vertical view angle, window ratio, near, far
    m_ProjMatrix = glm::perspective(glm::radians(70.f), windowManager->getRatio(), 0.1f, 200.f);
    // ModelView Matrix (camera)
    m_MVMatrix = camera->getViewMatrix();
    // Normal Matrix in the camera landmark
    m_NormalMatrix = glm::transpose(glm::inverse(m_MVMatrix));

    // Textures
    m_PacmanTexture = new Texture("../Code/assets/textures/pacman.jpg");
    m_GhostTexture = new Texture("../Code/assets/textures/ghost.jpg");
    m_WallTexture = new Texture("../Code/assets/textures/dungeon.png");
    m_GumTexture = new Texture("../Code/assets/textures/gum.jpg");
    m_SuperGumTexture = new Texture("../Code/assets/textures/superpacgum.jpg");
    m_FruitTexture = new Texture("../Code/assets/textures/fruit.jpg");
    m_SkyboxTexture = new CubeMap("../Code/assets/textures/darkness.jpg",
                                  "../Code/assets/textures/darkness.jpg",
                                  "../Code/assets/textures/darkness.jpg",
                                  "../Code/assets/textures/darkness.jpg",
                                  "../Code/assets/textures/darkness.jpg",
                                  "../Code/assets/textures/darkness.jpg"
                                );
    m_FloorTexture = new Texture("../Code/assets/textures/lava2.jpg");

    m_RenderTargetTexture = new Texture();

    // Text
    m_text.init_resources();

    // Freetype library
    m_ft = new FT_Library();

    // GLSL Program
    m_programList = programList;

    // Game Size Infos
    m_gameSize = gameSize;
    m_gameCorner = glm::vec2(-(gameSize.x / 2), -(gameSize.y / 2));

    // Skybox
    m_skybox = new StaticObject('K', gameSize.y/2, gameSize.x/2, 300.f, 300.f);

    // Floor
    m_floor = new StaticObject('F', gameSize.y/2, gameSize.x/2, gameSize.y, gameSize.x);

    // Mini Map
    m_miniMap = new StaticObject('F', gameSize.y/2, gameSize.x/2, gameSize.y, gameSize.x);
}

// Destructor
RenderManager::~RenderManager()
{
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteVertexArrays(1, &m_cubeVAO);

    glDeleteBuffers(1, &m_sphereVBO);
    glDeleteVertexArrays(1, &m_sphereVAO);

    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_planeVAO);

    delete(m_PacmanTexture);
    delete(m_GhostTexture);
    delete(m_WallTexture);
    delete(m_GumTexture);
    delete(m_SuperGumTexture);
    delete(m_FruitTexture);
    delete(m_SkyboxTexture);
    delete(m_FloorTexture);

    delete(m_skybox);
    delete(m_floor);
}

// ---------------
// TEXT FUNCTIONS
// ---------------
/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void RenderManager::renderText(const char *text, float x, float y, float sx, float sy) {
  useProgram(TEXT);
	const char *p;
  FT_Face face = m_text.getFace();
	FT_GlyphSlot g = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(m_programList->textProgram->uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(m_programList->textProgram->attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, m_text.getVBO());
	glVertexAttribPointer(m_programList->textProgram->attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		(FT_Load_Char(face, *p, FT_LOAD_RENDER));

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{x2, -y2, 0, 0},
			{x2 + w, -y2, 1, 0},
			{x2, -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(m_programList->textProgram->attribute_coord);
	glDeleteTextures(1, &tex);
}

void RenderManager::drawText() {
  FT_Face face = m_text.getFace();

	float sx = 2.0 / 1280;
	float sy = 2.0 / 720;

  useProgram(TEXT);

  /* White background */
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Enable blending, necessary for our alpha texture */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat black[4] = { 0, 0, 0, 1 };
	GLfloat red[4] = { 1, 0, 0, 1 };
	GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

	/* Set font size to 48 pixels, color to black */
	FT_Set_Pixel_Sizes(face, 0, 48);
	glUniform4fv(m_programList->textProgram->uniform_color, 1, red);

	/* Effects of alignment */
	renderText("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy);
	// renderText("The Misaligned Fox Jumps Over The Lazy Dog", -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);
  //
	// /* Scaling the texture versus changing the font size */
	// renderText("The Small Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 175 * sy, sx * 0.5, sy * 0.5);
	// FT_Set_Pixel_Sizes(face, 0, 24);
	// renderText("The Small Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 200 * sy, sx, sy);
	// FT_Set_Pixel_Sizes(face, 0, 48);
	// renderText("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 235 * sy, sx * 0.25, sy * 0.25);
	// FT_Set_Pixel_Sizes(face, 0, 12);
	// renderText("The Tiny Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 250 * sy, sx, sy);
	// FT_Set_Pixel_Sizes(face, 0, 48);
  //
	// /* Colors and transparency */
	// renderText("The Solid Black Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 430 * sy, sx, sy);
  //
	// glUniform4fv(m_programList->textProgram->uniform_color, 1, red);
	// renderText("The Solid Red Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 330 * sy, sx, sy);
	// renderText("The Solid Red Fox Jumps Over The Lazy Dog", -1 + 28 * sx, 1 - 450 * sy, sx, sy);
  //
	// glUniform4fv(m_programList->textProgram->uniform_color, 1, transparent_green);
	// renderText("The Transparent Green Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 380 * sy, sx, sy);
	// renderText("The Transparent Green Fox Jumps Over The Lazy Dog", -1 + 18 * sx, 1 - 440 * sy, sx, sy);
}

// ---------------
// CUBE FUNCTIONS
// ---------------

// Returns pointer of cube object
Cube* RenderManager::getCubePtr()
{
    return &m_cube;
}
// Returns pointer of cube VAO
GLuint* RenderManager::getCubeVAOPtr()
{
    return &m_cubeVAO;
}

// ---------------
// SPHERE FUNCTIONS
// ---------------

// Returns pointer of sphere object
Sphere* RenderManager::getSpherePtr()
{
    return &m_sphere;
}
// Returns pointer of sphere VAO
GLuint* RenderManager::getSphereVAOPtr()
{
    return &m_sphereVAO;
}

// ---------------
// PLANE FUNCTIONS
// ---------------

// Returns pointer of plane object
Plane* RenderManager::getPlanePtr()
{
    return &m_plane;
}
// Returns pointer of plane VAO
GLuint* RenderManager::getPlaneVAOPtr()
{
    return &m_planeVAO;
}


// ---------------
// RENDERING FUNCTIONS
// ---------------

void RenderManager::bindCubeVAO()
{
    glBindVertexArray(m_cubeVAO);
}
void RenderManager::bindSphereVAO()
{
    glBindVertexArray(m_sphereVAO);
}
void RenderManager::bindPlaneVAO()
{
    glBindVertexArray(m_planeVAO);
}
void RenderManager::debindVAO()
{
    glBindVertexArray(0);
}

// ---------------
// MATRIX FUNCTIONS
// ---------------

// Getters
glm::mat4 RenderManager::getProjMatrix() const
{
    return m_ProjMatrix;
}
glm::mat4 RenderManager::getMVMatrix() const
{
    return m_MVMatrix;
}
glm::mat4 RenderManager::getNormalMatrix() const
{
    return m_NormalMatrix;
}

// Update
void RenderManager::updateMVMatrix(Camera* camera)
{
    m_MVMatrix = camera->getViewMatrix();
}
void RenderManager::updateMVMatrix(Camera* camera, Character* character)
{
    m_MVMatrix = camera->getViewMatrix(character, m_gameCorner);
}

// ---------------
// TEXTURES FUNCTIONS
// ---------------

void RenderManager::loadTextures() const
{
    m_PacmanTexture->loadTexture();
    m_GhostTexture->loadTexture();
    m_WallTexture->loadTexture();
    m_GumTexture->loadTexture();
    m_SuperGumTexture->loadTexture();
    m_FruitTexture->loadTexture();
    m_SkyboxTexture->loadCubeMap();
    m_FloorTexture->loadTexture();
    m_RenderTargetTexture->loadTexture();
}


// ---------------
// GLSL PROGRAM FUNCTIONS
// ---------------

// Use the correct program associated to the shader
void RenderManager::useProgram(FS shader)
{
    switch (shader)
    {
        case NORMAL :
            m_programList->normalProgram->m_Program.use();
            break;

        case TEXTURE :
            m_programList->textureProgram->m_Program.use();
            break;

        case CUBEMAP :
            m_programList->cubemapProgram->m_Program.use();
            break;

        case DIRECTIONNAL_LIGHT :
            m_programList->directionnalLightProgram->m_Program.use();
            break;

        case POINT_LIGHT :
            m_programList->pointLightProgram->m_Program.use();
            break;

        default :
            m_programList->normalProgram->m_Program.use();
            break;
    }
}

// ---------------
// GENERAL MATRIX TRANSFORMATIONS
// ---------------

// Do the correct transformations
glm::mat4 RenderManager::transformMatrix(Object* object)
{
    glm::mat4 matrix = m_MVMatrix;

    // Translate to correct Position
    matrix = glm::translate(matrix, glm::vec3(m_gameCorner.x + object->getPosX(), 0, m_gameCorner.y + object->getPosY()));
    // Rotate the object - Orientation
    matrix = glm::rotate(matrix, (float)object->getOrientation() * glm::pi<float>()/180, glm::vec3(0, 1, 0));
    // Scale the object
    matrix = glm::scale(matrix, glm::vec3(object->getWidth(), object->getHeight(), object->getWidth()));

    return matrix;
}

// Apply Transformations, Update Uniforms
void RenderManager::applyTransformations(FS shader, glm::mat4 matrix)
{
    glm::mat4 lightMatrix;
    glm::vec4 lightVector;
    switch (shader)
    {
        case NORMAL :
            glUniformMatrix4fv(m_programList->normalProgram->uMVPMatrix, 1, GL_FALSE,
            glm::value_ptr(m_ProjMatrix * matrix));

            glUniformMatrix4fv(m_programList->normalProgram->uMVMatrix, 1, GL_FALSE,
            glm::value_ptr(matrix));

            glUniformMatrix4fv(m_programList->normalProgram->uNormalMatrix, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(matrix))));
            break;

        case TEXTURE :
            glUniform1i(m_programList->textureProgram->uTexture, 0);

            glUniform1f(m_programList->textureProgram->uTime, 0);

            glUniformMatrix4fv(m_programList->textureProgram->uMVPMatrix, 1, GL_FALSE,
            glm::value_ptr(m_ProjMatrix * matrix));

            glUniformMatrix4fv(m_programList->textureProgram->uMVMatrix, 1, GL_FALSE,
            glm::value_ptr(matrix));

            glUniformMatrix4fv(m_programList->textureProgram->uNormalMatrix, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(matrix))));
            break;

        case CUBEMAP :
            glUniformMatrix4fv(m_programList->cubemapProgram->uMVPMatrix, 1, GL_FALSE,
            glm::value_ptr(m_ProjMatrix * matrix));

            glUniformMatrix4fv(m_programList->cubemapProgram->uMVMatrix, 1, GL_FALSE,
            glm::value_ptr(matrix));

            glUniformMatrix4fv(m_programList->cubemapProgram->uNormalMatrix, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(matrix))));
            break;

        case DIRECTIONNAL_LIGHT :
            glUniform1i(m_programList->directionnalLightProgram->uTexture, 0);

            lightMatrix = glm::rotate(m_MVMatrix, 180.f, glm::vec3(1,1,1));
            lightVector = glm::normalize(glm::vec4(1,1,1,0)*lightMatrix);
            glUniform3f(m_programList->directionnalLightProgram->uLightDir_vs, lightVector.x, lightVector.y, lightVector.z);
            glUniform3f(m_programList->directionnalLightProgram->uLightIntensity, 2.0,2.0,2.0);

            glUniformMatrix4fv(m_programList->directionnalLightProgram->uMVPMatrix, 1, GL_FALSE,
            glm::value_ptr(m_ProjMatrix * matrix));

            glUniformMatrix4fv(m_programList->directionnalLightProgram->uMVMatrix, 1, GL_FALSE,
            glm::value_ptr(matrix));

            glUniformMatrix4fv(m_programList->directionnalLightProgram->uNormalMatrix, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(matrix))));
            break;

        case POINT_LIGHT :
            glUniform1i(m_programList->pointLightProgram->uTexture, 0);

            lightMatrix = glm::scale(matrix, glm::vec3(1000.f, 1000.f, 1000.f));
            lightVector = glm::normalize(glm::vec4(1,1,1,0)*matrix);
            glUniform3f(m_programList->pointLightProgram->uLightPos_vs, lightVector.x, lightVector.y, lightVector.z);
            glUniform3f(m_programList->pointLightProgram->uLightIntensity, 2.0,2.0,2.0);

            glUniformMatrix4fv(m_programList->pointLightProgram->uMVPMatrix, 1, GL_FALSE,
            glm::value_ptr(m_ProjMatrix * matrix));

            glUniformMatrix4fv(m_programList->pointLightProgram->uMVMatrix, 1, GL_FALSE,
            glm::value_ptr(matrix));

            glUniformMatrix4fv(m_programList->pointLightProgram->uNormalMatrix, 1, GL_FALSE,
            glm::value_ptr(glm::transpose(glm::inverse(matrix))));
            break;

        default :
            break;
    }
}

// Material Transformations for light
void RenderManager::materialTransformations(FS shader, float Kd, float Ks, float shininess)
{
    if ((shader == DIRECTIONNAL_LIGHT) || (shader == POINT_LIGHT))
    {
        glUniform3f(m_programList->directionnalLightProgram->uKd, Kd, Kd, Kd);
        glUniform3f(m_programList->directionnalLightProgram->uKs,  Ks, Ks, Ks);
        glUniform1f(m_programList->directionnalLightProgram->uShininess, shininess);
    }
}

// Enable texture if we use shader texture
void RenderManager::enableTexture(FS shader, Texture* texture)
{
    if ((shader == TEXTURE) || (shader == DIRECTIONNAL_LIGHT) || (shader == POINT_LIGHT))
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getID());
    }
}

// Disable texture if we use shader texture
void RenderManager::disableTexture(FS shader)
{
    if ((shader == TEXTURE) || (shader == DIRECTIONNAL_LIGHT) || (shader == POINT_LIGHT))
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }
}

// ---------------
// SPECIFIC TRANSFORMATIONS & PROGRAMS (shader)
// ---------------

// ---- CHARACTERS ----- //

// PACMAN
void RenderManager::drawPacman(Pacman* pacman, FS shader)
{
    //useProgram(shader);

    glm::mat4 transformationMatrix = transformMatrix(pacman);
    applyTransformations(shader, transformationMatrix);

    materialTransformations(shader, 0.7, 0.3, 100);

    enableTexture(shader, m_PacmanTexture);

    m_sphere.drawSphere();

    disableTexture(shader);
}

// GHOSTS
void RenderManager::drawGhosts(std::vector<Ghost*> ghost, FS shader)
{
    for (unsigned int i = 0; i < ghost.size(); ++i)
    {
        if (ghost[i]->getSuper())
        {
            useProgram(NORMAL);

            glm::mat4 transformationMatrix = transformMatrix(ghost[i]);
            applyTransformations(NORMAL, transformationMatrix);
            m_cube.drawCube();
        }
        else
        {
            useProgram(shader);

            enableTexture(shader, m_GhostTexture);

            glm::mat4 transformationMatrix = transformMatrix(ghost[i]);
            applyTransformations(shader, transformationMatrix);
            materialTransformations(shader, 0.8, 0.2, 100);
            m_cube.drawCube();

            disableTexture(shader);
        }
    }
}

// ---- ALL OBJECTS ----- //

// WALLS
void RenderManager::drawWalls(std::vector<Wall*> wall, FS shader)
{
    //useProgram(shader);
    enableTexture(shader, m_WallTexture);

    for (unsigned int i = 0; i < wall.size(); ++i)
    {
        glm::mat4 transformationMatrix = transformMatrix(wall[i]);
        applyTransformations(shader, transformationMatrix);
        materialTransformations(shader, 0.9, 0.1, 20);
        m_cube.drawCube();
    }

    disableTexture(shader);
}

// GOMMES
void RenderManager::drawPacGommes(std::vector<Edible*> edible, FS shader)
{
    //useProgram(shader);
    enableTexture(shader, m_GumTexture);

    for (unsigned int i = 0; i < edible.size(); ++i)
    {
        glm::mat4 transformationMatrix = transformMatrix(edible[i]);
        applyTransformations(shader, transformationMatrix);
        materialTransformations(shader, 0.9, 0.1, 50);
        m_sphere.drawSphere();
    }

    disableTexture(shader);
}

// SUPER GOMMES
void RenderManager::drawSuperPacGommes(std::vector<Edible*> edible, FS shader)
{
    //useProgram(shader);
    enableTexture(shader, m_SuperGumTexture);

    for (unsigned int i = 0; i < edible.size(); ++i)
    {
        glm::mat4 transformationMatrix = transformMatrix(edible[i]);
        applyTransformations(shader, transformationMatrix);
        materialTransformations(shader, 0.8, 0.2, 50);
        m_sphere.drawSphere();
    }

    disableTexture(shader);
}

// FRUITS
void RenderManager::drawFruits(std::vector<Edible*> edible, FS shader)
{
    //useProgram(shader);

    for (unsigned int i = 0; i < edible.size(); ++i)
    {
        if (edible[i]->getAvailability())
        {
            enableTexture(shader, m_FruitTexture);
            glm::mat4 transformationMatrix = transformMatrix(edible[i]);
            applyTransformations(shader, transformationMatrix);
            materialTransformations(shader, 0.9, 0.3, 50);
            m_sphere.drawSphere();
            disableTexture(shader);
        }
    }
}

// ---- SKYBOX ----- //
void RenderManager::initSkybox()
{
  useProgram(CUBEMAP);
  glm::mat4 transformationMatrix = transformMatrix(m_skybox);
  applyTransformations(CUBEMAP, transformationMatrix);
}

void RenderManager::drawSkybox()
{
  glDepthMask(GL_FALSE);
  //useProgram(CUBEMAP);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTexture->getID());
  m_cube.drawCube();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glDepthMask(GL_TRUE);
}

// ---- FLOOR ---- //
void RenderManager::drawFloor(FS shader)
{
    //useProgram(shader);

    // Matrix Transformations
    glm::mat4 matrix = m_MVMatrix;
    matrix = glm::translate(matrix, glm::vec3(-3.5f, -0.5f, 2.5f));  // Some values set to adjust the plane
    matrix = glm::rotate(matrix, (float)-90 * glm::pi<float>()/180, glm::vec3(1, 0, 0));
    matrix = glm::scale(matrix, glm::vec3(m_floor->getWidth(), m_floor->getHeight(), 1.f));

    applyTransformations(shader, matrix);

    glUniform1f(m_programList->textureProgram->uTime, 0.00001f*SDL_GetTicks());

    enableTexture(shader, m_FloorTexture);
    m_plane.drawPlane();
    disableTexture(shader);
}

// ---- MINI MAP ---- //
void RenderManager::drawMiniMap(FS shader)
{
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    // Matrix Transformations
    glm::mat4 matrix = m_MVMatrix;
    matrix = glm::translate(matrix, glm::vec3(-3.5f, -0.5f, 2.5f));  // Some values set to adjust the plane
    //matrix = glm::rotate(matrix, (float)-90 * glm::pi<float>()/180, glm::vec3(1, 0, 0));
    matrix = glm::scale(matrix, glm::vec3(m_floor->getWidth(), m_floor->getHeight(), 1.f));

    applyTransformations(shader, matrix);

    enableTexture(shader, m_RenderTargetTexture);
    m_plane.drawPlane();
    disableTexture(shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the
}

// ---- GLOBAL ---- //

// Draw the map (Characters & Objects)
void RenderManager::drawMap(Map* map, Controller* controller)
{
    // --- PLANE OBJECTS --- //
    bindPlaneVAO();

    useProgram(TEXTURE);
    drawFloor(TEXTURE);
    //drawMiniMap(TEXTURE);

    debindVAO();

    //--- CUBES OBJECTS --- //
    bindCubeVAO();

    useProgram(CUBEMAP);
    drawSkybox();

    drawGhosts(map->getGhosts(), DIRECTIONNAL_LIGHT);

    useProgram(DIRECTIONNAL_LIGHT);
    drawWalls(map->getWalls(), DIRECTIONNAL_LIGHT);

    debindVAO();

    // --- SPHERES OBJECTS --- //
    // Bind Sphere VAO
    bindSphereVAO();

    // Draw Pacman only in TPS
    if(!controller->isFPSactive())
        drawPacman(map->getPacman(), DIRECTIONNAL_LIGHT);
    drawPacGommes(map->getPacGommes(), DIRECTIONNAL_LIGHT);
    drawSuperPacGommes(map->getSuperPacGommes(), DIRECTIONNAL_LIGHT);
    drawFruits(map->getFruits(), DIRECTIONNAL_LIGHT);

    //De-bind Sphere VAO
    debindVAO();
}
