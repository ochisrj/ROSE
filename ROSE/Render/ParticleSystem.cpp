#include <glad/glad.h>
#include "ParticleSystem.h"
#include "SpriteShader.h"
#include "../Core/Log.h"

ParticleSystem& ParticleSystem::Instance() {
    static ParticleSystem instance;
    return instance;
}

void ParticleSystem::Init() {
    ROSE_LOG_INFO("ParticleSystem initialized");
}

void ParticleSystem::Update(float deltaTime) {
    for (auto& emitter : m_Emitters) {
        emitter->Update(deltaTime);
    }
}

void ParticleSystem::Render() {
    for (auto& emitter : m_Emitters) {
        emitter->Render();
    }
}

void ParticleSystem::Clear() {
    m_Emitters.clear();
}

ParticleEmitter* ParticleSystem::CreateEmitter(const ParticleEmitterSettings& settings) {
    auto emitter = std::make_unique<ParticleEmitter>(settings);
    emitter->Init();
    ParticleEmitter* ptr = emitter.get();
    m_Emitters.push_back(std::move(emitter));
    return ptr;
}

void ParticleSystem::RemoveEmitter(ParticleEmitter* emitter) {
    m_Emitters.erase(
        std::remove_if(m_Emitters.begin(), m_Emitters.end(), [emitter](const std::unique_ptr<ParticleEmitter>& e) {
            return e.get() == emitter;
        }),
        m_Emitters.end()
    );
}

ParticleEmitter::ParticleEmitter()
    : m_Rng(std::random_device{}()), m_Dist(0.0f, 1.0f) {
}

ParticleEmitter::ParticleEmitter(const ParticleEmitterSettings& settings)
    : m_Settings(settings), m_Rng(std::random_device{}()), m_Dist(0.0f, 1.0f) {
    m_Particles.resize(settings.MaxParticles);
    m_ParticleData.resize(settings.MaxParticles * 8);
}

void ParticleEmitter::Init() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Settings.MaxParticles * 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

    glBindVertexArray(0);

    for (auto& p : m_Particles) {
        p.Active = false;
        p.Lifetime = 0.0f;
    }
}

void ParticleEmitter::Update(float deltaTime) {
    if (!m_IsPlaying) return;

    m_EmitTimer += deltaTime * m_Settings.EmissionRate;
    while (m_EmitTimer >= 1.0f) {
        EmitParticle();
        m_EmitTimer -= 1.0f;
    }

    for (auto& particle : m_Particles) {
        if (!particle.Active) continue;
        UpdateParticle(particle, deltaTime);
    }

    UploadParticles();
}

void ParticleEmitter::UpdateParticle(Particle& particle, float deltaTime) {
    particle.Lifetime -= deltaTime;
    if (particle.Lifetime <= 0.0f) {
        particle.Active = false;
        return;
    }

    particle.Velocity.y += m_Settings.Gravity * deltaTime;
    particle.Velocity *= m_Settings.Damping;

    particle.Position += particle.Velocity * deltaTime;
    particle.Rotation += particle.RotationSpeed * deltaTime;

    float t = particle.Lifetime / particle.MaxLifetime;
    particle.Color = glm::mix(m_Settings.MinColor, m_Settings.MaxColor, t);
    particle.Size = glm::mix(m_Settings.MinSize, m_Settings.MaxSize, t);
}

void ParticleEmitter::UploadParticles() {
    int index = 0;
    for (const auto& p : m_Particles) {
        if (!p.Active) continue;

        float s = 0.5f;
        float c = cos(p.Rotation);
        float sn = sin(p.Rotation);

        glm::vec2 offsets[4] = {
            glm::vec2(-s * c + s * sn, -s * sn - s * c),
            glm::vec2( s * c + s * sn,  s * sn - s * c),
            glm::vec2( s * c - s * sn,  s * sn + s * c),
            glm::vec2(-s * c - s * sn, -s * sn + s * c)
        };

        for (int i = 0; i < 4; i++) {
            int idx = (index * 4 + i) * 8;
            m_ParticleData[idx + 0] = p.Position.x + offsets[i].x * p.Size.x;
            m_ParticleData[idx + 1] = p.Position.y + offsets[i].y * p.Size.y;
            m_ParticleData[idx + 2] = (float)(i % 2);
            m_ParticleData[idx + 3] = (float)(i / 2);
            m_ParticleData[idx + 4] = p.Color.r;
            m_ParticleData[idx + 5] = p.Color.g;
            m_ParticleData[idx + 6] = p.Color.b;
            m_ParticleData[idx + 7] = p.Color.a;
        }

        index++;
        if (index >= m_Settings.MaxParticles) break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_ParticleData.size() * sizeof(float), m_ParticleData.data());
}

void ParticleEmitter::EmitParticle() {
    Particle* target = nullptr;
    for (auto& p : m_Particles) {
        if (!p.Active) {
            target = &p;
            break;
        }
    }
    if (!target) return;

    float randX = m_Dist(m_Rng);
    float randY = m_Dist(m_Rng);

    target->Position = m_Settings.Position + glm::vec2(
        (randX - 0.5f) * m_Settings.SpawnSize.x,
        (randY - 0.5f) * m_Settings.SpawnSize.y
    );

    target->Velocity = glm::vec2(
        m_Settings.MinVelocity.x + m_Dist(m_Rng) * (m_Settings.MaxVelocity.x - m_Settings.MinVelocity.x),
        m_Settings.MinVelocity.y + m_Dist(m_Rng) * (m_Settings.MaxVelocity.y - m_Settings.MinVelocity.y)
    );

    float t = m_Dist(m_Rng);
    target->Color = glm::mix(m_Settings.MinColor, m_Settings.MaxColor, t);
    target->Size = glm::mix(m_Settings.MinSize, m_Settings.MaxSize, t);

    target->MaxLifetime = m_Settings.MinLifetime + m_Dist(m_Rng) * (m_Settings.MaxLifetime - m_Settings.MinLifetime);
    target->Lifetime = target->MaxLifetime;

    target->Rotation = 0.0f;
    target->RotationSpeed = m_Settings.MinRotationSpeed + m_Dist(m_Rng) * (m_Settings.MaxRotationSpeed - m_Settings.MinRotationSpeed);

    target->Active = true;
    target->TextureID = m_Settings.TextureID;
}

void ParticleEmitter::Render() {
    int activeCount = 0;
    for (const auto& p : m_Particles) {
        if (p.Active) activeCount++;
    }

    if (activeCount == 0) return;

    glUseProgram(SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D));

    if (m_Settings.TextureID) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Settings.TextureID);
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, activeCount * 4);
    glBindVertexArray(0);
}

void ParticleEmitter::Start() {
    m_IsPlaying = true;
}

void ParticleEmitter::Stop() {
    m_IsPlaying = false;
}

void ParticleEmitter::Reset() {
    for (auto& p : m_Particles) {
        p.Active = false;
        p.Lifetime = 0.0f;
    }
    m_EmitTimer = 0.0f;
}

void ParticleEmitter::Burst(int count) {
    for (int i = 0; i < count; i++) {
        EmitParticle();
    }
}

void ParticleEmitter::SetSettings(const ParticleEmitterSettings& settings) {
    m_Settings = settings;
    m_Particles.resize(settings.MaxParticles);
    m_ParticleData.resize(settings.MaxParticles * 8);
}

int ParticleEmitter::GetActiveParticleCount() const {
    int count = 0;
    for (const auto& p : m_Particles) {
        if (p.Active) count++;
    }
    return count;
}
