#ifndef TIME_H
#define TIME_H

class Time {
public:
    static void Init();
    static void Update();

    static float GetDeltaTime() { return s_DeltaTime; }
    static float GetTime() { return s_Time; }
    static float GetFixedDeltaTime() { return s_FixedDeltaTime; }
    static int GetFrameCount() { return s_FrameCount; }
    static float GetFPS() { return s_FPS; }

    static bool ShouldFixedUpdate();

private:
    static float s_DeltaTime;
    static float s_LastTime;
    static float s_Time;
    static float s_FixedDeltaTime;
    static float s_FixedAccumulator;
    static int s_FrameCount;
    static float s_FPS;
    static float s_FPSUpdateTimer;
};

#endif
