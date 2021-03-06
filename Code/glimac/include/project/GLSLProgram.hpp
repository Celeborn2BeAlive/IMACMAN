#pragma once

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

namespace glimac {

// Every key player can use
enum FS {NORMAL, TEXTURE, TEXT, CUBEMAP, MULTITEXTURE, DIRECTIONNAL_LIGHT, POINT_LIGHT, BLACK_AND_WHITE};

// GLSL Program with the normal fragment shader
struct NormalProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    NormalProgram(){}
    NormalProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/normal.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
    }
};

// GLSL Program with the 3D texture fragment shader
struct TextureProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;
    GLint uTime;

    TextureProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
        uTime = glGetUniformLocation(m_Program.getGLId(), "uTime");
    }
};

// GLSL Program with the 3D texture fragment shader
struct TextProgram {
    Program m_Program;

    GLint attribute_coord;
    GLint uniform_tex;
    GLint uniform_color;

    TextProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/text.vs.glsl",
                              applicationPath.dirPath() + "shaders/text.fs.glsl")) {
        attribute_coord = glGetUniformLocation(m_Program.getGLId(), "coord");
        uniform_tex = glGetUniformLocation(m_Program.getGLId(), "tex");
        uniform_color = glGetUniformLocation(m_Program.getGLId(), "color");
    }
};

// GLSL Program with the Cubemap texture fragment shader
struct CubeMapProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint cubeTexture;

    CubeMapProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/cubeMap.vs.glsl",
                              applicationPath.dirPath() + "shaders/cubeMap.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        cubeTexture = glGetUniformLocation(m_Program.getGLId(), "cubeTexture");
    }
};

// GLSL Program with the Multi 3D texture fragment shader
struct MultiTextureProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture1;
    GLint uTexture2;

    MultiTextureProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture1 = glGetUniformLocation(m_Program.getGLId(), "uTexture1");
        uTexture2 = glGetUniformLocation(m_Program.getGLId(), "uTexture2");
    }
};

// GLSL Program with the Directionnal Light fragment shader
struct DirectionnalLightProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightDir_vs;
    GLint uLightIntensity;

    GLint uColor;

    DirectionnalLightProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/directionnalLightTex.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
        // Variables uniformes materiaux
        uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        // Variables uniformes lumieres
        uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
        uColor = glGetUniformLocation(m_Program.getGLId(), "uColor");
    }
};

// GLSL Program with the Point Light fragment shader
struct PointLightProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightPos_vs;
    GLint uLightIntensity;

    PointLightProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/pointlight.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
        // Variables uniformes materiaux
        uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        // Variables uniformes lumieres
        uLightPos_vs = glGetUniformLocation(m_Program.getGLId(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
    }
};

// Black & White
struct BlackAndWhiteProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightDir_vs;
    GLint uLightIntensity;

    BlackAndWhiteProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/blackAndWhite.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");

        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
        // Variables uniformes materiaux
        uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        // Variables uniformes lumieres
        uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
    }
};

struct ProgramList {
    NormalProgram* normalProgram;
    TextureProgram* textureProgram;
    MultiTextureProgram* multiTextureProgram;
    DirectionnalLightProgram* directionnalLightProgram;
    PointLightProgram* pointLightProgram;
    CubeMapProgram* cubemapProgram;
    TextProgram* textProgram;
    BlackAndWhiteProgram* bwProgram;
};

}
