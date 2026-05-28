# ROSE Engine — Detailed Code Review / รีวิวโค้ดอย่างละเอียด

---

## English Version

---

## 1. ARCHITECTURE & CODE STRUCTURE

### 🔴 Critical: `main.cpp` is a Monolithic God Object (main.cpp:65-176)

**Problem**: Initialization, scene setup, rendering, ImGui lifecycle, and cleanup are all in `main()`. The `Core/`, `ECS/`, `Scene/`, `Resource/` directories are empty — they're scaffolding with no implementation. There's no `Application` class, no system/manager orchestration.

**Before** (current):
```cpp
int main() {
    glfwInit();
    // ... window creation ...
    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1; VBO VBO1(...); EBO EBO1(...);
    // ... OpenGL init ...
    // ... ImGui init ...
    while (!glfwWindowShouldClose(window)) {
        glClear(...);
        shaderProgram.Activate();
        camera.Inputs(window);
        camera.Matrix(...);
        Student.Bind();
        VAO1.Bind();
        ImGui_ImplOpenGL3_NewFrame();  // WRONG ORDER
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        MenuBar::Draw();
        glad_glDrawElements(...);      // BUG: should be glDrawElements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(...);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // cleanup...
}
```

**After** (refactored):
```cpp
class Application {
    Window m_Window;
    Renderer& m_Renderer = Renderer::Instance();
    ImGuiLayer m_ImGuiLayer;
    Scene m_ActiveScene;
public:
    Application() { Init(); }
    void Init() {
        m_Window.Create(800, 600, "ROSE");
        m_Renderer.Init();
        m_ImGuiLayer.Init(m_Window.GetHandle());
        m_ActiveScene.Init();
    }
    void Run() {
        while (m_Window.IsOpen()) {
            m_Window.PollEvents();
            m_Renderer.BeginFrame();
            m_ActiveScene.Update(m_Window.GetDeltaTime());
            m_ActiveScene.Render();
            m_ImGuiLayer.Begin();
            MenuBar::Draw();
            m_ImGuiLayer.End();
            m_Window.SwapBuffers();
        }
    }
};
```

### 🔴 Critical: Missing `Rule of Five` on GPU Resource Wrappers (VAO/VBO/EBO/Shader/Texture)

**Problem**: Classes like `VAO`, `VBO`, `EBO`, `Shader`, `Texture` have public `GLuint ID` members, raw pointer-like semantics, and **no** copy/move constructors or deleted copy. Accidental copy will cause double `glDelete*` crashes.

**Before**:
```cpp
class VAO {
public:
    GLuint ID;  // public — any code can mutate it
    VAO();      // glGenVertexArrays
    void Delete();  // glDeleteVertexArrays
    // NO copy/move control
};

void someFunc() {
    VAO a;
    VAO b = a;  // copies ID, both will call Delete() → crash
}
```

**After**:
```cpp
class VAO {
    GLuint m_ID = 0;
public:
    VAO() { glGenVertexArrays(1, &m_ID); }
    ~VAO() { if (m_ID) glDeleteVertexArrays(1, &m_ID); }
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    VAO(VAO&& other) noexcept : m_ID(other.m_ID) { other.m_ID = 0; }
    VAO& operator=(VAO&& other) noexcept {
        if (this != &other) {
            if (m_ID) glDeleteVertexArrays(1, &m_ID);
            m_ID = other.m_ID;
            other.m_ID = 0;
        }
        return *this;
    }
    GLuint GetID() const { return m_ID; }
    void Bind() const { glBindVertexArray(m_ID); }
    static void Unbind() { glBindVertexArray(0); }
};
```

### 🟡 Medium: Empty Directories & Dead Scaffolding

`Core/`, `ECS/`, `Scene/`, `Resource/` are all empty. Remove them or implement the systems they promise. `Resource/ResourceManager.h` is referenced from `TilemapRenderer.cpp:5` but doesn't exist — this will cause a linker error if `TilemapRenderer` is ever instantiated.

### 🟡 Medium: ImGui/ImPlot Internal Includes in Menu Files

Every menu `.cpp` includes `implot_internal.h` and `implot3d_internal.h` which are private headers. These should only be included in ImPlot's own source files. Also, `glad.h` and `glfw3.h` are included in every menu file but never used there.

---

## 2. OPENGL PERFORMANCE & GRAPHICS PIPELINE

### 🔴 Critical: `glad_glDrawElements` is Undefined (main.cpp:153)

```cpp
glad_glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
// ^^^ should be glDrawElements — glad_ prefix is incorrect, likely linker error
```

### 🔴 Critical: FBO Created/Destroyed Every Frame in PostProcess (PostProcess.cpp:48-107)

**Problem**: `ApplyEffects()` calls `glGenFramebuffers` / `glDeleteFramebuffers` + `glGenTextures` / `glDeleteTextures` **every single frame**. This is a massive CPU-GPU sync bottleneck — object creation/deletion should never happen at runtime.

**Before** (PostProcess.cpp:48-52):
```cpp
void PostProcess::ApplyEffects() {
    // ...
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);     // 🔴 Every frame
    unsigned int tempTex;
    glGenTextures(1, &tempTex);     // 🔴 Every frame
```

**After**:
```cpp
class PostProcess {
    unsigned int m_PingFBO = 0, m_PongFBO = 0;
    unsigned int m_PingTex = 0, m_PongTex = 0;
    void CreatePingPongResources();
    // ...
};

void PostProcess::Init(int width, int height) {
    CreateFramebuffer();
    CreatePingPongResources();  // ✨ Create once
    CreateQuad();
}

void PostProcess::ApplyEffects() {
    if (!m_Initialized || m_ActiveEffects.empty()) {
        RenderQuad(m_DefaultShaderID);
        return;
    }
    bool evenPass = true;
    for (auto& effect : m_ActiveEffects) {
        GLuint fbo = evenPass ? m_PingFBO : m_PongFBO;
        GLuint tex = evenPass ? m_PingTex : m_PongTex;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, tex, 0);
        // ... render to fbo ...
        evenPass = !evenPass;
    }
    // Final blit
    RenderQuad(m_DefaultShaderID);
}
```

### 🔴 Critical: `glGetUniformLocation` Called Per-Frame (SpriteBatch.cpp:90, Light2D.cpp:66-101, SpriteShader.cpp:473-577, Camera.cpp:21)

**Problem**: `glGetUniformLocation` is an expensive driver call that should be cached at init time, not called every frame. This appears in:
- `Camera::Matrix()` — called every frame
- `SpriteShader` — every `Set*` method calls `glGetUniformLocation`
- `SpriteBatch::Flush()` — called per-flush
- `LightManager::UploadLightsToShader()` — called every frame, with string concatenation for uniform names

**Before** (SpriteShader.cpp:473):
```cpp
void SpriteShader::SetProjection(const glm::mat4& projection) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Projection"); // 🔴
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
}
```

**After**:
```cpp
class SpriteShader {
    struct UniformCache {
        GLint Projection = -1;
        GLint View = -1;
        GLint Color = -1;
        // ...
    };
    UniformCache m_Cache[(int)SpriteShaderType::CRT + 1];
    
    void CacheUniforms(SpriteShaderType type) {
        unsigned int id = m_Shaders[(int)type];
        auto& c = m_Cache[(int)type];
        c.Projection = glGetUniformLocation(id, "u_Projection");
        c.View = glGetUniformLocation(id, "u_View");
        c.Color = glGetUniformLocation(id, "u_Color");
        // ... cache all at init time ...
    }
};

void SpriteShader::SetProjection(const glm::mat4& projection) {
    auto& c = m_Cache[(int)m_CurrentType];
    if (c.Projection != -1)
        glUniformMatrix4fv(c.Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
```

### 🟡 Medium: Render Loop ImGui Frame Timing is Wrong (main.cpp:138-156)

ImGui `NewFrame()` is called **after** the scene is already cleared and rendered. This means ImGui's internal input state and delta-time are captured at the wrong point. The standard pattern is: `NewFrame` → build UI → `Render` → `RenderDrawData`.

**Current order**:
```
glClear → shader.Activate → camera.Inputs → camera.Matrix → VAO1.Bind → NewFrame → Draw → glDrawElements → Render → SwapBuffers
```

**Correct order**:
```
NewFrame → camera.Inputs → build UI → glClear → shader.Activate → camera.Matrix → VAO1.Bind → glDrawElements → Render → RenderDrawData → SwapBuffers
```

### 🟡 Medium: No Batching in TilemapRenderer (TilemapRenderer.cpp:130-141)

Every tilemap render call re-uploads all vertex data with `glBufferSubData` even if nothing changed. Use a dirty flag (already have `m_NeedsRebuild`) to only re-upload when tiles actually change.

### 🟡 Medium: Particle System Uses Wrong Primitive (ParticleSystem.cpp:207)

```cpp
glDrawArrays(GL_TRIANGLES, 0, activeCount * 4);
```
This draws `activeCount * 4` vertices as triangles, but the data contains quads (4 vertices per particle). Since no index buffer is bound, this will render triangles `[0,1,2], [1,2,3]` etc. only if GL doesn't treat it as raw triangles. This is wrong — should be `activeCount * 6` or use an index buffer. Better: use `GL_TRIANGLES` with 6 indices per quad, or use geometry shader instancing.

### 🟢 Info: Shader String Comparison Bug (ShaderClass.cpp:85)

```cpp
if (type != "PROGRAM")  // compares pointer, NOT string content!
```
Use `strcmp(type, "PROGRAM") != 0` or `std::string_view` instead.

---

## 3. DEAR IMGUI OPTIMIZATION

### 🟡 Medium: Heavy Computation Inside ImGui Begin/End (tools_menu.cpp:87-92)

**Problem**: Inside `ImGui::Begin` / `ImGui::End`, you have a loop that scans all 120 FPS samples to compute min/max every frame. This stalls the UI drawing.

**Before** (tools_menu.cpp:86-101):
```cpp
ImGui::SeparatorText("FPS History");
{
    float max_fps = 0.0f;  // ← computed inside UI block
    float min_fps = FLT_MAX;
    for (int i = 0; i < 120; i++) {
        if (fps_history[i] > max_fps) max_fps = fps_history[i];
        if (fps_history[i] < min_fps) min_fps = fps_history[i];
    }
    ImGui::PlotLines("##fps", fps_history, ...);
}
```

**After** (cache min/max at sample time):
```cpp
void ToolMenu::DrawWindow() {
    static float fps_history[120] = {};
    static int fps_offset = 0;
    static float cached_min = 0.0f, cached_max = 200.0f;
    static double fps_refresh_time = 0.0;
    double now = ImGui::GetTime();

    if (now - fps_refresh_time >= 1.0 / 60.0) {
        float fps = ImGui::GetIO().Framerate;
        fps_history[fps_offset] = fps;
        fps_offset = (fps_offset + 1) % 120;
        fps_refresh_time = now;

        // Update cached extents here (once per sample, not per UI frame)
        cached_min = FLT_MAX; cached_max = 0.0f;
        for (int i = 0; i < 120; i++) {
            if (fps_history[i] > cached_max) cached_max = fps_history[i];
            if (fps_history[i] < cached_min) cached_min = fps_history[i];
        }
    }

    // ... ImGui::Begin ...
    ImGui::PlotLines("##fps", fps_history, 120, fps_offset,
                     nullptr, cached_min, cached_max, ImVec2(-1, 70));
}
```

### 🟢 Info: DLL Loading in ImGui (main.cpp:99-108)

ImPlot3D's `CreateContext` is called but there's no guarantee the DLL/shared library is linked correctly. Verify that `implot3d` is properly linked.

---

## 4. ACTIONABLE RECOMMENDATIONS SUMMARY

| Priority | Issue | File | Fix |
|----------|-------|------|-----|
| 🔴 **CRITICAL** | `glad_glDrawElements` typo | main.cpp:153 | Change to `glDrawElements` |
| 🔴 **CRITICAL** | FBO alloc/dealloc every frame | PostProcess.cpp:48-107 | Pre-allocate ping-pong FBOs |
| 🔴 **CRITICAL** | `glGetUniformLocation` per-frame | Camera, SpriteShader, Light2D, SpriteBatch | Cache uniform locations at init |
| 🔴 **CRITICAL** | Missing Rule of Five on GPU objects | VAO/VBO/EBO/Shader/Texture | Add move semantics, delete copy |
| 🔴 **CRITICAL** | ImGui NewFrame after scene render | main.cpp:147-156 | Reorder: NewFrame → draw → Render |
| 🟡 **HIGH** | `get_file_contents` throws int | ShaderClass.cpp:18 | Throw `std::runtime_error` |
| 🟡 **HIGH** | `compileErrors` pointer comparison | ShaderClass.cpp:85 | Use `strcmp` or `std::string` |
| 🟡 **HIGH** | Particle draw primitive wrong | ParticleSystem.cpp:207 | Use index buffer for quads |
| 🟡 **HIGH** | Mesh upload every frame | TilemapRenderer.cpp:130-141 | Only upload on dirty flag |
| 🟡 **HIGH** | Public `ID` fields | VAO/VBO/EBO/Shader/Texture | Make private with getters |
| 🟡 **HIGH** | Heavy loop in ImGui block | tools_menu.cpp:87-92 | Cache min/max at sample time |
| 🟢 **LOW** | Redundant includes in menu files | *_menu.cpp | Remove glad/glfw/internal includes |
| 🟢 **LOW** | Empty scaffolding directories | Core/, ECS/, Scene/, Resource/ | Implement or remove |
| 🟢 **LOW** | `GL_DYNAMIC_DRAW` for static data | VBO.cpp:9 | Use `GL_STATIC_DRAW` for static geometry |

### Instant Fixes (can apply immediately):

1. **main.cpp:153** — Fix the draw call name
2. **ShaderClass.cpp:85** — Fix string comparison
3. **main.cpp:147-156** — Move `NewFrame` before scene render
4. **PostProcess.cpp:48-52** — Move FBO creation to `Init()`, reuse
5. **SpriteShader.cpp + Light2D.cpp** — Cache uniform locations once in `Init()`

---

## ภาษาไทย (Thai Version)

---

## 1. โครงสร้างสถาปัตยกรรมและโค้ด

### 🔴 วิกฤต: `main.cpp` เป็น God Object ขนาดใหญ่ (main.cpp:65-176)

**ปัญหา**: การเริ่มต้นระบบ, การตั้งค่า Scene, การเรนเดอร์, วงจรชีวิต ImGui, และการทำความสะอาดทั้งหมดอยู่ใน `main()` เดียว `Core/`, `ECS/`, `Scene/`, `Resource/` ล้วนว่างเปล่า — เป็นแค่โครงสร้างที่ไม่มีอะไรเลย ไม่มีคลาส `Application`, ไม่มีระบบจัดการใดๆ

**ก่อน** (ปัจจุบัน):
```cpp
int main() {
    glfwInit();
    // ... สร้าง window ...
    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1; VBO VBO1(...); EBO EBO1(...);
    // ... เริ่มต้น OpenGL ...
    // ... เริ่มต้น ImGui ...
    while (!glfwWindowShouldClose(window)) {
        glClear(...);
        shaderProgram.Activate();
        camera.Inputs(window);
        camera.Matrix(...);
        Student.Bind();
        VAO1.Bind();
        ImGui_ImplOpenGL3_NewFrame();  // ลำดับผิด!
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        MenuBar::Draw();
        glad_glDrawElements(...);      // บั๊ก: ควรเป็น glDrawElements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(...);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ทำความสะอาด...
}
```

**หลัง** (ปรับปรุงแล้ว):
```cpp
class Application {
    Window m_Window;
    Renderer& m_Renderer = Renderer::Instance();
    ImGuiLayer m_ImGuiLayer;
    Scene m_ActiveScene;
public:
    Application() { Init(); }
    void Init() {
        m_Window.Create(800, 600, "ROSE");
        m_Renderer.Init();
        m_ImGuiLayer.Init(m_Window.GetHandle());
        m_ActiveScene.Init();
    }
    void Run() {
        while (m_Window.IsOpen()) {
            m_Window.PollEvents();
            m_Renderer.BeginFrame();
            m_ActiveScene.Update(m_Window.GetDeltaTime());
            m_ActiveScene.Render();
            m_ImGuiLayer.Begin();
            MenuBar::Draw();
            m_ImGuiLayer.End();
            m_Window.SwapBuffers();
        }
    }
};
```

### 🔴 วิกฤต: ไม่มี `Rule of Five` ในคลาส GPU Resource (VAO/VBO/EBO/Shader/Texture)

**ปัญหา**: คลาสเช่น `VAO`, `VBO`, `EBO`, `Shader`, `Texture` มีสมาชิก `GLuint ID` แบบ public, มีความหมายเหมือน raw pointer, และ **ไม่มี** copy/move constructor หรือ deleted copy การ copy โดยไม่ตั้งใจจะทำให้ `glDelete*` ถูกเรียกสองครั้งจนโปรแกรม Crash

**ก่อน**:
```cpp
class VAO {
public:
    GLuint ID;  // public — โค้ดอื่นแก้ไขได้
    VAO();      // glGenVertexArrays
    void Delete();  // glDeleteVertexArrays
    // ไม่มีการควบคุม copy/move
};

void someFunc() {
    VAO a;
    VAO b = a;  // คัดลอก ID, ทั้งคู่จะเรียก Delete() → crash
}
```

**หลัง**:
```cpp
class VAO {
    GLuint m_ID = 0;
public:
    VAO() { glGenVertexArrays(1, &m_ID); }
    ~VAO() { if (m_ID) glDeleteVertexArrays(1, &m_ID); }
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    VAO(VAO&& other) noexcept : m_ID(other.m_ID) { other.m_ID = 0; }
    VAO& operator=(VAO&& other) noexcept {
        if (this != &other) {
            if (m_ID) glDeleteVertexArrays(1, &m_ID);
            m_ID = other.m_ID;
            other.m_ID = 0;
        }
        return *this;
    }
    GLuint GetID() const { return m_ID; }
    void Bind() const { glBindVertexArray(m_ID); }
    static void Unbind() { glBindVertexArray(0); }
};
```

### 🟡 ปานกลาง: ไดเรกทอรีว่างเปล่าและ Scaffolding ที่ตายแล้ว

`Core/`, `ECS/`, `Scene/`, `Resource/` ล้วนว่างเปล่า ควรลบหรือ implement ระบบที่สัญญาไว้ `Resource/ResourceManager.h` ถูก引用จาก `TilemapRenderer.cpp:5` แต่ไม่มีอยู่จริง — จะทำให้เกิด linker error ถ้า `TilemapRenderer` ถูก instantiate

### 🟡 ปานกลาง: ImGui/ImPlot Internal Includes ใน Menu Files

ทุก menu `.cpp` รวม `implot_internal.h` และ `implot3d_internal.h` ซึ่งเป็นส่วนหัวส่วนตัว ควรรวมเฉพาะใน source files ของ ImPlot เท่านั้น นอกจากนี้ `glad.h` และ `glfw3.h` ถูกรวมในทุก menu file แต่ไม่ได้ใช้งาน

---

## 2. ประสิทธิภาพ OpenGL และ Graphics Pipeline

### 🔴 วิกฤต: `glad_glDrawElements` ไม่ได้ถูกนิยาม (main.cpp:153)

```cpp
glad_glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
// ^^^ ควรเป็น glDrawElements — คำนำหน้า glad_ ผิด, อาจเกิด linker error
```

### 🔴 วิกฤต: FBO ถูกสร้าง/ทำลายทุก Frame ใน PostProcess (PostProcess.cpp:48-107)

**ปัญหา**: `ApplyEffects()` เรียก `glGenFramebuffers` / `glDeleteFramebuffers` + `glGenTextures` / `glDeleteTextures` **ทุก frame** นี่คือ CPU-GPU sync bottleneck ขนาดใหญ่ — ไม่ควรสร้าง/ลบ object ใน runtime

**ก่อน** (PostProcess.cpp:48-52):
```cpp
void PostProcess::ApplyEffects() {
    // ...
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);     // 🔴 ทุก frame
    unsigned int tempTex;
    glGenTextures(1, &tempTex);     // 🔴 ทุก frame
```

**หลัง**:
```cpp
class PostProcess {
    unsigned int m_PingFBO = 0, m_PongFBO = 0;
    unsigned int m_PingTex = 0, m_PongTex = 0;
    void CreatePingPongResources();
    // ...
};

void PostProcess::Init(int width, int height) {
    CreateFramebuffer();
    CreatePingPongResources();  // ✨ สร้างครั้งเดียว
    CreateQuad();
}

void PostProcess::ApplyEffects() {
    if (!m_Initialized || m_ActiveEffects.empty()) {
        RenderQuad(m_DefaultShaderID);
        return;
    }
    bool evenPass = true;
    for (auto& effect : m_ActiveEffects) {
        GLuint fbo = evenPass ? m_PingFBO : m_PongFBO;
        GLuint tex = evenPass ? m_PingTex : m_PongTex;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, tex, 0);
        // ... เรนเดอร์ไปที่ fbo ...
        evenPass = !evenPass;
    }
    // Final blit
    RenderQuad(m_DefaultShaderID);
}
```

### 🔴 วิกฤต: `glGetUniformLocation` ถูกเรียกทุก Frame (SpriteBatch.cpp:90, Light2D.cpp:66-101, SpriteShader.cpp:473-577, Camera.cpp:21)

**ปัญหา**: `glGetUniformLocation` เป็นการเรียก driver ที่มีราคาแพง ควร cache ไว้ตั้งแต่ init ไม่ใช่เรียกทุก frame โดยปรากฏใน:
- `Camera::Matrix()` — เรียกทุก frame
- `SpriteShader` — ทุก method `Set*` เรียก `glGetUniformLocation`
- `SpriteBatch::Flush()` — เรียกทุก flush
- `LightManager::UploadLightsToShader()` — เรียกทุก frame พร้อม string concatenation สำหรับชื่อ uniform

**ก่อน** (SpriteShader.cpp:473):
```cpp
void SpriteShader::SetProjection(const glm::mat4& projection) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Projection"); // 🔴
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
}
```

**หลัง**:
```cpp
class SpriteShader {
    struct UniformCache {
        GLint Projection = -1;
        GLint View = -1;
        GLint Color = -1;
        // ...
    };
    UniformCache m_Cache[(int)SpriteShaderType::CRT + 1];
    
    void CacheUniforms(SpriteShaderType type) {
        unsigned int id = m_Shaders[(int)type];
        auto& c = m_Cache[(int)type];
        c.Projection = glGetUniformLocation(id, "u_Projection");
        c.View = glGetUniformLocation(id, "u_View");
        c.Color = glGetUniformLocation(id, "u_Color");
        // ... cache ทั้งหมดตอน init ...
    }
};

void SpriteShader::SetProjection(const glm::mat4& projection) {
    auto& c = m_Cache[(int)m_CurrentType];
    if (c.Projection != -1)
        glUniformMatrix4fv(c.Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
```

### 🟡 ปานกลาง: ลำดับ Frame ImGui ใน Render Loop ผิด (main.cpp:138-156)

ImGui `NewFrame()` ถูกเรียก **หลัง** จากที่ scene ถูกล้างและเรนเดอร์ไปแล้ว ทำให้ input state และ delta-time ของ ImGui ถูกจับภาพผิดจุด รูปแบบที่ถูกต้องคือ: `NewFrame` → build UI → `Render` → `RenderDrawData`

**ลำดับปัจจุบัน**:
```
glClear → shader.Activate → camera.Inputs → camera.Matrix → VAO1.Bind → NewFrame → Draw → glDrawElements → Render → SwapBuffers
```

**ลำดับที่ถูกต้อง**:
```
NewFrame → camera.Inputs → build UI → glClear → shader.Activate → camera.Matrix → VAO1.Bind → glDrawElements → Render → RenderDrawData → SwapBuffers
```

### 🟡 ปานกลาง: ไม่มี Batching ใน TilemapRenderer (TilemapRenderer.cpp:130-141)

ทุกครั้งที่เรนเดอร์ tilemap จะอัปโหลด vertex data ทั้งหมดใหม่ด้วย `glBufferSubData` แม้ไม่มีอะไรเปลี่ยนแปลง ควรใช้ dirty flag (มี `m_NeedsRebuild` อยู่แล้ว) เพื่ออัปโหลดเฉพาะเมื่อ tiles เปลี่ยนจริง

### 🟡 ปานกลาง: Particle System ใช้ Primitive ผิด (ParticleSystem.cpp:207)

```cpp
glDrawArrays(GL_TRIANGLES, 0, activeCount * 4);
```
 draws `activeCount * 4` vertices เป็น triangles แต่ข้อมูลเป็น quads (4 vertices ต่อ particle) เนื่องจากไม่มี index buffer ผูกอยู่ จะ render เป็น triangles `[0,1,2], [1,2,3]` ฯลฯ เท่านั้นถ้า GL ไม่ได้ treat เป็น raw triangles ซึ่งผิด ควรเป็น `activeCount * 6` หรือใช้ index buffer หรือใช้ geometry shader instancing

### 🟢 ข้อมูล: บั๊กการเปรียบเทียบ String ใน Shader (ShaderClass.cpp:85)

```cpp
if (type != "PROGRAM")  // เปรียบเทียบ pointer, NOT string content!
```
ให้ใช้ `strcmp(type, "PROGRAM") != 0` หรือ `std::string_view` แทน

---

## 3. การปรับแต่ง Dear ImGui

### 🟡 ปานกลาง: การคำนวณหนักภายใน ImGui Begin/End (tools_menu.cpp:87-92)

**ปัญหา**: ภายใน `ImGui::Begin` / `ImGui::End` มี loop ที่สแกน FPS samples ทั้ง 120 ตัวเพื่อหา min/max ทุก frame ทำให้ UI drawing หน่วง

**ก่อน** (tools_menu.cpp:86-101):
```cpp
ImGui::SeparatorText("FPS History");
{
    float max_fps = 0.0f;  // ← คำนวณภายใน UI block
    float min_fps = FLT_MAX;
    for (int i = 0; i < 120; i++) {
        if (fps_history[i] > max_fps) max_fps = fps_history[i];
        if (fps_history[i] < min_fps) min_fps = fps_history[i];
    }
    ImGui::PlotLines("##fps", fps_history, ...);
}
```

**หลัง** (cache min/max ตอนบันทึก sample):
```cpp
void ToolMenu::DrawWindow() {
    static float fps_history[120] = {};
    static int fps_offset = 0;
    static float cached_min = 0.0f, cached_max = 200.0f;
    static double fps_refresh_time = 0.0;
    double now = ImGui::GetTime();

    if (now - fps_refresh_time >= 1.0 / 60.0) {
        float fps = ImGui::GetIO().Framerate;
        fps_history[fps_offset] = fps;
        fps_offset = (fps_offset + 1) % 120;
        fps_refresh_time = now;

        // อัปเดต cached extents ที่นี่ (ครั้งเดียวต่อ sample, ไม่ใช่ทุก UI frame)
        cached_min = FLT_MAX; cached_max = 0.0f;
        for (int i = 0; i < 120; i++) {
            if (fps_history[i] > cached_max) cached_max = fps_history[i];
            if (fps_history[i] < cached_min) cached_min = fps_history[i];
        }
    }

    // ... ImGui::Begin ...
    ImGui::PlotLines("##fps", fps_history, 120, fps_offset,
                     nullptr, cached_min, cached_max, ImVec2(-1, 70));
}
```

### 🟢 ข้อมูล: การโหลด DLL ใน ImGui (main.cpp:99-108)

ImPlot3D's `CreateContext` ถูกเรียกแต่ไม่มีการรับประกันว่า DLL/shared library ถูก link อย่างถูกต้อง ควรตรวจสอบว่า `implot3d` link อย่างถูกต้อง

---

## 4. สรุปคำแนะนำที่ดำเนินการได้

| ความสำคัญ | ปัญหา | ไฟล์ | วิธีแก้ |
|----------|-------|------|--------|
| 🔴 **วิกฤต** | `glad_glDrawElements` พิมพ์ผิด | main.cpp:153 | เปลี่ยนเป็น `glDrawElements` |
| 🔴 **วิกฤต** | FBO สร้าง/ลบทุก frame | PostProcess.cpp:48-107 | จอง ping-pong FBOs ล่วงหน้า |
| 🔴 **วิกฤต** | `glGetUniformLocation` ทุก frame | Camera, SpriteShader, Light2D, SpriteBatch | Cache uniform locations ตอน init |
| 🔴 **วิกฤต** | ไม่มี Rule of Five บน GPU objects | VAO/VBO/EBO/Shader/Texture | เพิ่ม move semantics, ลบ copy |
| 🔴 **วิกฤต** | ImGui NewFrame หลัง scene render | main.cpp:147-156 | เรียงใหม่: NewFrame → draw → Render |
| 🟡 **สูง** | `get_file_contents` throw int | ShaderClass.cpp:18 | เปลี่ยนเป็น `std::runtime_error` |
| 🟡 **สูง** | `compileErrors` เปรียบเทียบ pointer | ShaderClass.cpp:85 | ใช้ `strcmp` หรือ `std::string` |
| 🟡 **สูง** | Particle draw primitive ผิด | ParticleSystem.cpp:207 | ใช้ index buffer สำหรับ quads |
| 🟡 **สูง** | อัปโหลด Mesh ทุก frame | TilemapRenderer.cpp:130-141 | อัปโหลดเฉพาะเมื่อ dirty flag |
| 🟡 **สูง** | ฟิลด์ `ID` เป็น public | VAO/VBO/EBO/Shader/Texture | ทำให้เป็น private พร้อม getters |
| 🟡 **สูง** | Loop หนักใน ImGui block | tools_menu.cpp:87-92 | Cache min/max ตอน sample time |
| 🟢 **ต่ำ** | Includes ซ้ำซ้อนใน menu files | *_menu.cpp | ลบ glad/glfw/internal includes |
| 🟢 **ต่ำ** | ไดเรกทอรี Scaffolding ว่างเปล่า | Core/, ECS/, Scene/, Resource/ | Implement หรือลบ |
| 🟢 **ต่ำ** | `GL_DYNAMIC_DRAW` สำหรับ static data | VBO.cpp:9 | ใช้ `GL_STATIC_DRAW` สำหรับ static geometry |

### วิธีแก้ที่ทำได้ทันที:

1. **main.cpp:153** — แก้ชื่อ draw call
2. **ShaderClass.cpp:85** — แก้การเปรียบเทียบ string
3. **main.cpp:147-156** — ย้าย `NewFrame` ก่อน scene render
4. **PostProcess.cpp:48-52** — ย้าย FBO creation ไป `Init()`, นำกลับมาใช้ใหม่
5. **SpriteShader.cpp + Light2D.cpp** — Cache uniform locations ครั้งเดียวใน `Init()`
