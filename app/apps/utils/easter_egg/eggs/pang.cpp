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

using namespace SYSTEM::INPUTS;

namespace EasterEgg
{
    /*******************************************************************************************
     *
     *   raylib - classic game: pang
     *
     *   Sample game developed by Ian Eito and Albert Martos and Ramon Santamaria
     *
     *   This game has been created using raylib v1.3 (www.raylib.com)
     *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
     *
     *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
     *
     ********************************************************************************************/

#include <math.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
// #define PLAYER_BASE_SIZE 20.0f
#define PLAYER_BASE_SIZE 10.0f
#define PLAYER_SPEED 5.0f
#define PLAYER_MAX_SHOOTS 1

#define MAX_BIG_BALLS 2
#define BALLS_SPEED 2.0f

    //----------------------------------------------------------------------------------
    // Types and Structures Definition
    //----------------------------------------------------------------------------------
    typedef struct Player
    {
        Vector2 position;
        Vector2 speed;
        Vector3 collider;
        float rotation;
    } Player;

    typedef struct Shoot
    {
        Vector2 position;
        Vector2 speed;
        float radius;
        float rotation;
        int lifeSpawn;
        bool active;
    } Shoot;

    typedef struct Ball
    {
        Vector2 position;
        Vector2 speed;
        float radius;
        int points;
        bool active;
    } Ball;

    typedef struct Points
    {
        Vector2 position;
        int value;
        float alpha;
    } Points;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
// static const int screenWidth = 240;
// static const int screenHeight = 240;
#define screenWidth HAL::GetCanvas()->width()
#define screenHeight HAL::GetCanvas()->height()

    // static int framesCounter = 0;
    // static bool gameOver = false;
    // static bool pause = false;
    // static int score = 0;

    // static Player player = {0};
    // static Shoot shoot[PLAYER_MAX_SHOOTS] = {0};
    // static Ball bigBalls[MAX_BIG_BALLS] = {0};
    // static Ball mediumBalls[MAX_BIG_BALLS * 2] = {0};
    // static Ball smallBalls[MAX_BIG_BALLS * 4] = {0};
    // static Points points[5] = {0};

    // // NOTE: Defined triangle is isosceles with common angles of 70 degrees.
    // static float shipHeight = 0.0f;
    // static float gravity = 0.0f;

    // static int countmediumBallss = 0;
    // static int countsmallBallss = 0;
    // static int meteorsDestroyed = 0;
    // static Vector2 linePosition = {0};

    // static bool victory = false;
    // static bool lose = false;
    // static bool awake = false;

    struct PangData_t
    {
        int framesCounter = 0;
        bool gameOver = false;
        bool pause = false;
        int score = 0;

        Player player = {0};
        Shoot shoot[PLAYER_MAX_SHOOTS] = {0};
        Ball bigBalls[MAX_BIG_BALLS] = {0};
        Ball mediumBalls[MAX_BIG_BALLS * 2] = {0};
        Ball smallBalls[MAX_BIG_BALLS * 4] = {0};
        Points points[5] = {0};

        // NOTE: Defined triangle is isosceles with common angles of 70 degrees.
        float shipHeight = 0.0f;
        float gravity = 0.0f;

        int countmediumBallss = 0;
        int countsmallBallss = 0;
        int meteorsDestroyed = 0;
        Vector2 linePosition = {0};

        bool victory = false;
        bool lose = false;
        bool awake = false;

        int last_encoder_count = 0;
        int encoder_direction = 0;
    };
    static PangData_t* _data = nullptr;

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
    void start_pang()
    {
        _data = new PangData_t;

        // Initialization (Note windowTitle is unused on Android)
        //---------------------------------------------------------
        InitWindow(screenWidth, screenHeight, nullptr);

        InitGame();

#if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
        SetTargetFPS(60);
        //--------------------------------------------------------------------------------------

        auto update_count = HAL::Millis();

        // Main game loop
        while (1) // Detect window close button or ESC key
        {
            // // Update and Draw
            // //----------------------------------------------------------------------------------
            // UpdateDrawFrame();
            // //----------------------------------------------------------------------------------

            if (HAL::Millis() - update_count > 20)
            {
                HAL::FeedTheDog();
                Button::Update();

                // Update encoder
                if (HAL::GetEncoderCount() != _data->last_encoder_count)
                {
                    if (HAL::GetEncoderCount() < _data->last_encoder_count)
                        _data->encoder_direction = -1;
                    else
                        _data->encoder_direction = 1;
                    _data->last_encoder_count = HAL::GetEncoderCount();
                }
                else
                    _data->encoder_direction = 0;

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
    static void InitGame(void)
    {
        float posx, posy;
        float velx = 0;
        float vely = 0;

        _data->framesCounter = 0;
        _data->gameOver = false;
        _data->pause = false;
        _data->score = 0;

        _data->victory = false;
        _data->lose = false;
        _data->awake = true;
        _data->gravity = 0.25f;

        _data->linePosition = (Vector2){0.0f, 0.0f};
        _data->shipHeight = (PLAYER_BASE_SIZE / 2) / tanf(20 * DEG2RAD);

        // Initialization player
        _data->player.position = (Vector2){(float)screenWidth / 2, (float)screenHeight};
        _data->player.speed = (Vector2){PLAYER_SPEED, PLAYER_SPEED};
        _data->player.rotation = 0;
        _data->player.collider =
            (Vector3){_data->player.position.x, _data->player.position.y - _data->shipHeight / 2.0f, 12.0f};

        _data->meteorsDestroyed = 0;

        // Initialize shoots
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
        {
            _data->shoot[i].position = (Vector2){0, 0};
            _data->shoot[i].speed = (Vector2){0, 0};
            _data->shoot[i].radius = 2;
            _data->shoot[i].active = false;
            _data->shoot[i].lifeSpawn = 0;
        }

        // Initialize big meteors
        for (int i = 0; i < MAX_BIG_BALLS; i++)
        {
            _data->bigBalls[i].radius = 20.0f;
            posx = GetRandomValue(0 + _data->bigBalls[i].radius, screenWidth - _data->bigBalls[i].radius);
            posy = GetRandomValue(0 + _data->bigBalls[i].radius, screenHeight / 2);

            _data->bigBalls[i].position = (Vector2){posx, posy};

            while ((velx == 0) || (vely == 0))
            {
                velx = GetRandomValue(-BALLS_SPEED, BALLS_SPEED);
                vely = GetRandomValue(-BALLS_SPEED, BALLS_SPEED);
            }

            _data->bigBalls[i].speed = (Vector2){velx, vely};
            _data->bigBalls[i].points = 200;
            _data->bigBalls[i].active = true;
        }

        // Initialize medium meteors
        for (int i = 0; i < MAX_BIG_BALLS * 2; i++)
        {
            _data->mediumBalls[i].position = (Vector2){-100, -100};
            _data->mediumBalls[i].speed = (Vector2){0, 0};
            _data->mediumBalls[i].radius = 10.0f;
            _data->mediumBalls[i].points = 100;
            _data->mediumBalls[i].active = false;
        }

        // Initialize small meteors
        for (int i = 0; i < MAX_BIG_BALLS * 4; i++)
        {
            _data->smallBalls[i].position = (Vector2){-100, -100};
            _data->smallBalls[i].speed = (Vector2){0, 0};
            _data->smallBalls[i].radius = 5.0f;
            _data->smallBalls[i].points = 50;
            _data->smallBalls[i].active = false;
        }

        // Initialize animated points
        for (int i = 0; i < 5; i++)
        {
            _data->points[i].position = (Vector2){0, 0};
            _data->points[i].value = 0;
            _data->points[i].alpha = 0.0f;
        }

        _data->countmediumBallss = 0;
        _data->countsmallBallss = 0;
    }

    // Update game (one frame)
    void UpdateGame(void)
    {
        if (!_data->gameOver && !_data->victory)
        {
            // if (IsKeyPressed('P'))
            if (Button::Side()->wasClicked())
                _data->pause = !_data->pause;

            if (!_data->pause)
            {
                // Player logic
                // if (IsKeyDown(KEY_LEFT))
                //     player.position.x -= player.speed.x;
                // if (IsKeyDown(KEY_RIGHT))
                //     player.position.x += player.speed.x;
                if (_data->encoder_direction < 0)
                    _data->player.position.x -= _data->player.speed.x;
                else if (_data->encoder_direction > 0)
                    _data->player.position.x += _data->player.speed.x;

                // Player vs wall collision logic
                if (_data->player.position.x + PLAYER_BASE_SIZE / 2 > screenWidth)
                    _data->player.position.x = screenWidth - PLAYER_BASE_SIZE / 2;
                else if (_data->player.position.x - PLAYER_BASE_SIZE / 2 < 0)
                    _data->player.position.x = 0 + PLAYER_BASE_SIZE / 2;

                // Player shot logic
                // if (IsKeyPressed(KEY_SPACE))
                if (Button::Encoder()->wasClicked())
                {
                    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                    {
                        if (!_data->shoot[i].active)
                        {
                            _data->shoot[i].position =
                                (Vector2){_data->player.position.x, _data->player.position.y - _data->shipHeight};
                            _data->shoot[i].speed.y = PLAYER_SPEED;
                            _data->shoot[i].active = true;

                            _data->linePosition = (Vector2){_data->player.position.x, _data->player.position.y};

                            break;
                        }
                    }
                }

                // Shoot life timer
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (_data->shoot[i].active)
                        _data->shoot[i].lifeSpawn++;
                }

                // Shot logic
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (_data->shoot[i].active)
                    {
                        _data->shoot[i].position.y -= _data->shoot[i].speed.y;

                        // Shot vs walls collision logic
                        if ((_data->shoot[i].position.x > screenWidth + _data->shoot[i].radius) ||
                            (_data->shoot[i].position.x < 0 - _data->shoot[i].radius) ||
                            (_data->shoot[i].position.y > screenHeight + _data->shoot[i].radius) ||
                            (_data->shoot[i].position.y < 0 - _data->shoot[i].radius))
                        {
                            _data->shoot[i].active = false;
                            _data->shoot[i].lifeSpawn = 0;
                        }

                        // Player shot life spawn
                        if (_data->shoot[i].lifeSpawn >= 120)
                        {
                            _data->shoot[i].position = (Vector2){0.0f, 0.0f};
                            _data->shoot[i].speed = (Vector2){0.0f, 0.0f};
                            _data->shoot[i].lifeSpawn = 0;
                            _data->shoot[i].active = false;
                        }
                    }
                }

                // Player vs meteors collision logic
                _data->player.collider =
                    (Vector3){_data->player.position.x, _data->player.position.y - _data->shipHeight / 2, 12};

                for (int i = 0; i < MAX_BIG_BALLS; i++)
                {
                    if (CheckCollisionCircles((Vector2){_data->player.collider.x, _data->player.collider.y},
                                              _data->player.collider.z,
                                              _data->bigBalls[i].position,
                                              _data->bigBalls[i].radius) &&
                        _data->bigBalls[i].active)
                    {
                        _data->gameOver = true;
                    }
                }

                for (int i = 0; i < MAX_BIG_BALLS * 2; i++)
                {
                    if (CheckCollisionCircles((Vector2){_data->player.collider.x, _data->player.collider.y},
                                              _data->player.collider.z,
                                              _data->mediumBalls[i].position,
                                              _data->mediumBalls[i].radius) &&
                        _data->mediumBalls[i].active)
                    {
                        _data->gameOver = true;
                    }
                }

                for (int i = 0; i < MAX_BIG_BALLS * 4; i++)
                {
                    if (CheckCollisionCircles((Vector2){_data->player.collider.x, _data->player.collider.y},
                                              _data->player.collider.z,
                                              _data->smallBalls[i].position,
                                              _data->smallBalls[i].radius) &&
                        _data->smallBalls[i].active)
                    {
                        _data->gameOver = true;
                    }
                }

                // Meteors logic (big)
                for (int i = 0; i < MAX_BIG_BALLS; i++)
                {
                    if (_data->bigBalls[i].active)
                    {
                        // Meteor movement logic
                        _data->bigBalls[i].position.x += _data->bigBalls[i].speed.x;
                        _data->bigBalls[i].position.y += _data->bigBalls[i].speed.y;

                        // Meteor vs wall collision logic
                        if (((_data->bigBalls[i].position.x + _data->bigBalls[i].radius) >= screenWidth) ||
                            ((_data->bigBalls[i].position.x - _data->bigBalls[i].radius) <= 0))
                            _data->bigBalls[i].speed.x *= -1;
                        if ((_data->bigBalls[i].position.y - _data->bigBalls[i].radius) <= 0)
                            _data->bigBalls[i].speed.y *= -2;

                        if ((_data->bigBalls[i].position.y + _data->bigBalls[i].radius) >= screenHeight)
                        {
                            _data->bigBalls[i].speed.y *= -1;
                            _data->bigBalls[i].position.y = screenHeight - _data->bigBalls[i].radius;
                        }

                        _data->bigBalls[i].speed.y += _data->gravity;
                    }
                }

                // Meteors logic (medium)
                for (int i = 0; i < MAX_BIG_BALLS * 2; i++)
                {
                    if (_data->mediumBalls[i].active)
                    {
                        // Meteor movement logic
                        _data->mediumBalls[i].position.x += _data->mediumBalls[i].speed.x;
                        _data->mediumBalls[i].position.y += _data->mediumBalls[i].speed.y;

                        // Meteor vs wall collision logic
                        if (_data->mediumBalls[i].position.x + _data->mediumBalls[i].radius >= screenWidth ||
                            _data->mediumBalls[i].position.x - _data->mediumBalls[i].radius <= 0)
                            _data->mediumBalls[i].speed.x *= -1;
                        if (_data->mediumBalls[i].position.y - _data->mediumBalls[i].radius <= 0)
                            _data->mediumBalls[i].speed.y *= -1.5;
                        if (_data->mediumBalls[i].position.y + _data->mediumBalls[i].radius >= screenHeight)
                        {
                            _data->mediumBalls[i].speed.y *= -1;
                            _data->mediumBalls[i].position.y = screenHeight - _data->mediumBalls[i].radius;
                        }

                        _data->mediumBalls[i].speed.y += _data->gravity + 0.12f;
                    }
                }

                // Meteors logic (small)
                for (int i = 0; i < MAX_BIG_BALLS * 4; i++)
                {
                    if (_data->smallBalls[i].active)
                    {
                        // Meteor movement logic
                        _data->smallBalls[i].position.x += _data->smallBalls[i].speed.x;
                        _data->smallBalls[i].position.y += _data->smallBalls[i].speed.y;

                        // Meteor vs wall collision logic
                        if (_data->smallBalls[i].position.x + _data->smallBalls[i].radius >= screenWidth ||
                            _data->smallBalls[i].position.x - _data->smallBalls[i].radius <= 0)
                            _data->smallBalls[i].speed.x *= -1;
                        if (_data->smallBalls[i].position.y - _data->smallBalls[i].radius <= 0)
                            _data->smallBalls[i].speed.y *= -1.5;
                        if (_data->smallBalls[i].position.y + _data->smallBalls[i].radius >= screenHeight)
                        {
                            _data->smallBalls[i].speed.y *= -1;
                            _data->smallBalls[i].position.y = screenHeight - _data->smallBalls[i].radius;
                        }

                        _data->smallBalls[i].speed.y += _data->gravity + 0.25f;
                    }
                }

                // Player-shot vs meteors logic
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if ((_data->shoot[i].active))
                    {
                        for (int a = 0; a < MAX_BIG_BALLS; a++)
                        {
                            if (_data->bigBalls[a].active &&
                                (_data->bigBalls[a].position.x - _data->bigBalls[a].radius <= _data->linePosition.x &&
                                 _data->bigBalls[a].position.x + _data->bigBalls[a].radius >= _data->linePosition.x) &&
                                (_data->bigBalls[a].position.y + _data->bigBalls[a].radius >= _data->shoot[i].position.y))
                            {
                                _data->shoot[i].active = false;
                                _data->shoot[i].lifeSpawn = 0;
                                _data->bigBalls[a].active = false;
                                _data->meteorsDestroyed++;
                                _data->score += _data->bigBalls[a].points;

                                for (int z = 0; z < 5; z++)
                                {
                                    if (_data->points[z].alpha == 0.0f)
                                    {
                                        _data->points[z].position = _data->bigBalls[a].position;
                                        _data->points[z].value = _data->bigBalls[a].points;
                                        _data->points[z].alpha = 1.0f;
                                        z = 5;
                                    }
                                }

                                for (int j = 0; j < 2; j++)
                                {
                                    if ((_data->countmediumBallss % 2) == 0)
                                    {
                                        _data->mediumBalls[_data->countmediumBallss].position =
                                            (Vector2){_data->bigBalls[a].position.x, _data->bigBalls[a].position.y};
                                        _data->mediumBalls[_data->countmediumBallss].speed =
                                            (Vector2){-1 * BALLS_SPEED, BALLS_SPEED};
                                    }
                                    else
                                    {
                                        _data->mediumBalls[_data->countmediumBallss].position =
                                            (Vector2){_data->bigBalls[a].position.x, _data->bigBalls[a].position.y};
                                        _data->mediumBalls[_data->countmediumBallss].speed =
                                            (Vector2){BALLS_SPEED, BALLS_SPEED};
                                    }

                                    _data->mediumBalls[_data->countmediumBallss].active = true;
                                    _data->countmediumBallss++;
                                }

                                a = MAX_BIG_BALLS;
                            }
                        }
                    }

                    if ((_data->shoot[i].active))
                    {
                        for (int b = 0; b < MAX_BIG_BALLS * 2; b++)
                        {
                            if (_data->mediumBalls[b].active &&
                                (_data->mediumBalls[b].position.x - _data->mediumBalls[b].radius <= _data->linePosition.x &&
                                 _data->mediumBalls[b].position.x + _data->mediumBalls[b].radius >= _data->linePosition.x) &&
                                (_data->mediumBalls[b].position.y + _data->mediumBalls[b].radius >= _data->shoot[i].position.y))
                            {
                                _data->shoot[i].active = false;
                                _data->shoot[i].lifeSpawn = 0;
                                _data->mediumBalls[b].active = false;
                                _data->meteorsDestroyed++;
                                _data->score += _data->mediumBalls[b].points;

                                for (int z = 0; z < 5; z++)
                                {
                                    if (_data->points[z].alpha == 0.0f)
                                    {
                                        _data->points[z].position = _data->mediumBalls[b].position;
                                        _data->points[z].value = _data->mediumBalls[b].points;
                                        _data->points[z].alpha = 1.0f;
                                        z = 5;
                                    }
                                }

                                for (int j = 0; j < 2; j++)
                                {
                                    if (_data->countsmallBallss % 2 == 0)
                                    {
                                        _data->smallBalls[_data->countsmallBallss].position =
                                            (Vector2){_data->mediumBalls[b].position.x, _data->mediumBalls[b].position.y};
                                        _data->smallBalls[_data->countsmallBallss].speed =
                                            (Vector2){BALLS_SPEED * -1, BALLS_SPEED * -1};
                                    }
                                    else
                                    {
                                        _data->smallBalls[_data->countsmallBallss].position =
                                            (Vector2){_data->mediumBalls[b].position.x, _data->mediumBalls[b].position.y};
                                        _data->smallBalls[_data->countsmallBallss].speed =
                                            (Vector2){BALLS_SPEED, BALLS_SPEED * -1};
                                    }

                                    _data->smallBalls[_data->countsmallBallss].active = true;
                                    _data->countsmallBallss++;
                                }

                                b = MAX_BIG_BALLS * 2;
                            }
                        }
                    }

                    if ((_data->shoot[i].active))
                    {
                        for (int c = 0; c < MAX_BIG_BALLS * 4; c++)
                        {
                            if (_data->smallBalls[c].active &&
                                (_data->smallBalls[c].position.x - _data->smallBalls[c].radius <= _data->linePosition.x &&
                                 _data->smallBalls[c].position.x + _data->smallBalls[c].radius >= _data->linePosition.x) &&
                                (_data->smallBalls[c].position.y + _data->smallBalls[c].radius >= _data->shoot[i].position.y))
                            {
                                _data->shoot[i].active = false;
                                _data->shoot[i].lifeSpawn = 0;
                                _data->smallBalls[c].active = false;
                                _data->meteorsDestroyed++;
                                _data->score += _data->smallBalls[c].points;

                                for (int z = 0; z < 5; z++)
                                {
                                    if (_data->points[z].alpha == 0.0f)
                                    {
                                        _data->points[z].position = _data->smallBalls[c].position;
                                        _data->points[z].value = _data->smallBalls[c].points;
                                        _data->points[z].alpha = 1.0f;
                                        z = 5;
                                    }
                                }

                                c = MAX_BIG_BALLS * 4;
                            }
                        }
                    }
                }

                if (_data->meteorsDestroyed == (MAX_BIG_BALLS + MAX_BIG_BALLS * 2 + MAX_BIG_BALLS * 4))
                    _data->victory = true;
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

        // Points move-up and fade logic
        for (int z = 0; z < 5; z++)
        {
            if (_data->points[z].alpha > 0.0f)
            {
                _data->points[z].position.y -= 2;
                _data->points[z].alpha -= 0.02f;
            }

            if (_data->points[z].alpha < 0.0f)
                _data->points[z].alpha = 0.0f;
        }
    }

    // Draw game (one frame)
    void DrawGame(void)
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!_data->gameOver)
        {
            // Draw player
            Vector2 v1 = {_data->player.position.x + sinf(_data->player.rotation * DEG2RAD) * (_data->shipHeight),
                          _data->player.position.y - cosf(_data->player.rotation * DEG2RAD) * (_data->shipHeight)};
            Vector2 v2 = {_data->player.position.x - cosf(_data->player.rotation * DEG2RAD) * (PLAYER_BASE_SIZE / 2),
                          _data->player.position.y - sinf(_data->player.rotation * DEG2RAD) * (PLAYER_BASE_SIZE / 2)};
            Vector2 v3 = {_data->player.position.x + cosf(_data->player.rotation * DEG2RAD) * (PLAYER_BASE_SIZE / 2),
                          _data->player.position.y + sinf(_data->player.rotation * DEG2RAD) * (PLAYER_BASE_SIZE / 2)};
            DrawTriangle(v1, v2, v3, MAROON);

            // Draw meteors (big)
            for (int i = 0; i < MAX_BIG_BALLS; i++)
            {
                if (_data->bigBalls[i].active)
                    DrawCircleV(_data->bigBalls[i].position, _data->bigBalls[i].radius, DARKGRAY);
                else
                    DrawCircleV(_data->bigBalls[i].position, _data->bigBalls[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            // Draw meteors (medium)
            for (int i = 0; i < MAX_BIG_BALLS * 2; i++)
            {
                if (_data->mediumBalls[i].active)
                    DrawCircleV(_data->mediumBalls[i].position, _data->mediumBalls[i].radius, GRAY);
                else
                    DrawCircleV(_data->mediumBalls[i].position, _data->mediumBalls[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            // Draw meteors (small)
            for (int i = 0; i < MAX_BIG_BALLS * 4; i++)
            {
                if (_data->smallBalls[i].active)
                    DrawCircleV(_data->smallBalls[i].position, _data->smallBalls[i].radius, GRAY);
                else
                    DrawCircleV(_data->smallBalls[i].position, _data->smallBalls[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            // Draw shoot
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (_data->shoot[i].active)
                    DrawLine(_data->linePosition.x,
                             _data->linePosition.y,
                             _data->shoot[i].position.x,
                             _data->shoot[i].position.y,
                             RED);
            }

            // Draw score points
            for (int z = 0; z < 5; z++)
            {
                if (_data->points[z].alpha > 0.0f)
                {
                    // DrawText(TextFormat("+%02i", points[z].value),
                    //          points[z].position.x,
                    //          points[z].position.y,
                    //          20,
                    //          Fade(BLUE, points[z].alpha));

                    HAL::GetCanvas()->setCursor(_data->points[z].position.x, _data->points[z].position.y);
                    HAL::GetCanvas()->setTextColor(Fade(BLUE, _data->points[z].alpha));
                    HAL::GetCanvas()->printf("+%02i", _data->points[z].value);
                }
            }

            // Draw score (UI)
            // DrawText(TextFormat("SCORE: %i", score), 10, 10, 20, LIGHTGRAY);

            HAL::GetCanvas()->setTextColor(LIGHTGRAY);
            HAL::GetCanvas()->setCursor(10, 10);
            HAL::GetCanvas()->printf("SCORE: %i", _data->score);

            if (_data->victory)
            {
                // DrawText("YOU WIN!", screenWidth / 2 - MeasureText("YOU WIN!", 60) / 2, 100, 60, LIGHTGRAY);
                // DrawText("PRESS [ENTER] TO PLAY AGAIN",
                //          GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                //          GetScreenHeight() / 2 - 50,
                //          20,
                //          LIGHTGRAY);

                HAL::GetCanvas()->setTextColor((uint32_t)GRAY);
                HAL::GetCanvas()->drawCenterString("YOU WIN!", 120, 52);
                HAL::GetCanvas()->drawCenterString("PRESS [START]", 120, 76);
                HAL::GetCanvas()->drawCenterString("TO PLAY AGAIN", 120, 100);
            }

            if (_data->pause)
            // DrawText(
            //     "GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, LIGHTGRAY);
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
        //          LIGHTGRAY);
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
