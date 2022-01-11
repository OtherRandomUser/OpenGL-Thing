#pragma once

#include <string>

class Texture
{
public:
    explicit Texture(const std::string& filename);

    void bind(int unit);

private:
    unsigned int _id;
};
