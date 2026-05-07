#include "Animation2D.h"
#include "../Core/Log.h"

Animator2D::Animator2D() {
}

void Animator2D::AddAnimation(const std::string& name, const std::vector<AnimationFrame>& frames,
                               bool loop, bool pingPong) {
    Animation anim;
    anim.Name = name;
    anim.Frames = frames;
    anim.Loop = loop;
    anim.PingPong = pingPong;
    anim.CurrentFrame = 0;
    anim.Timer = 0.0f;
    anim.TotalDuration = 0.0f;
    anim.IsPlaying = false;

    for (const auto& frame : frames) {
        anim.TotalDuration += frame.Duration;
    }

    m_Animations[name] = anim;
}

void Animator2D::AddAnimationFromSheet(const std::string& name, int frameCount,
                                        float frameDuration, int spriteWidth, int spriteHeight,
                                        int sheetWidth, bool loop) {
    std::vector<AnimationFrame> frames;
    frames.reserve(frameCount);

    int cols = sheetWidth / spriteWidth;

    for (int i = 0; i < frameCount; i++) {
        AnimationFrame frame;
        frame.X = (i % cols) * spriteWidth;
        frame.Y = (i / cols) * spriteHeight;
        frame.Width = spriteWidth;
        frame.Height = spriteHeight;
        frame.Duration = frameDuration;
        frames.push_back(frame);
    }

    AddAnimation(name, frames, loop);
}

void Animator2D::Play(const std::string& name) {
    auto it = m_Animations.find(name);
    if (it == m_Animations.end()) {
        ROSE_LOG_WARN("Animation '%s' not found", name.c_str());
        return;
    }

    if (m_CurrentAnimation == name && m_IsPlaying) return;

    m_CurrentAnimation = name;
    m_IsPlaying = true;
    it->second.CurrentFrame = 0;
    it->second.Timer = 0.0f;
}

void Animator2D::Stop() {
    m_IsPlaying = false;
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it != m_Animations.end()) {
        it->second.CurrentFrame = 0;
        it->second.Timer = 0.0f;
    }
}

void Animator2D::Pause() {
    m_IsPlaying = false;
}

void Animator2D::Resume() {
    m_IsPlaying = true;
}

void Animator2D::Update(float deltaTime) {
    if (!m_IsPlaying || m_CurrentAnimation.empty()) return;

    auto it = m_Animations.find(m_CurrentAnimation);
    if (it == m_Animations.end()) return;

    Animation& anim = it->second;
    if (anim.Frames.empty()) return;

    anim.Timer += deltaTime * m_Speed;

    while (anim.Timer >= anim.Frames[anim.CurrentFrame].Duration) {
        anim.Timer -= anim.Frames[anim.CurrentFrame].Duration;
        anim.CurrentFrame++;

        if (anim.CurrentFrame >= (int)anim.Frames.size()) {
            if (anim.Loop) {
                if (anim.PingPong) {
                    std::reverse(anim.Frames.begin(), anim.Frames.end());
                    anim.CurrentFrame = 0;
                } else {
                    anim.CurrentFrame = 0;
                }
            } else {
                anim.CurrentFrame = (int)anim.Frames.size() - 1;
                m_IsPlaying = false;

                if (m_OnEndCallback) {
                    m_OnEndCallback(m_CurrentAnimation);
                }
                return;
            }
        }
    }
}

AnimationFrame Animator2D::GetCurrentFrame() const {
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it == m_Animations.end() || it->second.Frames.empty()) {
        return AnimationFrame{0, 0, 0, 0, 0.0f};
    }
    return it->second.Frames[it->second.CurrentFrame];
}

bool Animator2D::IsLooping() const {
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it == m_Animations.end()) return false;
    return it->second.Loop;
}

int Animator2D::GetCurrentFrameIndex() const {
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it == m_Animations.end()) return 0;
    return it->second.CurrentFrame;
}

float Animator2D::GetProgress() const {
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it == m_Animations.end()) return 0.0f;

    const Animation& anim = it->second;
    if (anim.TotalDuration <= 0.0f || anim.Frames.empty()) return 0.0f;

    float elapsed = anim.Timer;
    for (int i = 0; i < anim.CurrentFrame; i++) {
        elapsed += anim.Frames[i].Duration;
    }

    return elapsed / anim.TotalDuration;
}

void Animator2D::OnAnimationEnd(const std::function<void(const std::string&)>& callback) {
    m_OnEndCallback = callback;
}
