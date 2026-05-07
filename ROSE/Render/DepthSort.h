#ifndef DEPTH_SORT_H
#define DEPTH_SORT_H

#include <vector>
#include <algorithm>

class DepthSortable {
public:
    virtual float GetDepthSortValue() const = 0;
    virtual ~DepthSortable() = default;
};

class DepthSort {
public:
    template<typename T>
    static void SortByY(std::vector<T>& items) {
        std::sort(items.begin(), items.end(), [](const T& a, const T& b) {
            return a->GetDepthSortValue() < b->GetDepthSortValue();
        });
    }

    template<typename T>
    static void SortByLayer(std::vector<T>& items) {
        std::stable_sort(items.begin(), items.end(), [](const T& a, const T& b) {
            return a->GetLayer() < b->GetLayer();
        });
    }

    template<typename T>
    static void SortByYThenLayer(std::vector<T>& items) {
        std::stable_sort(items.begin(), items.end(), [](const T& a, const T& b) {
            if (a->GetLayer() == b->GetLayer()) {
                return a->GetDepthSortValue() < b->GetDepthSortValue();
            }
            return a->GetLayer() < b->GetLayer();
        });
    }

    static float GetDepthFromY(float y, float minY = -1000.0f, float maxY = 1000.0f,
                                float minDepth = 0.0f, float maxDepth = 1.0f) {
        float t = (y - minY) / (maxY - minY);
        return minDepth + t * (maxDepth - minDepth);
    }
};

#endif
