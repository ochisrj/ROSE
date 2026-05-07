#include <glad/glad.h>
#include "PostProcess.h"
#include "SpriteShader.h"
#include "../Core/Log.h"

PostProcess& PostProcess::Instance() {
    static PostProcess instance;
    return instance;
}

void PostProcess::Init(int width, int height) {
    if (m_Initialized) return;

    m_Width = width;
    m_Height = height;

    CreateFramebuffer();
    CreateQuad();

    m_Initialized = true;
    ROSE_LOG_INFO("PostProcess initialized (%dx%d)", width, height);
}

void PostProcess::Resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    DeleteFramebuffer();
    CreateFramebuffer();
}

void PostProcess::BeginRender() {
    if (!m_Initialized) return;
    BindFramebuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcess::EndRender() {
    if (!m_Initialized) return;
    UnbindFramebuffer();
}

void PostProcess::ApplyEffects() {
    if (!m_Initialized || m_ActiveEffects.empty()) {
        RenderQuad(SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D));
        return;
    }

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);

    unsigned int tempTex;
    glGenTextures(1, &tempTex);
    glBindTexture(GL_TEXTURE_2D, tempTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTex, 0);

    bool pingPong = false;

    for (auto& effect : m_ActiveEffects) {
        unsigned int shaderID = 0;

        switch (effect) {
            case PostProcessEffect::Bloom:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
            case PostProcessEffect::Vignette:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
            case PostProcessEffect::CRT:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
            case PostProcessEffect::ColorGrading:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
            case PostProcessEffect::Pixelate:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
            default:
                shaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);
                break;
        }

        unsigned int sourceTex = pingPong ? tempTex : m_SceneTextureID;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sourceTex);

        glUseProgram(shaderID);
        GLint loc = glGetUniformLocation(shaderID, "u_SceneTexture");
        if (loc != -1) glUniform1i(loc, 0);

        glViewport(0, 0, m_Width, m_Height);
        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        pingPong = !pingPong;
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tempTex);

    RenderQuad(SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D));
}

void PostProcess::AddEffect(PostProcessEffect effect) {
    for (auto& e : m_ActiveEffects) {
        if (e == effect) return;
    }
    m_ActiveEffects.push_back(effect);
}

void PostProcess::RemoveEffect(PostProcessEffect effect) {
    m_ActiveEffects.erase(
        std::remove(m_ActiveEffects.begin(), m_ActiveEffects.end(), effect),
        m_ActiveEffects.end()
    );
}

void PostProcess::ClearEffects() {
    m_ActiveEffects.clear();
}

void PostProcess::Shutdown() {
    DeleteFramebuffer();
    if (m_QuadVAO) glDeleteVertexArrays(1, &m_QuadVAO);
    if (m_QuadVBO) glDeleteBuffers(1, &m_QuadVBO);
    m_Initialized = false;
}

void PostProcess::CreateFramebuffer() {
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    glGenTextures(1, &m_SceneTextureID);
    glBindTexture(GL_TEXTURE_2D, m_SceneTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SceneTextureID, 0);

    glGenRenderbuffers(1, &m_RenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ROSE_LOG_ERROR("PostProcess framebuffer not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::DeleteFramebuffer() {
    if (m_FramebufferID) glDeleteFramebuffers(1, &m_FramebufferID);
    if (m_SceneTextureID) glDeleteTextures(1, &m_SceneTextureID);
    if (m_RenderbufferID) glDeleteRenderbuffers(1, &m_RenderbufferID);
    m_FramebufferID = 0;
    m_SceneTextureID = 0;
    m_RenderbufferID = 0;
}

void PostProcess::CreateQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);

    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void PostProcess::BindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
}

void PostProcess::UnbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::RenderQuad(unsigned int shaderID) {
    glUseProgram(shaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SceneTextureID);

    GLint loc = glGetUniformLocation(shaderID, "u_SceneTexture");
    if (loc != -1) glUniform1i(loc, 0);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
