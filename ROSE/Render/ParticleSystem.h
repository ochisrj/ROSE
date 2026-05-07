#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <glm/glm.hpp>
#include <vector>
#include <random>

struct Particle {
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec4 Color;
    glm::vec2 Size;
    float Lifetime;
    float MaxLifetime;
    float Rotation;
    float RotationSpeed;
    bool Active;
    unsigned int TextureID;
};

struct ParticleEmitterSettings {
    glm::vec2 Position = glm::vec2(0.0f, 0.0f);
    glm::vec2 SpawnSize = glm::vec2(0.0f, 0.0f);
    glm::vec2 MinVelocity = glm::vec2(-1.0f, -1.0f);
    glm::vec2 MaxVelocity = glm::vec2(1.0f, 1.0f);
    glm::vec4 MinColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 MaxColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec2 MinSize = glm::vec2(1.0f, 1.0f);
    glm::vec2 MaxSize = glm::vec2(1.0f, 1.0f);
    float MinLifetime = 0.5f;
    float MaxLifetime = 2.0f;
    float EmissionRate = 10.0f;
    int MaxParticles = 1000;
    bool Loop = true;
    float Gravity = 0.0f;
    float Damping = 1.0f;
    float MinRotationSpeed = -1.0f;
    float MaxRotationSpeed = 1.0f;
    unsigned int TextureID = 0;
};

class ParticleEmitter {
public:
    ParticleEmitter();
    explicit ParticleEmitter(const ParticleEmitterSettings& settings);

    void Init();
    void Update(float deltaTime);
    void Render();

    void Start();
    void Stop();
    void Reset();
    void Burst(int count);

    void SetSettings(const ParticleEmitterSettings& settings);
    const ParticleEmitterSettings& GetSettings() const { return m_Settings; }

    void SetPosition(glm::vec2 pos) { m_Settings.Position = pos; }
    void SetEmissionRate(float rate) { m_Settings.EmissionRate = rate; }
    bool IsPlaying() const { return m_IsPlaying; }

    unsigned int GetVAO() const { return m_VAO; }
    int GetActiveParticleCount() const;

private:
    void EmitParticle();
    void UpdateParticle(Particle& particle, float deltaTime);
    void UploadParticles();

    ParticleEmitterSettings m_Settings;
    std::vector<Particle> m_Particles;
    std::vector<float> m_ParticleData;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_ShaderID = 0;

    bool m_IsPlaying = false;
    float m_EmitTimer = 0.0f;
    std::mt19937 m_Rng;
    std::uniform_real_distribution<float> m_Dist;
};

class ParticleSystem {
public:
    static ParticleSystem& Instance();

    void Init();
    void Update(float deltaTime);
    void Render();
    void Clear();

    ParticleEmitter* CreateEmitter(const ParticleEmitterSettings& settings);
    void RemoveEmitter(ParticleEmitter* emitter);

    int GetActiveEmitters() const { return (int)m_Emitters.size(); }

private:
    ParticleSystem() = default;
    std::vector<std::unique_ptr<ParticleEmitter>> m_Emitters;
};

#endif
