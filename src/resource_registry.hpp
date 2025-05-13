#pragma once

#include <memory>
#include <unordered_map>

template <typename T, typename Name>
class ResourceRegistry {
public:
    ResourceRegistry() = default;
    ~ResourceRegistry() = default;

    void AddResource(const Name& name, std::shared_ptr<T> resource) {
        resources[name] = resource;
    }

    std::shared_ptr<T> GetResource(const Name& name) const {
        auto it = resources.find(name);
        return (it != resources.end()) ? it->second : nullptr;
    }

private:
    std::unordered_map<Name, std::shared_ptr<T>> resources;
};
