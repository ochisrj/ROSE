#include <glad/glad.h>
#include "Renderer.h"
#include "../Core/Log.h"

Renderer& Renderer::Instance() {
    static Renderer instance;
    return instance;
}

void Renderer::Init() {
    if (m_Initialized) return;
    ROSE_LOG_INFO("Renderer initialized with %d layers", (int)RenderLayer::Count);
    m_Initialized = true;
}

void Renderer::BeginFrame() {
    if (m_BlendEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Renderer::EndFrame() {
    for (int i = 0; i < (int)RenderLayer::Count; i++) {
        DrawLayer(static_cast<RenderLayer>(i));
    }
}

void Renderer::DrawLayer(RenderLayer layer) {
    int idx = (int)layer;
    if (idx < 0 || idx >= (int)RenderLayer::Count) return;

    for (auto* renderable : m_RenderQueue[idx]) {
        renderable->Submit();
    }
}

void Renderer::Clear() {
    for (int i = 0; i < (int)RenderLayer::Count; i++) {
        m_RenderQueue[i].clear();
    }
}

void Renderer::Submit(Renderable* renderable) {
    int idx = (int)renderable->GetLayer();
    if (idx >= 0 && idx < (int)RenderLayer::Count) {
        m_RenderQueue[idx].push_back(renderable);
    }
}

void Renderer::SetRenderMode(RenderMode mode) {
    m_Mode = mode;
}

void Renderer::SetBlendEnabled(bool enabled) {
    m_BlendEnabled = enabled;
    if (!enabled) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Renderer::Resize(int width, int height) {
    m_Width = width;
    m_Height = height;
}
