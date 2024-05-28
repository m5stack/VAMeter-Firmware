/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>

// Vector2 type
typedef struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;
} Vector2;

// Vector3, 3 components
typedef struct Vector3
{
    float x = 0.0f; // Vector x component
    float y = 0.0f; // Vector y component
    float z = 0.0f; // Vector z component
} Vector3;

// Vector4, 4 components
typedef struct Vector4
{
    float x = 0.0f; // Vector x component
    float y = 0.0f; // Vector y component
    float z = 0.0f; // Vector z component
    float w = 0.0f; // Vector w component
} Vector4;

// Quaternion, 4 components (Vector4 alias)
typedef Vector4 Quaternion;

// Rectangle type
typedef struct Rectangle
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
} Rectangle;

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef EPSILON
#define EPSILON 0.000001f
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define LIGHTGRAY 0xC8C8C8  // Light Gray
#define GRAY 0x828282       // Gray
#define DARKGRAY 0x505050   // Dark Gray
#define YELLOW 0xFDF900     // Yellow
#define GOLD 0xFFCB00       // Gold
#define ORANGE 0xFFA100     // Orange
#define PINK 0xFF6DC2       // Pink
#define RED 0xE62937        // Red
#define MAROON 0xBE2137     // Maroon
#define GREEN 0x00E430      // Green
#define LIME 0x009E2F       // Lime
#define DARKGREEN 0x00752C  // Dark Green
#define SKYBLUE 0x66BFFF    // Sky Blue
#define BLUE 0x0079F1       // Blue
#define DARKBLUE 0x0052AC   // Dark Blue
#define PURPLE 0xC87AFF     // Purple
#define VIOLET 0x873CBE     // Violet
#define DARKPURPLE 0x701F7E // Dark Purple
#define BEIGE 0xD3B083      // Beige
#define BROWN 0x7F6A4F      // Brown
#define DARKBROWN 0x4C3F2F  // Dark Brown

#define WHITE 0xFFFFFF // White
#define BLACK 0x000000 // Black
// #define BLANK      0x   // Blank (Transparent)
#define MAGENTA 0xFF00FF  // Magenta
#define RAYWHITE 0xF5F5F5 // My own White (raylib logo)

void InitWindow(int w, int h, const char* title);
void SetTargetFPS(int fps);
bool WindowShouldClose();
void CloseWindow();
int GetScreenWidth();
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2);
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec);
void BeginDrawing();
void EndDrawing();
void ClearBackground(uint32_t color);
void DrawRectangle(int x, int y, int w, int h, uint32_t color);
void DrawCircleV(Vector2 center, float r, uint32_t color);
void DrawCircle(int centerX, int centerY, float radius, uint32_t color);
void DrawLineV(Vector2 startPos, Vector2 endPos, uint32_t color);
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color);
void DrawRectangleV(Vector2 position, Vector2 size, uint32_t color);
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, uint32_t color);
int GetRandomValue(int min, int max);
uint32_t Fade(uint32_t color, float alpha);
