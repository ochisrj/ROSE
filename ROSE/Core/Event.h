#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <any>

class Event {
public:
    using Listener = std::function<void(const std::any&)>;

    template<typename T>
    static void Subscribe(const std::string& eventType, std::function<void(const T&)> listener) {
        auto wrapper = [listener](const std::any& data) {
            if (data.has_value()) {
                try {
                    listener(std::any_cast<const T&>(data));
                } catch (...) {}
            }
        };
        GetListeners()[eventType].push_back(wrapper);
    }

    template<typename T>
    static void Publish(const std::string& eventType, const T& data) {
        auto& listeners = GetListeners()[eventType];
        for (auto& listener : listeners) {
            listener(data);
        }
    }

    static void UnsubscribeAll(const std::string& eventType) {
        GetListeners().erase(eventType);
    }

    static void Clear() {
        GetListeners().clear();
    }

private:
    static std::unordered_map<std::string, std::vector<Listener>>& GetListeners() {
        static std::unordered_map<std::string, std::vector<Listener>> listeners;
        return listeners;
    }
};

struct WindowResizeEvent { int width; int height; };
struct KeyPressedEvent { int key; int scancode; int action; int mods; };
struct MouseMoveEvent { double x; double y; };
struct SceneChangeEvent { const char* sceneName; };

#endif
