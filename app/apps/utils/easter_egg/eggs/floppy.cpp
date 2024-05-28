/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../easter_egg.h"
#include "../../../../hal/hal.h"
#include "../../../../assets/assets.h"
#include "../../system/system.h"
#include "../raylib/raylib.h"
#include "apps/utils/system/inputs/button/button.h"
#include <cstdint>

using namespace SYSTEM::INPUTS;

namespace EasterEgg
{
    /*******************************************************************************************
     *
     *   raylib - classic game: floppy
     *
     *   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
     *
     *   This game has been created using raylib v1.3 (www.raylib.com)
     *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
     *
     *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
     *
     ********************************************************************************************/

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define MAX_TUBES 100
#define FLOPPY_RADIUS 6
#define TUBES_WIDTH 20

    //----------------------------------------------------------------------------------
    // Types and Structures Definition
    //----------------------------------------------------------------------------------
    typedef struct Floppy
    {
        Vector2 position;
        int radius;
        uint32_t color;
    } Floppy;

    typedef struct Tubes
    {
        Rectangle rec;
        uint32_t color;
        bool active;
    } Tubes;

    //------------------------------------------------------------------------------------
    // Global Variables Declaration
    //------------------------------------------------------------------------------------
    // static const int screenWidth = 800;
    // static const int screenHeight = 450;
#define screenWidth HAL::GetCanvas()->width()
#define screenHeight HAL::GetCanvas()->height()

    // static bool gameOver = false;
    // static bool pause = false;
    // static int score = 0;
    // static int hiScore = 0;

    // static Floppy floppy = {0};
    // static Tubes tubes[MAX_TUBES * 2] = {0};
    // static Vector2 tubesPos[MAX_TUBES] = {0};
    // static int tubesSpeedX = 0;
    // static bool superfx = false;

    struct FloppyData_t
    {
        bool gameOver = false;
        bool pause = false;
        int score = 0;
        int hiScore = 0;

        Floppy floppy = {0};
        Tubes tubes[MAX_TUBES * 2] = {0};
        Vector2 tubesPos[MAX_TUBES] = {0};
        int tubesSpeedX = 0;
        bool superfx = false;
    };
    static FloppyData_t* _data = nullptr;

    //------------------------------------------------------------------------------------
    // Module Functions Declaration (local)
    //------------------------------------------------------------------------------------
    static void InitGame(void);        // Initialize game
    static void UpdateGame(void);      // Update game (one frame)
    static void DrawGame(void);        // Draw game (one frame)
    static void UnloadGame(void);      // Unload game
    static void UpdateDrawFrame(void); // Update and Draw (one frame)

    //------------------------------------------------------------------------------------
    // Program main entry point
    //------------------------------------------------------------------------------------
    void start_floppy()
    {
        _data = new FloppyData_t;

        // Initialization
        //---------------------------------------------------------
        InitWindow(screenWidth, screenHeight, nullptr);

        InitGame();

#if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
        // SetTargetFPS(60);
        //--------------------------------------------------------------------------------------

        // // Main game loop
        // while (!WindowShouldClose()) // Detect window close button or ESC key
        // {
        //     // Update and Draw
        //     //----------------------------------------------------------------------------------
        //     UpdateDrawFrame();
        //     //----------------------------------------------------------------------------------
        // }

        auto update_count = HAL::Millis();
        while (1)
        {
            if (HAL::Millis() - update_count > 20)
            {
                HAL::FeedTheDog();
                Button::Update();

                // // Update encoder
                // if (HAL::GetEncoderCount() != _data->last_encoder_count)
                // {
                //     if (HAL::GetEncoderCount() < _data->last_encoder_count)
                //         _data->encoder_direction = -1;
                //     else
                //         _data->encoder_direction = 1;
                //     _data->last_encoder_count = HAL::GetEncoderCount();
                // }
                // else
                //     _data->encoder_direction = 0;

                UpdateDrawFrame();

                if (Button::Side()->wasHold())
                    break;

                update_count = HAL::Millis();
            }
        }
#endif
        // De-Initialization
        //--------------------------------------------------------------------------------------
        UnloadGame(); // Unload loaded data (textures, sounds, models...)

        CloseWindow(); // Close window and OpenGL context
        //--------------------------------------------------------------------------------------

        delete _data;
    }
    //------------------------------------------------------------------------------------
    // Module Functions Definitions (local)
    //------------------------------------------------------------------------------------

    // Initialize game variables
    void InitGame(void)
    {
        _data->floppy.radius = FLOPPY_RADIUS;
        _data->floppy.position = (Vector2){80, (float)screenHeight / 2 - _data->floppy.radius};
        _data->tubesSpeedX = 2;

        for (int i = 0; i < MAX_TUBES; i++)
        {
            _data->tubesPos[i].x = 200 + 180 * i;
            _data->tubesPos[i].y = -GetRandomValue(0, 60);
        }

        for (int i = 0; i < MAX_TUBES * 2; i += 2)
        {
            _data->tubes[i].rec.x = _data->tubesPos[i / 2].x;
            _data->tubes[i].rec.y = _data->tubesPos[i / 2].y;
            _data->tubes[i].rec.width = TUBES_WIDTH;
            _data->tubes[i].rec.height = 120;

            _data->tubes[i + 1].rec.x = _data->tubesPos[i / 2].x;
            _data->tubes[i + 1].rec.y = _data->tubesPos[i / 2].y + 60 + 120;
            _data->tubes[i + 1].rec.width = TUBES_WIDTH;
            _data->tubes[i + 1].rec.height = 120;

            _data->tubes[i / 2].active = true;
        }

        _data->score = 0;

        _data->gameOver = false;
        _data->superfx = false;
        _data->pause = false;
    }

    // Update game (one frame)
    void UpdateGame(void)
    {
        if (!_data->gameOver)
        {
            // if (IsKeyPressed('P'))
            if (Button::Side()->wasClicked())
                _data->pause = !_data->pause;

            if (!_data->pause)
            {
                for (int i = 0; i < MAX_TUBES; i++)
                    _data->tubesPos[i].x -= _data->tubesSpeedX;

                for (int i = 0; i < MAX_TUBES * 2; i += 2)
                {
                    _data->tubes[i].rec.x = _data->tubesPos[i / 2].x;
                    _data->tubes[i + 1].rec.x = _data->tubesPos[i / 2].x;
                }

                // if (IsKeyDown(KEY_SPACE) && !_data->gameOver)
                if (Button::Encoder()->isPressed() && !_data->gameOver)
                    _data->floppy.position.y -= 3;
                else
                    _data->floppy.position.y += 3;

                // Check Collisions
                for (int i = 0; i < MAX_TUBES * 2; i++)
                {
                    if (CheckCollisionCircleRec(_data->floppy.position, _data->floppy.radius, _data->tubes[i].rec))
                    {
                        _data->gameOver = true;
                        _data->pause = false;
                    }
                    else if ((_data->tubesPos[i / 2].x < _data->floppy.position.x) && _data->tubes[i / 2].active &&
                             !_data->gameOver)
                    {
                        _data->score += 100;
                        _data->tubes[i / 2].active = false;

                        _data->superfx = true;

                        if (_data->score > _data->hiScore)
                            _data->hiScore = _data->score;
                    }
                }
            }
        }
        else
        {
            // if (IsKeyPressed(KEY_ENTER))
            if (Button::Side()->wasClicked())
            {
                InitGame();
                _data->gameOver = false;
            }
        }
    }

    // Draw game (one frame)
    void DrawGame(void)
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!_data->gameOver)
        {
            DrawCircle(_data->floppy.position.x, _data->floppy.position.y, _data->floppy.radius, DARKGRAY);

            // Draw tubes
            for (int i = 0; i < MAX_TUBES; i++)
            {
                DrawRectangle(_data->tubes[i * 2].rec.x,
                              _data->tubes[i * 2].rec.y,
                              _data->tubes[i * 2].rec.width,
                              _data->tubes[i * 2].rec.height,
                              GRAY);
                DrawRectangle(_data->tubes[i * 2 + 1].rec.x,
                              _data->tubes[i * 2 + 1].rec.y,
                              _data->tubes[i * 2 + 1].rec.width,
                              _data->tubes[i * 2 + 1].rec.height,
                              GRAY);
            }

            // Draw flashing fx (one frame only)
            if (_data->superfx)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
                _data->superfx = false;
            }

            // DrawText(TextFormat("%04i", score), 20, 20, 40, GRAY);
            // DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, LIGHTGRAY);
            HAL::GetCanvas()->setTextColor((uint32_t)GRAY);
            HAL::GetCanvas()->setCursor(5, 5);
            HAL::GetCanvas()->printf("BEST: %04i", _data->hiScore);
            HAL::GetCanvas()->setCursor(5, 29);
            HAL::GetCanvas()->printf("%04i", _data->score);

            if (_data->pause)
            // DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
            {
                HAL::GetCanvas()->setTextColor((uint32_t)GRAY);
                HAL::GetCanvas()->drawCenterString("GAME PAUSED", 120, 76);
            }
        }
        else
        // DrawText("PRESS [ENTER] TO PLAY AGAIN",
        //          GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
        //          GetScreenHeight() / 2 - 50,
        //          20,
        //          GRAY);
        {
            HAL::GetCanvas()->setTextColor((uint32_t)GRAY);
            HAL::GetCanvas()->drawCenterString("PRESS [START]", 120, 76);
            HAL::GetCanvas()->drawCenterString("TO PLAY AGAIN", 120, 100);
        }

        EndDrawing();
    }

    // Unload game variables
    void UnloadGame(void)
    {
        // TODO: Unload all dynamic loaded data (textures, sounds, models...)
    }

    // Update and Draw (one frame)
    void UpdateDrawFrame(void)
    {
        UpdateGame();
        DrawGame();
    }
} // namespace EasterEgg
