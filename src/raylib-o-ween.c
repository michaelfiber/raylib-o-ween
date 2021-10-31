#include <math.h>
#include "raylib.h"
#include <stdio.h>
#include "string.h"

// GB_UP       = KEY_W = scale up 0.1 || move up
// GB_DOWN     = KEY_S = scale down 0.1 || move down
// GB_LEFT     = KEY_A = scale down 1.0 || move left
// GB_RIGHT    = KEY_D = scale up 1.0 || move right
// GB_Y        = KEY_U = add new decoration
// GB_X        = KEY_O = remove current decoration
// GB_B        = KEY_J = switch the selected decoration's sprite
// GB_A        = KEY_L = switch between MOVE, SCALE, and SELECT modes
// GB_START    = KEY_ENTER = exit the program

const int width = 800;
const int height = 480;
const int MAX_DECO = 1023;
const int TEXTURE_COUNT = 5;
const float DISPLAY_TIMEOUT_AMOUNT = 3.0f;

const int GB_UP = 0;
const int GB_DOWN = 1;
const int GB_LEFT = 2;
const int GB_RIGHT = 3;
const int GB_Y = 4;
const int GB_X = 5;
const int GB_B = 6;
const int GB_A = 7;
const int GB_START = 8;

const int MODE_SELECT = 0;
const int MODE_MOVE = 1;
const int MODE_SCALE = 2;

const int MAX_MODE = 2;

int BUTTON_MAP[9] = {
    KEY_W,
    KEY_S,
    KEY_A,
    KEY_D,
    KEY_U,
    KEY_O,
    KEY_J,
    KEY_L,
    KEY_ENTER};

bool JUST_PRESSED[9] = {0};

typedef struct Deco
{
    bool active;
    int x;
    int y;
    int type;
    float scale;
} Deco;

// GB_UP       = KEY_W = scale up 0.1 || move up
// GB_DOWN     = KEY_S = scale down 0.1 || move down
// GB_LEFT     = KEY_A = scale down 1.0 || move left
// GB_RIGHT    = KEY_D = scale up 1.0 || move right
// GB_Y        = KEY_U = add new decoration
// GB_X        = KEY_O = remove current decoration
// GB_B        = KEY_J = switch the selected decoration's sprite
// GB_A        = KEY_L = switch between MOVE, SCALE, and SELECT modes

int main(void)
{
    InitWindow(width, height, "Raylib-o-ween");
    SetTargetFPS(60);

    int currentDeco = -1;
    int decoCount = 0;
    int mode = MODE_SELECT;
    float modeDisplayTimeout = DISPLAY_TIMEOUT_AMOUNT;
    float selectorDisplayTimeout = DISPLAY_TIMEOUT_AMOUNT;
    int selectModeWidth = MeasureText("SELECT mode", 20);
    int moveModeWidth = MeasureText("MOVE mode", 20);
    int scaleModeWidth = MeasureText("SCALE mode", 20);

    SetExitKey(KEY_ENTER);

    Deco decos[1024] = {0};
    float frameTimers[1024] = {0};
    int currentFrames[1024] = {0};

    unsigned int decosSize = 1024 * sizeof(Deco);

    if (FileExists("layout.dat"))
    {
        unsigned char *loadedDecos = LoadFileData("layout.dat", &decosSize);
        for (int i = 0; i < MAX_DECO; i++)
        {
            if (((Deco *)loadedDecos)[i].active)
            {
                decoCount++;
                printf("new deco count %i\n", decoCount);
                currentDeco = i;
            }
            decos[i].active = ((Deco *)loadedDecos)[i].active;
            decos[i].scale = ((Deco *)loadedDecos)[i].scale;
            decos[i].type = ((Deco *)loadedDecos)[i].type;
            decos[i].x = ((Deco *)loadedDecos)[i].x;
            decos[i].y = ((Deco *)loadedDecos)[i].y;
        }
        UnloadFileData(loadedDecos);
    }

    printf("DONE LOADING\n");

    Texture2D textures[1024] = {0};

    // IF YOU ADD MORE, UPDATE TEXTURE_COUNT!!
    textures[0] = LoadTexture("resources/boo1.png");
    textures[1] = LoadTexture("resources/boo2.png");
    textures[2] = LoadTexture("resources/boo3.png");
    textures[3] = LoadTexture("resources/jackolantern.png");
    textures[4] = LoadTexture("resources/torch.png");
    // IF YOU ADD MORE, UPDATE TEXTURE_COUNT!!

    while (!WindowShouldClose())
    {
        bool dataChanged = false;

        float delta = GetFrameTime();

        modeDisplayTimeout -= delta;
        if (modeDisplayTimeout < 0)
            modeDisplayTimeout = 0;

        selectorDisplayTimeout -= delta;
        if (selectorDisplayTimeout < 0)
            selectorDisplayTimeout = 0;

        bool anyPressed = false;

        for (int i = 0; i < 9; i++)
        {
            if (IsKeyDown(BUTTON_MAP[i]) && !JUST_PRESSED[i])
            {
                anyPressed = true;
                printf("%i is pressed\n", i);
                switch (i)
                {
                case GB_UP:
                    if (mode == MODE_SCALE && currentDeco > -1)
                    {
                        decos[currentDeco].scale += 0.1f;
                        dataChanged = true;
                    }
                    else if (mode == MODE_MOVE && currentDeco > -1)
                    {
                        decos[currentDeco].y -= ceilf(decos[currentDeco].scale);
                        dataChanged = true;
                    }
                    break;
                case GB_DOWN:
                    if (mode == MODE_SCALE && currentDeco > -1)
                    {
                        decos[currentDeco].scale -= 0.1f;
                        if (decos[currentDeco].scale < 0.1f)
                            decos[currentDeco].scale = 0.1f;
                        dataChanged = true;
                    }
                    else if (mode == MODE_MOVE && currentDeco > -1)
                    {
                        decos[currentDeco].y += ceilf(decos[currentDeco].scale);
                        dataChanged = true;
                    }
                    break;
                case GB_LEFT:
                    printf("LEFT\n");
                    if (mode == MODE_MOVE && currentDeco > -1)
                    {
                        decos[currentDeco].x -= ceilf(decos[currentDeco].scale);
                        dataChanged = true;
                    }
                    else if (mode == MODE_SCALE && currentDeco > -1)
                    {
                        decos[currentDeco].scale -= 0.5f;
                        if (decos[currentDeco].scale < 0.1f)
                            decos[currentDeco].scale = 0.1f;
                        dataChanged = true;
                    }
                    else if (mode == MODE_SELECT && currentDeco > -1)
                    {
                        currentDeco--;
                        if (currentDeco < 0)
                            currentDeco = decoCount - 1;
                    }
                    break;
                case GB_RIGHT:
                    if (mode == MODE_MOVE && currentDeco > -1)
                    {
                        decos[currentDeco].x += ceilf(decos[currentDeco].scale);
                        dataChanged = true;
                    }
                    else if (mode == MODE_SCALE && currentDeco > -1)
                    {
                        decos[currentDeco].scale += 0.5f;
                        dataChanged = true;
                    }
                    else if (mode == MODE_SELECT && currentDeco > -1)
                    {
                        currentDeco++;
                        if (currentDeco >= decoCount)
                            currentDeco = 0;
                    }
                    break;
                case GB_X:
                    // REMOVE CURRENT DECO
                    printf("remove %i\n", currentDeco);
                    if (currentDeco > -1)
                    {
                        if (currentDeco < decoCount - 1)
                        {
                            for (int j = currentDeco + 1; j < decoCount; j++)
                            {
                                decos[j - 1].active = decos[j].active;
                                decos[j - 1].scale = decos[j].scale;
                                decos[j - 1].type = decos[j].type;
                                decos[j - 1].x = decos[j].x;
                                decos[j - 1].y = decos[j].y;
                            }
                            decoCount--;
                        }
                        else if (decoCount == 1 || currentDeco == decoCount - 1)
                        {
                            decos[0].active = false;
                            decos[0].scale = 0.0f;
                            decos[0].type = 0;
                            decos[0].x = 0;
                            decos[0].y = 0;
                            decoCount--;
                        }
                    }
                    break;
                case GB_Y:
                    // ADD NEW DECO
                    if (decoCount < MAX_DECO)
                    {
                        currentDeco = decoCount;
                        decoCount++;
                        decos[currentDeco].active = true;
                        decos[currentDeco].x = width / 2;
                        decos[currentDeco].y = height / 2;
                        decos[currentDeco].scale = 1.0f;
                        decos[currentDeco].type = 0;
                        dataChanged = true;
                    }
                    break;
                case GB_B:
                    // SWITCH CURRENT DECO TYPE
                    if (currentDeco > -1)
                    {
                        decos[currentDeco].type++;
                        if (decos[currentDeco].type >= TEXTURE_COUNT)
                            decos[currentDeco].type = 0;
                    }
                    break;
                case GB_A:
                    // SWITCH CURSOR MODE
                    mode++;
                    if (mode > MAX_MODE)
                        mode = 0;
                    modeDisplayTimeout = DISPLAY_TIMEOUT_AMOUNT;
                    break;
                case GB_START:
                    break;
                }
            }
            JUST_PRESSED[i] = IsKeyDown(BUTTON_MAP[i]);
        }

        if (anyPressed)
        {
            selectorDisplayTimeout = DISPLAY_TIMEOUT_AMOUNT;
        }

        if (dataChanged)
        {
            printf("SAVE DATA TO FILE!\n");
            SaveFileData("layout.dat", decos, sizeof(Deco) * 1024);
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);

            // draw decos:
            for (int i = 0; i < decoCount; i++)
            {
                if (frameTimers[i] > 0) frameTimers[i] -= delta;
                if (frameTimers[i] < 0) {
                    frameTimers[i] = 0;
                    currentFrames[i]++;
                    int frameCount = textures[decos[i].type].width / textures[decos[i].type].height;
                    if (currentFrames[i] >= frameCount) currentFrames[i] = 0;
                }

                if (decos[i].active)
                {
                    if (textures[decos[i].type].width > textures[decos[i].type].height && frameTimers[i] <= 0) {
                        frameTimers[i] = 0.25f;
                    }

                    DrawTexturePro(
                        textures[decos[i].type],
                        (Rectangle){textures[decos[i].type].height * currentFrames[i], 0, textures[decos[i].type].height, textures[decos[i].type].height},
                        (Rectangle){decos[i].x, decos[i].y, textures[decos[i].type].height * decos[i].scale, textures[decos[i].type].height * decos[i].scale},
                        (Vector2){textures[decos[i].type].height / 2 * decos[i].scale, textures[decos[i].type].height / 2 * decos[i].scale},
                        0.0f,
                        WHITE);
                    if (currentDeco == i && selectorDisplayTimeout > 0)
                    {
                        DrawRectangleLines(
                            decos[i].x - textures[decos[i].type].height / 2 * decos[i].scale,
                            decos[i].y - textures[decos[i].type].height / 2 * decos[i].scale,
                            textures[decos[i].type].height * decos[i].scale,
                            textures[decos[i].type].height * decos[i].scale,
                            GREEN);
                    }
                }
            }

            // draw the mode display.
            if (modeDisplayTimeout > 0.0f)
            {
                switch (mode)
                {
                case MODE_SELECT:
                    DrawText("SELECT mode", width / 2 - selectModeWidth / 2, height / 2, 20, GREEN);
                    break;
                case MODE_MOVE:
                    DrawText("MOVE mode", width / 2 - moveModeWidth / 2, height / 2, 20, GREEN);
                    break;
                case MODE_SCALE:
                    DrawText("SCALE mode", width / 2 - scaleModeWidth / 2, height / 2, 20, GREEN);
                    break;
                }
            }
        }
        EndDrawing();
    }

    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        UnloadTexture(textures[i]);
    }
    CloseWindow();
}