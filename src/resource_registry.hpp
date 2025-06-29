#pragma once

#include <unordered_map>

template <typename T, typename Name>
class ResourceRegistry {
public:
    ResourceRegistry() = default;
    ~ResourceRegistry() = default;

    void add(const Name& name, T resource) {
        resources[name] = resource;
    }

    T get(const Name& name) const {
        return resources.at(name);
    }

private:
    std::unordered_map<Name, T> resources;
};
