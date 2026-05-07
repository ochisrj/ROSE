#ifndef SPRITE_VERT
#define SPRITE_VERT R"(
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexSlot;
layout (location = 3) in float aDepthSortY;

out vec2 v_TexCoord;
out float v_TexSlot;
out float v_DepthSortY;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
    v_TexCoord = aTexCoord;
    v_TexSlot = aTexSlot;
    v_DepthSortY = aDepthSortY;
    vec4 worldPos = u_View * vec4(aPos, 0.0, 1.0);
    gl_Position = u_Projection * worldPos;
}
)"
#endif

#ifndef SPRITE_FRAG
#define SPRITE_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform vec4 u_Color = vec4(1.0);
uniform float u_Time = 0.0;

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);
    FragColor = texColor * u_Color;
    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef NORMALMAP_FRAG
#define NORMALMAP_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform sampler2D u_NormalMap;
uniform vec2 u_LightPos;
uniform vec3 u_LightColor = vec3(1.0);
uniform float u_LightIntensity = 1.0;
uniform float u_LightRange = 10.0;
uniform vec3 u_AmbientColor = vec3(0.1);
uniform vec4 u_Color = vec4(1.0);

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);

    vec2 normalMap = texture(u_NormalMap, v_TexCoord).rg * 2.0 - 1.0;
    vec3 normal = normalize(vec3(normalMap, 1.0));

    vec2 fragPos = vec2(
        gl_FragCoord.x / 1920.0,
        gl_FragCoord.y / 1080.0
    );
    vec2 lightDir = normalize(u_LightPos - fragPos);
    float dist = length(u_LightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 2.0 * dist * dist / (u_LightRange * u_LightRange));

    float diff = max(dot(normal, vec3(lightDir, 0.0)), 0.0);
    vec3 lighting = u_AmbientColor + u_LightColor * diff * u_LightIntensity * attenuation;

    FragColor = vec4(texColor.rgb * lighting, texColor.a) * u_Color;
    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef OUTLINE_FRAG
#define OUTLINE_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform vec3 u_OutlineColor = vec3(0.0);
uniform float u_OutlineWidth = 2.0;
uniform vec2 u_TexelSize = vec2(1.0/512.0, 1.0/512.0);
uniform vec4 u_Color = vec4(1.0);

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);

    float outline = 0.0;
    for (int x = -int(u_OutlineWidth); x <= int(u_OutlineWidth); x++) {
        for (int y = -int(u_OutlineWidth); y <= int(u_OutlineWidth); y++) {
            vec2 offset = vec2(float(x), float(y)) * u_TexelSize;
            outline += texture(u_Textures[int(v_TexSlot)], v_TexCoord + offset).a;
        }
    }

    bool isEdge = (outline > 0.0 && texColor.a == 0.0);

    if (isEdge) {
        FragColor = vec4(u_OutlineColor, 1.0);
    } else {
        FragColor = texColor * u_Color;
    }

    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef GLOW_FRAG
#define GLOW_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform vec3 u_GlowColor = vec3(1.0, 1.0, 0.5);
uniform float u_GlowIntensity = 1.0;
uniform vec2 u_TexelSize = vec2(1.0/512.0, 1.0/512.0);
uniform vec4 u_Color = vec4(1.0);

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);

    float glow = 0.0;
    int radius = 4;
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            vec2 offset = vec2(float(x), float(y)) * u_TexelSize;
            glow += texture(u_Textures[int(v_TexSlot)], v_TexCoord + offset).a;
        }
    }
    glow /= float((radius * 2 + 1) * (radius * 2 + 1));

    vec3 finalColor = texColor.rgb + u_GlowColor * glow * u_GlowIntensity;
    FragColor = vec4(finalColor, texColor.a) * u_Color;
    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef FLASH_FRAG
#define FLASH_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform float u_FlashIntensity = 0.0;
uniform vec3 u_FlashColor = vec3(1.0, 1.0, 1.0);
uniform vec4 u_Color = vec4(1.0);

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);
    vec3 finalColor = mix(texColor.rgb, u_FlashColor, u_FlashIntensity);
    FragColor = vec4(finalColor, texColor.a) * u_Color;
    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef DISSOLVE_FRAG
#define DISSOLVE_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
in float v_TexSlot;
in float v_DepthSortY;

out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform sampler2D u_NoiseTexture;
uniform float u_DissolveProgress = 0.0;
uniform vec3 u_DissolveColor = vec3(1.0, 0.5, 0.0);
uniform vec4 u_Color = vec4(1.0);

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexSlot)], v_TexCoord);
    float noise = texture(u_NoiseTexture, v_TexCoord).r;

    if (noise < u_DissolveProgress) {
        discard;
    }

    float edge = smoothstep(u_DissolveProgress, u_DissolveProgress + 0.05, noise);
    vec3 finalColor = mix(u_DissolveColor, texColor.rgb, edge);
    FragColor = vec4(finalColor, texColor.a) * u_Color;
    if (FragColor.a < 0.01) discard;
}
)"
#endif

#ifndef POSTPROCESS_VERT
#define POSTPROCESS_VERT R"(
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)"
#endif

#ifndef BLOOM_FRAG
#define BLOOM_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_Threshold = 0.5;
uniform float u_Intensity = 0.8;
uniform vec2 u_TexelSize = vec2(1.0/1920.0, 1.0/1080.0);
uniform int u_BlurPass = 0;

vec3 Blur(sampler2D tex, vec2 uv, vec2 texelSize, int samples) {
    vec3 result = vec3(0.0);
    float total = 0.0;
    for (int x = -samples; x <= samples; x++) {
        for (int y = -samples; y <= samples; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float weight = 1.0 / (1.0 + float(x*x + y*y));
            result += texture(tex, uv + offset).rgb * weight;
            total += weight;
        }
    }
    return result / total;
}

void main() {
    vec4 color = texture(u_SceneTexture, v_TexCoord);

    if (length(color.rgb) < u_Threshold) {
        FragColor = color;
        return;
    }

    vec3 bloom = Blur(u_SceneTexture, v_TexCoord, u_TexelSize * 2.0, 2);
    bloom *= u_Intensity;

    FragColor = vec4(color.rgb + bloom, color.a);
}
)"
#endif

#ifndef VIGNETTE_FRAG
#define VIGNETTE_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_Strength = 0.4;
uniform float u_Smoothness = 0.5;
uniform vec3 u_VignetteColor = vec3(0.0);

void main() {
    vec4 color = texture(u_SceneTexture, v_TexCoord);

    vec2 center = v_TexCoord - 0.5;
    float dist = length(center);
    float vignette = smoothstep(0.8, 0.8 - u_Strength * u_Smoothness, dist);

    color.rgb = mix(color.rgb, u_VignetteColor, vignette);
    FragColor = color;
}
)"
#endif

#ifndef CRT_FRAG
#define CRT_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_ScanlineStrength = 0.3;
uniform float u_Curvature = 0.02;
uniform float u_NoiseAmount = 0.05;
uniform float u_Time = 0.0;

float Noise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = v_TexCoord;

    uv -= 0.5;
    float dist = length(uv);
    uv += uv * dist * dist * u_Curvature;
    uv += 0.5;

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        FragColor = vec4(0.0);
        return;
    }

    vec3 color = texture(u_SceneTexture, uv).rgb;

    float scanline = sin(uv.y * 1080.0 * 3.14159) * 0.5 + 0.5;
    color *= 1.0 - u_ScanlineStrength * (1.0 - scanline);

    float noise = Noise(uv * u_Time * 100.0) * u_NoiseAmount;
    color += noise - u_NoiseAmount * 0.5;

    FragColor = vec4(color, 1.0);
}
)"
#endif

#ifndef COLORGRADING_FRAG
#define COLORGRADING_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_Brightness = 1.0;
uniform float u_Contrast = 1.0;
uniform float u_Saturation = 1.0;

void main() {
    vec3 color = texture(u_SceneTexture, v_TexCoord).rgb;

    color *= u_Brightness;

    color = (color - 0.5) * u_Contrast + 0.5;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(gray), color, u_Saturation);

    FragColor = vec4(color, 1.0);
}
)"
#endif

#ifndef PIXELATE_FRAG
#define PIXELATE_FRAG R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_PixelSize = 4.0;
uniform vec2 u_Resolution = vec2(1920.0, 1080.0);

void main() {
    vec2 texelSize = u_PixelSize / u_Resolution;
    vec2 uv = floor(v_TexCoord / texelSize) * texelSize;
    FragColor = texture(u_SceneTexture, uv);
}
)"
#endif

#include <glad/glad.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "SpriteShader.h"
#include "../Core/Log.h"

SpriteShader& SpriteShader::Instance() {
    static SpriteShader instance;
    return instance;
}

std::string SpriteShader::ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

unsigned int SpriteShader::CompileShader(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        ROSE_LOG_ERROR("Vertex shader compilation failed: %s", infoLog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        ROSE_LOG_ERROR("Fragment shader compilation failed: %s", infoLog);
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        ROSE_LOG_ERROR("Shader program linking failed: %s", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void SpriteShader::Init() {
    m_Shaders[(int)SpriteShaderType::Default2D] = CompileShader(SPRITE_VERT, SPRITE_FRAG);
    m_Shaders[(int)SpriteShaderType::NormalMap] = CompileShader(SPRITE_VERT, NORMALMAP_FRAG);
    m_Shaders[(int)SpriteShaderType::Outline] = CompileShader(SPRITE_VERT, OUTLINE_FRAG);
    m_Shaders[(int)SpriteShaderType::Glow] = CompileShader(SPRITE_VERT, GLOW_FRAG);
    m_Shaders[(int)SpriteShaderType::Flash] = CompileShader(SPRITE_VERT, FLASH_FRAG);
    m_Shaders[(int)SpriteShaderType::Dissolve] = CompileShader(SPRITE_VERT, DISSOLVE_FRAG);

    ROSE_LOG_INFO("SpriteShader initialized with 6 shader variants");
}

unsigned int SpriteShader::GetShaderID(SpriteShaderType type) const {
    return m_Shaders[(int)type];
}

void SpriteShader::UseShader(SpriteShaderType type) {
    m_CurrentShader = m_Shaders[(int)type];
    glUseProgram(m_CurrentShader);
}

void SpriteShader::SetProjection(const glm::mat4& projection) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Projection");
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
}

void SpriteShader::SetView(const glm::mat4& view) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_View");
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}

void SpriteShader::SetColor(glm::vec4 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Color");
    if (loc != -1) glUniform4f(loc, color.r, color.g, color.b, color.a);
}

void SpriteShader::SetLightPos(glm::vec2 pos) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_LightPos");
    if (loc != -1) glUniform2f(loc, pos.x, pos.y);
}

void SpriteShader::SetLightColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_LightColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetLightIntensity(float intensity) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_LightIntensity");
    if (loc != -1) glUniform1f(loc, intensity);
}

void SpriteShader::SetNormalMapEnabled(bool enabled) {
}

void SpriteShader::SetOutlineColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_OutlineColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetOutlineWidth(float width) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_OutlineWidth");
    if (loc != -1) glUniform1f(loc, width);
}

void SpriteShader::SetGlowColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_GlowColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetGlowIntensity(float intensity) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_GlowIntensity");
    if (loc != -1) glUniform1f(loc, intensity);
}

void SpriteShader::SetFlashIntensity(float intensity) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_FlashIntensity");
    if (loc != -1) glUniform1f(loc, intensity);
}

void SpriteShader::SetFlashColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_FlashColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetDissolveProgress(float progress) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_DissolveProgress");
    if (loc != -1) glUniform1f(loc, progress);
}

void SpriteShader::SetDissolveColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_DissolveColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetDissolveNoiseTexture(unsigned int id) {
}

void SpriteShader::SetVignetteStrength(float strength) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_VignetteStrength");
    if (loc != -1) glUniform1f(loc, strength);
}

void SpriteShader::SetVignetteColor(glm::vec3 color) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_VignetteColor");
    if (loc != -1) glUniform3f(loc, color.r, color.g, color.b);
}

void SpriteShader::SetCRTScanlineStrength(float strength) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_ScanlineStrength");
    if (loc != -1) glUniform1f(loc, strength);
}

void SpriteShader::SetCRTCurvature(float curvature) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Curvature");
    if (loc != -1) glUniform1f(loc, curvature);
}

void SpriteShader::SetTextureUnit(int unit) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_Textures");
    if (loc != -1) glUniform1iv(loc, 1, &unit);
}

void SpriteShader::SetNormalMapUnit(int unit) {
    GLint loc = glGetUniformLocation(m_CurrentShader, "u_NormalMap");
    if (loc != -1) glUniform1i(loc, unit);
}
