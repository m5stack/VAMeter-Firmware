/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "raylib.h"
#include "../../../../hal/hal.h"
#include "../../../../assets/assets.h"
#include "core/math/math.h"
#include "lgfx/v1/misc/colortype.hpp"
#include <cstdint>

void InitWindow(int w, int h, const char* title) { AssetPool::LoadFont24(HAL::GetCanvas()); }

void SetTargetFPS(int fps) {}

bool WindowShouldClose() { return false; }

void CloseWindow() {}

int GetScreenWidth() { return HAL::GetCanvas()->width(); }

void BeginDrawing() {}

void EndDrawing() { HAL::CanvasUpdate(); }

void ClearBackground(uint32_t color) { HAL::GetCanvas()->fillScreen(color); }

void DrawRectangle(int x, int y, int w, int h, uint32_t color) { HAL::GetCanvas()->fillRect(x, y, w, h, color); }

void DrawCircleV(Vector2 center, float r, uint32_t color) { HAL::GetCanvas()->fillSmoothCircle(center.x, center.y, r, color); }

// Draw a color-filled circle
void DrawCircle(int centerX, int centerY, float radius, uint32_t color)
{
    DrawCircleV((Vector2){(float)centerX, (float)centerY}, radius, color);
}

void DrawLineV(Vector2 startPos, Vector2 endPos, uint32_t color)
{
    HAL::GetCanvas()->drawLine(startPos.x, startPos.y, endPos.x, endPos.y, color);
}

void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color)
{
    HAL::GetCanvas()->drawLine(startPosX, startPosY, endPosX, endPosY, color);
}

void DrawRectangleV(Vector2 position, Vector2 size, uint32_t color)
{
    HAL::GetCanvas()->fillRect(position.x, position.y, size.x, size.y, color);
}

void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, uint32_t color)
{
    HAL::GetCanvas()->fillTriangle(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, color);
}

#include <random>
std::random_device rd;
std::mt19937 gen(rd());
int GetRandomValue(int min, int max)
{
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

// Check collision between two circles
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    bool collision = false;

    float dx = center2.x - center1.x; // X distance between centers
    float dy = center2.y - center1.y; // Y distance between centers

    float distance = sqrtf(dx * dx + dy * dy); // Distance between centers

    if (distance <= (radius1 + radius2))
        collision = true;

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec)
{
    bool collision = false;

    int recCenterX = (int)(rec.x + rec.width / 2.0f);
    int recCenterY = (int)(rec.y + rec.height / 2.0f);

    float dx = fabsf(center.x - (float)recCenterX);
    float dy = fabsf(center.y - (float)recCenterY);

    if (dx > (rec.width / 2.0f + radius))
    {
        return false;
    }
    if (dy > (rec.height / 2.0f + radius))
    {
        return false;
    }

    if (dx <= (rec.width / 2.0f))
    {
        return true;
    }
    if (dy <= (rec.height / 2.0f))
    {
        return true;
    }

    float cornerDistanceSq =
        (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) + (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    collision = (cornerDistanceSq <= (radius * radius));

    return collision;
}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
uint32_t Fade(uint32_t color, float alpha)
{
    auto ret = SmoothUIToolKit::Hex2Rgb(color);

    ret.red *= alpha;
    ret.green *= alpha;
    ret.blue *= alpha;

    return SmoothUIToolKit::Rgb2Hex(ret);
}
