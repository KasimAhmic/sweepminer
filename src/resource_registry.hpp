#pragma once

#include <unordered_map>

template <typename T, typename Name>
class ResourceRegistry {
public:
    ResourceRegistry() = default;
    ~ResourceRegistry() = default;

    void AddResource(const Name& name, T resource) {
        resources[name] = resource;
    }

    T GetResource(const Name& name) const {
        auto it = resources.find(name);
        return (it != resources.end()) ? it->second : nullptr;
    }

private:
    std::unordered_map<Name, T> resources;
};
