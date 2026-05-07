#ifndef ANIMATION_2D_H
#define ANIMATION_2D_H

#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

struct AnimationFrame {
    int X, Y;
    int Width, Height;
    float Duration;
};

struct Animation {
    std::string Name;
    std::vector<AnimationFrame> Frames;
    bool Loop = true;
    bool PingPong = false;
    int CurrentFrame = 0;
    float Timer = 0.0f;
    float TotalDuration = 0.0f;
    bool IsPlaying = false;
};

class Animator2D {
public:
    Animator2D();

    void AddAnimation(const std::string& name, const std::vector<AnimationFrame>& frames,
                      bool loop = true, bool pingPong = false);
    void AddAnimationFromSheet(const std::string& name, int frameCount,
                                float frameDuration, int spriteWidth, int spriteHeight,
                                int sheetWidth, bool loop = true);

    void Play(const std::string& name);
    void Stop();
    void Pause();
    void Resume();

    void Update(float deltaTime);

    AnimationFrame GetCurrentFrame() const;
    std::string GetCurrentAnimation() const { return m_CurrentAnimation; }

    bool IsPlaying() const { return m_IsPlaying; }
    bool IsLooping() const;
    int GetCurrentFrameIndex() const;
    float GetProgress() const;

    void SetSpeed(float speed) { m_Speed = speed; }
    float GetSpeed() const { return m_Speed; }

    void OnAnimationEnd(const std::function<void(const std::string&)>& callback);

private:
    std::unordered_map<std::string, Animation> m_Animations;
    std::string m_CurrentAnimation;
    bool m_IsPlaying = false;
    float m_Speed = 1.0f;
    std::function<void(const std::string&)> m_OnEndCallback;
};

#endif
