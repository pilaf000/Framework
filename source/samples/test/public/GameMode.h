#pragma once
#include <cstdint>

namespace test
{

enum class Mode : std::uint8_t
{
    Title = 0u,
    Main,
    Result,
    End
};

class GameMode
{
public:
    GameMode();
    ~GameMode();

private:
};
}