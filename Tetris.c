#include "raylib.h"
#include <time.h>
#include <string.h>
#include "Tetris.h"
#include "TetrominoShape.h"

#define PlayClipAudio(clip)(PlaySound(clip))

//WindowSize
const int windowWidth = 600; 
const int windowHeight = 700; 

//GameTextInfo
int score = 0;
int game_over = 0;

//Tetromino info
const float start_tetromino_speed = 1;
float tretomino_speed;
const int tetrominoStartX = STAGE_WIDTH * 0.5f;
const int tetrominoStartY = 0;
int currentTetrominoX;
int currentTetrominoY;
int currentTetrominoType;
int currentRotation;
const float moveTetrominoDownTimer = 1.f;
float timeToMoveTetrominoDown;
int currentColor;

//Sounds
Sound clip_audio;
Sound explosion_audio;
Sound intro_tetris;
Sound finish_game_audio;
Music music_loop;

//Visual Effect
int enable_effect = 0;
int circle_start_pos_x,circle_start_posY;
float circle_start_radius = 2;
float cirlce_radius;
float life_time = .4f;
float current_life_time;
int base_effect = 0;

void ResetTetromino(){
    currentTetrominoX = tetrominoStartX;
    currentTetrominoY = tetrominoStartY;
    tretomino_speed = start_tetromino_speed;
    currentTetrominoType = GetRandomValue(0, 6);
    currentRotation = 0;
    timeToMoveTetrominoDown = moveTetrominoDownTimer;
    currentColor = GetRandomValue(0, 7);
}
void LoadAudio()
{
    InitAudioDevice();
    clip_audio = LoadSound("C:/Users/Alex/Desktop/Aiv3Year/Boh/AivTetris/Audio/clip.wav");
    explosion_audio = LoadSound("C:/Users/Alex/Desktop/Aiv3Year/Boh/AivTetris/Audio/Explosion.wav");
    intro_tetris = LoadSound("C:/Users/Alex/Desktop/Aiv3Year/Boh/AivTetris/Audio/Tetris.wav");
    finish_game_audio = LoadSound("C:/Users/Alex/Desktop/Aiv3Year/Boh/AivTetris/Audio/FinishGame.wav");
    music_loop = LoadMusicStream("C:/Users/Alex/Desktop/Aiv3Year/Boh/AivTetris/Audio/MusicLoop.wav");
}
void ResetGame(){
    tretomino_speed = start_tetromino_speed;
    score = 0;
    game_over = 0;
    enable_effect = 0;
    for(int y = 1; y < STAGE_HEIGHT - 1; y++)
    {
        for(int x = 1; x < STAGE_WIDTH -1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;
            const int color = stage[offset];

            if(stage[offset] != 0)
            {
                stage[offset] = 0;
            }
        }
    }
    ResetTetromino();
    PlayMusicStream(music_loop);
    StopSound(finish_game_audio);
}
void enable_circle_effect(const int pos_x,const int pos_y,const int effect_type)
{
    if (enable_effect == 1)
    {
        return;
    }
    
    circle_start_pos_x = pos_x;
    circle_start_posY = pos_y;
    cirlce_radius = circle_start_radius;
    current_life_time = life_time;
    enable_effect = 1;
    base_effect = effect_type;
}
void effect_countdown()
{
    current_life_time -= GetFrameTime();
    if (current_life_time <= 0)
    {
        enable_effect = 0;
    }
    cirlce_radius *= 1.2f;
}
int CheckCollision(const int tetrominoStartX, const int tetrominoStartY, const int *tetromino)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int offset = y * TETROMINO_SIZE + x;

            if(tetromino[offset] == 1)
            {
                const int offset_stage = (y + tetrominoStartY) * STAGE_WIDTH + (x +tetrominoStartX);

                if (stage[offset_stage] != 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}
void ResetLines(int startLineY)
{
    for (int y = startLineY; y >= 0; y--)
    {
        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;
            const int offset_below = (y+1) * STAGE_WIDTH + x;

            if (stage[offset_below] == 0 && stage[offset] > 0)
            {
                stage[offset_below] = stage[offset];
                stage[offset] = 0;
            }
        }
    }   
}
void DeleteLines()
{
    for (int y = 0; y < STAGE_HEIGHT - 1; y++)
    {
        int checkLine = 1;

        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;

            if (stage[offset] == 0)
            {
                checkLine = 0;
                break;
            }
        }

        if(checkLine)
        {
            const int offset = y * STAGE_WIDTH + 1;
            memset(stage+offset,0,(STAGE_WIDTH-2)* sizeof(int));

            enable_circle_effect((windowWidth * 0.5f) - ((STAGE_WIDTH * TILE_SIZE) * 0.5f),((y + 1) * TILE_SIZE) +  (windowHeight * 0.5f) - ((STAGE_HEIGHT * TILE_SIZE) * 0.5f),1);
            ResetLines(y);
            score+=100;
            PlayClipAudio(explosion_audio);
        }
    }   
}
void drawTetromino(const Color currentColor, const int startOffsetX, const int startOffsetY, const int tetrominoStartX, const int tetrominoStartY, const int *tetromino)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int offset = y * TETROMINO_SIZE + x;

            if(tetromino[offset] == 1)
            {
                DrawRectangle((x + tetrominoStartX) * TILE_SIZE + startOffsetX, (y + tetrominoStartY) * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, currentColor);
            }
        }
    }
}
void draw_stage(const int startOffsetX,const int startOffsetY){
    for(int y = 0; y < STAGE_HEIGHT; y++)
        {
            for(int x = 0; x < STAGE_WIDTH; x++)
            {
                const int offset = y * STAGE_WIDTH + x;
                const int color = stage[offset];

                if(stage[offset] != 0)
                {
                    DrawRectangle(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, colorTypes[color-1]);
                }

                DrawRectangleLines(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }
}
void draw_circle_effect()
{
    if (base_effect == 0)
    {
        for (int i = -1; i < 2; i++)
        {
            DrawCircleLines(circle_start_pos_x + (i * TILE_SIZE) ,circle_start_posY - ((i==0?2:0) * TILE_SIZE),cirlce_radius,RED);
        }
    }
    else
    {
        for (int i = 0; i < STAGE_WIDTH; i++)
        {
            DrawCircleLines(circle_start_pos_x + (i * TILE_SIZE),circle_start_posY,cirlce_radius,RED);
        }
    }
}
void draw_all(const int startOffsetX,const int startOffsetY)
{
    BeginDrawing();
    ClearBackground(BLACK);
    draw_stage(startOffsetX,startOffsetY);
    drawTetromino(colorTypes[currentColor],startOffsetX, startOffsetY, currentTetrominoX, currentTetrominoY, tetrominoTypes[currentTetrominoType][currentRotation]);
    DrawText(TextFormat("Score: %d",score), windowWidth * 0.5f - 100,5,50,WHITE);
    DrawText(TextFormat("Tetromino Speed: %f",tretomino_speed), 0,650,35,WHITE);
    if (enable_effect == 1 && !game_over)
    {
        draw_circle_effect();
    }
    if (game_over==1)
    {
        DrawText("Press Space to restart..", windowWidth * 0.5f - 250,windowHeight * 0.5f,40,WHITE);
    }
    EndDrawing();
}
int main(int argc, char** argv, char** environ)
{
    //Setting World start screen offset
    const int startOffsetX = (windowWidth * 0.5f) - ((STAGE_WIDTH * TILE_SIZE) * 0.5f);
    const int startOffsetY = (windowHeight * 0.5f) - ((STAGE_HEIGHT * TILE_SIZE) * 0.5f);

    //Initialize Random
    time_t unixTime;
    time(&unixTime);
    SetRandomSeed(unixTime);

    //Setup Windo
    InitWindow(windowWidth, windowHeight, "Tetris");
    SetTargetFPS(60);
    
    //Init Game Instances
    LoadAudio();
    ResetTetromino();

    //PlayStartClip/Background
    PlayClipAudio(intro_tetris);
    PlayMusicStream(music_loop);

    while(!WindowShouldClose())
    {
        //Gameloop
        UpdateMusicStream(music_loop);
        if(game_over == 0)
        {
            if(enable_effect){
                effect_countdown();
            }
            timeToMoveTetrominoDown -= GetFrameTime() * tretomino_speed;
            if (IsKeyPressed(KEY_SPACE))
            {
                int new_rotation =  currentRotation + 1;

                if (new_rotation > 3)
                {
                    new_rotation = 0;
                }

                if (!CheckCollision(currentTetrominoX,currentTetrominoY,tetrominoTypes[currentTetrominoType][new_rotation]))
                {
                    currentRotation = new_rotation;
                }
            }
            if (IsKeyPressed(KEY_RIGHT))
            {
                // No need to check overflow, wall is your protector
                if (!CheckCollision(currentTetrominoX+1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoX++;
                }
            }
            if (IsKeyPressed(KEY_LEFT))
            {
                // No need to check overflow, wall is your protector
                if (!CheckCollision(currentTetrominoX-1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoX--;
                }
            }
            if(timeToMoveTetrominoDown <= 0 || IsKeyPressed(KEY_DOWN))
            {          
                if(!CheckCollision(currentTetrominoX,currentTetrominoY+1,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoY++;
                    timeToMoveTetrominoDown = moveTetrominoDownTimer;
                }
                else
                {
                    PlayClipAudio(clip_audio);

                    for(int y = 0; y < TETROMINO_SIZE; y++)
                    {
                        for(int x = 0; x < TETROMINO_SIZE; x++)
                        {   
                            const int offset = y * TETROMINO_SIZE + x;

                            const int *tetromino = tetrominoTypes[currentTetrominoType][currentRotation];

                            if(tetromino[offset] == 1)
                            {
                                const int offset_stage = (y + currentTetrominoY) * STAGE_WIDTH + (x + currentTetrominoX);

                                stage[offset_stage] = currentColor+1;
                            }
                        }
                    }

                    DeleteLines();

                    enable_circle_effect((currentTetrominoX + 1) * TILE_SIZE + startOffsetX,(currentTetrominoY + 3) * TILE_SIZE + startOffsetY,0);

                    currentTetrominoX = tetrominoStartX;
                    currentTetrominoY = tetrominoStartY;

                    currentTetrominoType = GetRandomValue(0, 6);
                    currentRotation = 0;
                    currentColor = GetRandomValue(0, 7);

                    float new_tretomino_speed =  tretomino_speed + .5f;
                    if (new_tretomino_speed > 5)
                    {
                        new_tretomino_speed = 5;
                    }
                    tretomino_speed = new_tretomino_speed;
                    score++;

                    if(CheckCollision(currentTetrominoX,currentTetrominoY+1,tetrominoTypes[currentTetrominoType][currentRotation]))
                    {
                        StopMusicStream(music_loop);
                        PlayClipAudio(finish_game_audio);
                        game_over = 1;
                    }
            }
        }
        }
        else //GameOver
        {
            if (IsKeyPressed(KEY_SPACE)){
               ResetGame();
            }
        }
       //Draw
       draw_all(startOffsetX,startOffsetY);
    }
    CloseWindow();
    return 0;
}