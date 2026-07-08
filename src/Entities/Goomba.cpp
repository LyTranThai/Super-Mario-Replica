#include "Goomba.hpp"

Goomba::Goomba(Vector2 pos) 
    : Enemy(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 24.0f, 24.0f }, Vector2{ 4.0f, 8.0f }, "goomba", RED) {}
