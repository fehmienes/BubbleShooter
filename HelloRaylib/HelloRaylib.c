#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>


#define MAX_MOVING_BUBBLES 40
#define MAX_STATIC_BUBBLE 400

typedef enum {
    screenMenu,
    screenScore,
    screenInfo,
    screenLevels,
    screenWon,
    screenGame,
    screenOver,
    screenPause
} GameScreen;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool isMoving;
    bool isVisible;
    bool isChecked;
    bool isFalling;
    bool isScored;
    int colorNum;
    Color color;
    Color secondColor;
} Bubble;

typedef struct {
    Vector2 position;
    Vector2 size;
    Color colorBtn;
    Color colorText;
    char* text;
    int fontSize;
} Button;

void addStaticBalls(Bubble* movingBall, Bubble staticBalls[]);
bool isCollision(Bubble* movingBall, Bubble* staticBall);
bool isExplosion(Bubble* bubble, Bubble staticBalls[]);
void createNewBall(Bubble* newBubble, int colorNum);
void drawScreen(Bubble* movingBalls, Bubble staticBalls[], Bubble *secondBall);
void waitTimer(double second);
bool checkFreeBall(Bubble* firstBall, Bubble staticBalls[]);
void checkReset(Bubble staticBalls[]);
void createButton(Button buttonInfo, int spaceX);
bool isButtonClicked(Button button);
void clearArray(int* array, int size);
int indexCounter(int* a, int sizeOfArray);
int currenBallCounter(Bubble staticBalls[]);
int calculateScore(Bubble staticBalls[]);
void updateBalls(Bubble staticBalls[]);
void currentBallColors(int currenColors[], Bubble staticBalls[]);
void newLevel(int arrayOfLevel[], int level, Bubble staticBalls[]);
void resetStaticBalls(Bubble staticBalls[]);
void addSecondBall(Bubble *secondBall, int colorNum);
void swapColors(Bubble* firstBall, Bubble* secondBall);
void drawScoreBar(Bubble staticBalls[], Rectangle scoreBar, Texture2D freeStar, Texture2D goldStar);

int staticBallNum = 0;
int movingBallsNum = 0;
const int radius = 20;
const float speed = 12;
const int screenWidth = 600;
const int screenHeight = 950;
Vector2 startingPosition = { 300, 831 };
int nextBallNum = 0;
int expQue[200];
int queCounter = 0;
int numberOfGameBalls = MAX_STATIC_BUBBLE;
bool isBallReady = true;

int main(void)
{
    int arrayOfLevel[MAX_STATIC_BUBBLE];
    clearArray(arrayOfLevel,MAX_STATIC_BUBBLE);

    InitWindow(screenWidth, screenHeight, "Bubble Shooter");
    InitAudioDevice();
    
    int scoreTableValues[10][2] = { {10000,10},{9400,8}, {8000,7}, {7800,7}, {7500,7}, {7200,7}, {6600,5}, {3800,4}, {2000,2}, {1200,1} };
    
    clearArray(expQue,200);

    GameScreen currentScreen = screenMenu;

    Texture2D background = LoadTexture("arkaplan.png");
    Texture2D mainMenu = LoadTexture("mainmenu.png");
    Texture2D scoreTable = LoadTexture("scoreboard.png");
    Texture2D returnMenu = LoadTexture("returnMenu.png");
    Texture2D infoPage = LoadTexture("infoPage.png");
    Texture2D topBar = LoadTexture("topBar.png");
    Texture2D changeBall = LoadTexture("changeBall.png");
    Texture2D freeStar = LoadTexture("freeStar.png");
    Texture2D goldStar = LoadTexture("goldStar.png");
    Texture2D pauseScreen = LoadTexture("pauseScreen.png");


    Sound bubbleExpSound = LoadSound("pop.mp3");
    Sound clickSound = LoadSound("clickSound.mp3");
    Sound music = LoadSound("metin2.mp3");
    Sound trink = LoadSound("trink.mp3");

    Bubble movingBalls[MAX_MOVING_BUBBLES];
    Bubble staticBalls[MAX_STATIC_BUBBLE + MAX_MOVING_BUBBLES];
    Bubble secondBall;

    Color textColor = WHITE;

    Rectangle scoreBar = {
        .x = 200,
        .y = 60,
        .width = 200,
        .height = 8
    };

    movingBalls[nextBallNum] = (Bubble){ {screenWidth / 2, screenHeight * 7 / 8}, {0,0}, false, true ,0, 0 , 0,0, BLUE, SKYBLUE };
    secondBall = (Bubble){ {startingPosition.x, startingPosition.y +80}, {0,0}, false, true ,0, 0 , 0, 1, RED, PINK };

    int level = 1;
    int score[5] = { 0,0,0,0,0 };
    int counter = 0;
    int arrayOfCurrenColors[3] = { -1, -1, -1 };
    bool isUpdateStart = false;
    bool isExplosionStart = false;
    bool isFallingStart = false;
    bool isOrganised = false;
    bool giveNewBall = false;
    bool islevelUp = true;
    bool createBall = false;
    float timeForWaiting = 0.0f;

    Button infoBtn = {
        .position = (Vector2) { 160, 800},
        .size = (Vector2) {280,45},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "INFORMATION"
    };
    Button playBtn = {
        .position = (Vector2) { 160, 680},
        .size = (Vector2) {280,45},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "PLAY"
    };
    Button quitBtn = {
        .position = (Vector2) { 160, 860},
        .size = (Vector2) {280,45},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "QUIT"
    };
    Button scoreBtn = {
        .position = (Vector2) { 160, 740},
        .size = (Vector2) {280,45},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "SCORES"
    };
    Button continueGame = {
        .position = (Vector2) { screenWidth / 2 - 125, screenHeight / 2 - 127},
        .size = (Vector2) {250,50},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "Resume"
    };
    Button restartGame = {
    .position = (Vector2) { screenWidth / 2 - 125, screenHeight / 2 - 47},
    .size = (Vector2) {250,50},
    .fontSize = 30,
    .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
    .colorText = BLACK,
    .text = "Restart"
    };
    Button levelsOnPause = {
        .position = (Vector2) { screenWidth / 2 - 125, screenHeight / 2 + 33},
        .size = (Vector2) {250,50},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "Information"
    };
    Button backMenu = {
        .position = (Vector2) { screenWidth / 2 - 125, screenHeight / 2 + 113},
        .size = (Vector2) {250,50},
        .fontSize = 30,
        .colorBtn = ColorFromHSV(0.0f, 0.0f, 0.6f),
        .colorText = BLACK,
        .text = "Menu"
    };

    newLevel(arrayOfLevel, level, staticBalls);
    SetTargetFPS(60);

    //PlaySound(music);
    while (!WindowShouldClose()) {
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (currentScreen == screenMenu) {
            DrawTexture(mainMenu, 0, 0, WHITE);
            createButton(playBtn, 105);
            createButton(scoreBtn, 80);
            createButton(infoBtn, 32);
            createButton(quitBtn, 105);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(playBtn)) {
                PlaySound(clickSound);
                currentScreen = screenGame;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(infoBtn)) {
                PlaySound(clickSound);
                currentScreen = screenInfo;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(quitBtn)) {
                PlaySound(clickSound);
                CloseWindow();
                return 0;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(scoreBtn)) {
                PlaySound(clickSound);
                currentScreen = screenScore;
            }
        }
        else if (currentScreen == screenInfo) {
            DrawTexture(infoPage, 0, 0, WHITE);
            DrawTexture((returnMenu), 10, 10, WHITE);
            DrawText(TextFormat("HOW TO PLAY"), 170, 25, 40, WHITE);
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x > 20 && mousePos.x < 65 && mousePos.y > 20 && mousePos.y < 65 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentScreen = screenMenu;
            }
        }
        else if (currentScreen == screenScore) {

            DrawTexture(scoreTable, 0, 0, WHITE);
            DrawTexture((returnMenu), 10, 10, WHITE);
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x > 20 && mousePos.x < 65 && mousePos.y > 20 && mousePos.y < 65 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentScreen = screenMenu;
            }
            Color textColor = GOLD;
            for (int i = 0; i < 10;i++) {
                switch (i)
                {
                case 0:
                    textColor = ColorFromHSV(50.0f, 0.9f, 1.0f);
                    break;
                case 1:
                    textColor = WHITE;
                    break;
                case 2:
                    textColor = ColorFromHSV(25.0f, 0.75f, 0.65f);
                    break;
                default:
                    textColor = ColorFromHSV(0.0f, 0.0f, 0.75f);
                    break;
                }
                DrawText(TextFormat("%d. %d POINT %d.LEVEL", i + 1, scoreTableValues[i][0], scoreTableValues[i][1]), 55, 143 + 81 * i, 40, textColor);
                
            }

            DrawText(TextFormat("SCORES"), 210, 25, 48, WHITE);
        }
        else if (currentScreen == screenPause) {

            DrawTexture(pauseScreen, 0, 0, WHITE);
            DrawText(TextFormat("PAUSE"), screenWidth / 2 - 65, screenHeight / 2 - 213, 40, WHITE);
            createButton(continueGame, 75);
            createButton(backMenu, 85);
            createButton(levelsOnPause, 40);
            createButton(restartGame, 70);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(continueGame)) {
                PlaySound(clickSound);
                currentScreen = screenGame;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(backMenu)) {
                PlaySound(clickSound);
                currentScreen = screenMenu;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(levelsOnPause)) {
                PlaySound(clickSound);
                currentScreen = screenLevels;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(restartGame)) {
                PlaySound(clickSound);
                isUpdateStart = false;
                isExplosionStart = false;
                isFallingStart = false;
                isOrganised = false;
                giveNewBall = false;
                islevelUp = false;
                createBall = true;
                clearArray(expQue, 200);
                queCounter = 0;
                counter = 0;
                nextBallNum = 0;
                createNewBall(&movingBalls[nextBallNum], GetRandomValue(0, 2));
                newLevel(arrayOfLevel, level, staticBalls);
                numberOfGameBalls = MAX_STATIC_BUBBLE + 1;
                currentScreen = screenGame;
            }
        }
        else if (currentScreen == screenOver) {
            DrawTexture(pauseScreen, 0, 0, WHITE);
        }
        else if (currentScreen == screenLevels) {

        }
        else if (currentScreen == screenWon);
        else if (currentScreen == screenGame) {
            DrawTexture(background, 0, 0, WHITE);
            DrawText(TextFormat("Top Sayisi: %d, %d", nextBallNum, movingBalls[nextBallNum].colorNum), 5, screenHeight - 35, 30, DARKGRAY);
            DrawTexture(changeBall, 236, startingPosition.y - 23, WHITE);
            //DrawText(TextFormat("%d",movingBallsNum), startingPosition.x - 7, startingPosition.y + 30, 20, (Color) { 230, 220, 255, 255 } );
            DrawFPS(10, 10);
            if (!movingBalls[nextBallNum].isMoving) {
                if (IsKeyPressed(KEY_R) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMousePosition().y > 800)) {
                    swapColors(&movingBalls[nextBallNum], &secondBall);
                }
            }
            if (IsKeyPressed(KEY_P)) {
                currentScreen = screenPause;
            }
            if (IsKeyPressed(KEY_K)) {
                printf("nextbull num = %d ve colornum = %d ve isVisible = %d\n",nextBallNum, movingBalls[nextBallNum].colorNum, movingBalls[nextBallNum].isVisible);
                for (int i = 0; i < MAX_MOVING_BUBBLES + MAX_STATIC_BUBBLE; i++)
                {
                    printf("%d, ", staticBalls[i].isVisible);
                }
            }
            if (isCollision(&movingBalls[nextBallNum], staticBalls)) {
                movingBallsNum--;
                timeForWaiting = 0.3f;
                isExplosionStart = true;
                addStaticBalls(&movingBalls[nextBallNum], staticBalls);
            }
            if (isExplosionStart) {
                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.5f) {
                    if (isExplosion(&staticBalls[MAX_STATIC_BUBBLE + nextBallNum], staticBalls)) {
                        for (int i = 0; i < numberOfGameBalls; i++) {
                            if (expQue[i] == -1) {
                                break;
                            }
                            isExplosion(&staticBalls[expQue[i]], staticBalls);
                        }
                        for (int i = 0; i < 200; i++) {
                            printf("%d ,", expQue[i]);
                        }
                        printf("\n");
                    }
                    queCounter = 0;
                    checkReset(staticBalls);
                    isExplosionStart = false;
                    isFallingStart = true;
                }
            }
            if (isFallingStart) {
                if (indexCounter(expQue, 200) >= 3) {

                    timeForWaiting += GetFrameTime();
                    if (staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isVisible) {
                        PlaySound(bubbleExpSound);
                    }
                    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isVisible = false;
                    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isScored = true;

                    if (timeForWaiting >= 0.6f) {
                        if (expQue[counter] != -1) {

                            if (!(staticBalls[expQue[counter]].isVisible)) {
                                counter++;
                            }
                            else {
                                PlaySound(bubbleExpSound);
                                staticBalls[expQue[counter]].isFalling = true;
                                staticBalls[expQue[counter]].isVisible = false;
                                staticBalls[expQue[counter]].isScored = true;
                                counter++;
                                timeForWaiting = 0.54f;
                            }
                        }
                        else {
                            counter = 0;
                            timeForWaiting = 0.50f;
                            isFallingStart = false;
                            clearArray(expQue, 200);
                            isUpdateStart = true;
                        }
                    }
                }
                else {
                    clearArray(expQue, 200);
                    createBall = true;
                    isFallingStart = false;
                    isOrganised = true;
                }
            }
            if (isUpdateStart) {

                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.60f) {
                    for (int i = 0; i < numberOfGameBalls;i++) {
                        if (!checkFreeBall(&staticBalls[i], staticBalls)) {
                            if (staticBalls[i].isVisible)
                                staticBalls[i].isScored = true;
                            staticBalls[i].isFalling = true;
                            staticBalls[i].isVisible = false;
                        }
                        checkReset(staticBalls);
                    }
                    clearArray(expQue, 200);
                    isUpdateStart = false;
                    isOrganised = true;
                }
            }
            if (isOrganised) {
                printf("\nisOrganised\n");
                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.50f) {

                    updateBalls(staticBalls);
                    int highestY = 0;
                    int lowestY = 500;
                    int ballnum = 400;
                    for (int i = 0; i < numberOfGameBalls; i++) {
                        if (staticBalls[i].isVisible && staticBalls[i].position.y > highestY)
                            highestY = staticBalls[i].position.y;
                    }
                    for (int i = 0; i < numberOfGameBalls; i++) {
                        if (staticBalls[i].isVisible && staticBalls[i].position.y < lowestY) {
                            lowestY = staticBalls[i].position.y;
                            ballnum = i;
                        }
                    }
                    if (highestY >= radius * 20 + 70 || lowestY > 75) {
                        isOrganised = false;
                        createBall = true;
                    }

                    timeForWaiting = 0.40f;
                    printf("\nBall = %d , LowestY = %d \n", ballnum, lowestY);
                }
            }
            if (createBall) {
                int hold = 1;
                printf("score1 = %d \n", calculateScore(staticBalls));
                if (currenBallCounter(staticBalls) != 0) {
                    printf("\ncreateNewwBall2\n");
                    clearArray(arrayOfCurrenColors, 3);
                    currentBallColors(arrayOfCurrenColors, staticBalls);
                    while (1) {
                        hold = GetRandomValue(0, 2);
                        if (arrayOfCurrenColors[hold] != -1) {
                            hold = arrayOfCurrenColors[hold];
                            break;
                        }
                    }
                    createNewBall(&movingBalls[++nextBallNum], secondBall.colorNum);
                    while (1) {
                        hold = GetRandomValue(0, 2);
                        if (indexCounter(arrayOfCurrenColors, 3) == 1) {
                            hold = movingBalls[nextBallNum].colorNum;
                            break;
                        }
                        if (arrayOfCurrenColors[hold] != -1 && movingBalls[nextBallNum].colorNum != arrayOfCurrenColors[hold]) {
                            hold = arrayOfCurrenColors[hold];
                            break;
                        }
                    }
                    addSecondBall(&secondBall, hold);
                    printf("\ncreateNewwBall3\n");
                    isBallReady = true;
                    createBall = false;
                }
                else {
                    if (islevelUp) {
                        textColor = GOLD;
                        PlaySound(trink);
                        islevelUp = false;
                    }
                    timeForWaiting += GetFrameTime();
                    if (timeForWaiting >= 2.00f) {
                        level++;
                        if (level <= 9) {
                            nextBallNum = 0;
                            newLevel(arrayOfLevel, level, staticBalls);
                            textColor = WHITE;
                            islevelUp = true;
                            numberOfGameBalls = MAX_STATIC_BUBBLE + 1;
                        }
                    }
                }
            }

            drawScreen(&movingBalls[nextBallNum], &staticBalls, &secondBall);
            DrawText(TextFormat("nextballnum: %d", nextBallNum), 0, 800, 16, GRAY);
            DrawTexture(topBar, 0, 0, WHITE);DrawText(TextFormat("%d", currenBallCounter(staticBalls)), 90, 35, 20, textColor);
            drawScoreBar(staticBalls, scoreBar, freeStar, goldStar);
            DrawText(TextFormat("SCORE"), 480, 25, 20, textColor);
            DrawText(TextFormat("LEVEL %d", level), 255, 15, 20, textColor);
            DrawText(TextFormat("%d", calculateScore(staticBalls)), 495, 50, 20, textColor);
            DrawText(TextFormat("numberOfGameBalls = %d", numberOfGameBalls), 15, 900, 20, textColor);
            DrawFPS(10, 10);
        }
        EndDrawing();
    }
    CloseWindow();
    CloseAudioDevice();

    return 0;
}
bool isButtonClicked(Button button) {
    Vector2 mousePos = GetMousePosition();
    if (mousePos.x >= button.position.x && mousePos.x <= button.position.x + button.size.x) {
        if (mousePos.y >= button.position.y && mousePos.y <= button.position.y + button.size.y) {
            return true;

        }
    }
    return false;
}
void createButton(Button buttonInfo, int spaceX) {
    Vector2 mousePos = GetMousePosition();
    int textPosX;
    int textPosY;
    Rectangle button = {
            .x = buttonInfo.position.x,
            .y = buttonInfo.position.y,
            .width = buttonInfo.size.x,
            .height = buttonInfo.size.y 
    };
    if (mousePos.x >= buttonInfo.position.x && mousePos.x <= buttonInfo.position.x + buttonInfo.size.x
         && mousePos.y >= buttonInfo.position.y && mousePos.y <= buttonInfo.position.y + buttonInfo.size.y) {
        buttonInfo.colorBtn = (Color){ 230, 220, 255, 255 };
        buttonInfo.colorText = ORANGE;
    }

    textPosX = buttonInfo.position.x + spaceX;
    textPosY = buttonInfo.position.y + buttonInfo.size.y / 4 ;
    DrawRectangleRounded(button, 0.6, 2000, buttonInfo.colorBtn);
    DrawRectangleRoundedLinesEx(button, 0.6, 2, 2, BLACK);
    DrawText(buttonInfo.text, textPosX, textPosY, buttonInfo.fontSize, buttonInfo.colorText);




}
void createNewBall(Bubble* newBubble, int colorNum) {
    newBubble->isScored = 0;
    newBubble->colorNum = colorNum;
    newBubble->isChecked = false;
    newBubble->position = startingPosition;
    newBubble->isVisible = true;
    newBubble->isMoving = false;
    newBubble->isFalling = false;
    newBubble->velocity = (Vector2){ 0,0 };
    switch (newBubble->colorNum)
    {
    case -1:
        newBubble->color = YELLOW;
        newBubble->secondColor = ORANGE;
        break;
    case 0:
        newBubble->color = BLUE;
        newBubble->secondColor = SKYBLUE;
        break;
    case 1:
        newBubble->color = RED;
        newBubble->secondColor = PINK;
        break;
    case 2:
        newBubble->color = DARKGREEN;
        newBubble->secondColor = GREEN;
        break;
    default:
        newBubble->color = BLACK;
        break;
    }


}
bool isCollision(Bubble* movingBall, Bubble* staticBall) {
    bool result = false;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (movingBall->isVisible && staticBall[i].isVisible && CheckCollisionCircles(movingBall->position, radius, staticBall[i].position, radius)) {
            movingBall->velocity.x = 0;
            movingBall->velocity.y = 0;
            movingBall->isMoving = false;
            if (movingBall->position.x <= staticBall[i].position.x) {
                //topun sol altı
                if (movingBall->position.y < staticBall[i].position.y + radius) {
                    movingBall->position.x = staticBall[i].position.x - radius * 2 - 3;
                    movingBall->position.y = staticBall[i].position.y;
                }
                //topun sol üstü
                else {
                    movingBall->position.x = staticBall[i].position.x - radius;
                    movingBall->position.y = staticBall[i].position.y + radius * 2;
                }
            }
            else {
                //topun sağ altı
                if (movingBall->position.y < staticBall[i].position.y + +radius) {
                    movingBall->position.x = staticBall[i].position.x + radius * 2 + 3;
                    movingBall->position.y = staticBall[i].position.y;
                }
                //topun sağ üstü
                else {
                    movingBall->position.x = staticBall[i].position.x + radius;
                    movingBall->position.y = staticBall[i].position.y + radius * 2;
                }
            }
            result = true;
        }
    }
    return result;
}
bool isExplosion(Bubble* firstBubble, Bubble staticBalls[]) {

    if (!firstBubble->isVisible || firstBubble->isChecked) return false;
    firstBubble->isChecked = true;
    bool exploded = false;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (!staticBalls[i].isVisible) continue;
        Vector2 range = Vector2Subtract(firstBubble->position, staticBalls[i].position);
        float distance = Vector2Length(range);
        if (distance <= radius * 2 + radius && firstBubble->colorNum == staticBalls[i].colorNum && !staticBalls[i].isChecked) {
            expQue[queCounter++] = i;
            exploded = true;
        }
    }

    if (exploded) {
        expQue[queCounter++] = -2;
    }
    return exploded;
}
void drawScreen(Bubble* movingBalls, Bubble staticBalls[], Bubble *secondBall) {

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isBallReady) {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.y < 800) {
            isBallReady = false;
            movingBalls->velocity = Vector2Subtract(mousePos, movingBalls->position);
            float lenght = Vector2Length(movingBalls->velocity);
            if (lenght != 0) {
                movingBalls->velocity.x = (movingBalls->velocity.x / lenght) * speed;
                movingBalls->velocity.y = (movingBalls->velocity.y / lenght) * speed;
                movingBalls->isMoving = true;
            }
        }
    }

    if (movingBalls->isMoving) {
        movingBalls->position = Vector2Add(movingBalls->position, movingBalls->velocity);
    }
    if (movingBalls->position.x <= 10 || movingBalls->position.x >= screenWidth - 10) {
        movingBalls->velocity.x *= -1;
    }
    if (movingBalls->position.y <= 10 || movingBalls->position.y >= screenHeight - 10) {
        movingBalls->velocity.y *= -1;
    }

    if (movingBalls->isVisible) {
        DrawCircleGradient(movingBalls->position.x, movingBalls->position.y, radius, movingBalls->secondColor, movingBalls->color);
        DrawCircleLinesV(movingBalls->position, radius, BLACK); 
    }
    if (secondBall->isVisible && secondBall->colorNum != -1) {
        DrawCircleGradient(secondBall->position.x, secondBall->position.y, radius, secondBall->secondColor, secondBall->color);
        DrawCircleLinesV(secondBall->position, radius, BLACK);
    }

    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].colorNum == -1)
            continue;

        if (staticBalls[i].isVisible) {
            DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
            DrawText((TextFormat("%d", i)), staticBalls[i].position.x, staticBalls[i].position.y, 10, BLACK);
            DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
        }
        else {
            if (staticBalls[i].isFalling) {
                if (!staticBalls[i].isMoving) {
                    staticBalls[i].velocity = (Vector2){ GetRandomValue(-2,2) ,1 };
                    staticBalls[i].isMoving = true;
                }
                if (staticBalls[i].position.y >= 1000) {
                    staticBalls[i].velocity = (Vector2){ 0,0 };
                    DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
                    DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
                }
                else {
                    staticBalls[i].position = Vector2Add(staticBalls[i].position, staticBalls[i].velocity);
                    staticBalls[i].position.y *= 1.027;
                    staticBalls[i].position.x *= 0.985;
                    DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
                    DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
                }
            }
            else {
                //BURADASIN DİREKT PATLAYAN TOPLARIN RENKLERİNİ SOLDURUP YOK EDECEKSİN
                staticBalls[i].position = Vector2Add(staticBalls[i].position, staticBalls[i].velocity);
                staticBalls[i].position.y *= 1.027;
                staticBalls[i].position.x *= 0.985;
                DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
                DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
            }

        }

    }
}
void waitTimer(double second) {
    float waitTimer = 0.5f;
}
bool checkFreeBall(Bubble* firstBall, Bubble staticBalls[]) {
    if (CheckCollisionCircleLine(firstBall->position, radius + 3, (Vector2) { 0, 120 }, (Vector2) { screenWidth, 120 })) {
        return true;
    }
    else {
        for (int i = 0; i < numberOfGameBalls; i++) {
            if (staticBalls[i].isChecked || !staticBalls[i].isVisible || staticBalls[i].colorNum == -1) {
                continue;
            }
            if (CheckCollisionCircles(firstBall->position, radius + radius/3, staticBalls[i].position, radius + radius/3)) {
                firstBall->isChecked = true;
                if (checkFreeBall(&staticBalls[i], staticBalls)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void clearArray(int* array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = -1;
    }
}
void checkReset(Bubble staticBalls[]) {
    for (int i = 0; i < numberOfGameBalls; i++) {
        staticBalls[i].isChecked = false;
    }
}
void addStaticBalls(Bubble* movingBall, Bubble staticBalls[]) {
    movingBall->isVisible = false;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].position = movingBall->position;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isScored = false;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isVisible = true;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isChecked = false;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isFalling = false;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isMoving = false;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].color = movingBall->color;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].colorNum = movingBall->colorNum;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].secondColor = movingBall->secondColor;
    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].velocity = (Vector2) { 0,0 };
    numberOfGameBalls++;
}
int indexCounter(int* array, int sizeOfArray) {
    int counter = 0;
    for (int i = 0; i < sizeOfArray; i++) {
        if (array[i] != -1) {
            counter++;
        }
        else {
            break;
        }
    }
    return counter;
}
int currenBallCounter(Bubble staticBalls[]) {
    int counter = 0;
    for (int i = 0; i < numberOfGameBalls; i++){
        if (staticBalls[i].isVisible)
            counter++;
    }
    return counter;
}
int calculateScore(Bubble staticBalls[]) {
    int score = 0;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].isScored) {
            score += 20;
        }
    }
    return score;
}
void updateBalls(Bubble staticBalls[]) {
    int highestPos = 0;
    int lowestPos = 500;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].isVisible && staticBalls[i].position.y > highestPos) {
            highestPos = staticBalls[i].position.y;
        }
    }
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].isVisible && staticBalls[i].position.y < lowestPos) {
            lowestPos = staticBalls[i].position.y;
        }
    }
    if (highestPos > 20 * radius + 70) {
        for (int i = 0; i < numberOfGameBalls; i++) {
            staticBalls[i].position.y -= radius * 2;
        }
    }
    if (highestPos < 20 * radius + 70 && lowestPos < 80) {
        for (int i = 0; i < numberOfGameBalls; i++) {
            staticBalls[i].position.y += radius * 2;
        }
    }
}
void currentBallColors(int currentColors[], Bubble staticBalls[]) {
    bool isThere = false;
    int counter = 0;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].isVisible && staticBalls[i].position.y >= 100 && staticBalls[i].colorNum != 3) {
            for (int j = 0; j < 3;j++) {
                if (currentColors[j] == staticBalls[i].colorNum) {
                    isThere = true;
                    break;
                }
            }
            if (!isThere && counter < 3) {
                currentColors[counter] = staticBalls[i].colorNum;
                counter++;
            }
            isThere = false;
        }
    }
}
void newLevel(int arrayOfLevel[], int level, Bubble staticBalls[]) {

    int staticX = radius + 1;
    int staticY = 20 * radius + 70;
    FILE* fPtr = NULL;
    int result = 1;
    int row = 0;
    clearArray(arrayOfLevel, MAX_STATIC_BUBBLE);
    switch (level){
    case 1:
        result = fopen_s(&fPtr, "level1.txt", "r");
        break;
    case 2:
        result = fopen_s(&fPtr, "level2.txt", "r");
        break;
    case 3:
        result = fopen_s(&fPtr, "level3.txt", "r");
        break;
    case 4:
        result = fopen_s(&fPtr, "level4.txt", "r");
        break;
    case 5:
        result = fopen_s(&fPtr, "level5.txt", "r");
        break;
    case 6:
        result = fopen_s(&fPtr, "level6.txt", "r");
        break;
    case 7:
        result = fopen_s(&fPtr, "level7.txt", "r");
        break;
    case 8:
        result = fopen_s(&fPtr, "level8.txt", "r");
        break;
    case 9:
        result = fopen_s(&fPtr, "level9.txt", "r");
        break;
    default:
        break;
    }
    if (result == 0) {
        printf("dosyaya ulaşıldı!");
        fscanf_s(fPtr, "%d", &staticBallNum);
        printf("\n%d", staticBallNum);
        fscanf_s(fPtr, "%d", &movingBallsNum);
        for (int i = 0; i < staticBallNum; i++) {
            fscanf_s(fPtr, "%d", &arrayOfLevel[i]);
        }
        fclose(fPtr);
    }
    else {
        printf("dosya okunamadı!");
    }

    resetStaticBalls(staticBalls);

    for (int i = 0; i < MAX_STATIC_BUBBLE; i++) {
        staticBalls[i].colorNum = arrayOfLevel[i];
        if (staticBalls[i].colorNum == -1) {
            staticBalls[i].isVisible = false;
            staticBalls[i].isScored = false;
            continue;
        }
        staticBalls[i].isScored = false;
        staticBalls[i].position.x = staticX;
        staticBalls[i].position.y = staticY;
        staticBalls[i].isFalling = false;
        staticBalls[i].isMoving = false;
        staticBalls[i].isChecked = false;
        staticBalls[i].isVisible = true;
        staticX += radius * 2 + 3;
        if (staticX > screenWidth - 5) {
            row++;
            staticY -= radius * 2;
            if (row % 2 == 0)
                staticX = radius + 1;
            else
                staticX = radius * 2 + 1;
        }
        switch (staticBalls[i].colorNum)
        {
        case 0:
            staticBalls[i].color = BLUE;
            staticBalls[i].secondColor = SKYBLUE;
            break;
        case 1:
            staticBalls[i].color = RED;
            staticBalls[i].secondColor = PINK;
            break;
        case 2:
            staticBalls[i].color = DARKGREEN;
            staticBalls[i].secondColor = GREEN;
            break;
        case 3:
            staticBalls[i].color = BLACK;
            staticBalls[i].secondColor = GRAY;
            break;
        case 4:
            staticBalls[i].isVisible = false;
            staticBalls[i].position = (Vector2){ 0,950 };
            break;
        default:
            break;
        }

    }
}
void resetStaticBalls(Bubble staticBalls[]) {
    for (int i = 0; i < MAX_STATIC_BUBBLE + MAX_MOVING_BUBBLES;i++) {
        staticBalls[i].position.x = 0; 
        staticBalls[i].position.y = 920;
        staticBalls[i].isFalling = false;
        staticBalls[i].isMoving = false;
        staticBalls[i].isScored = false;
        staticBalls[i].isChecked = false;
        staticBalls[i].isVisible = false;
        staticBalls[i].colorNum = -1;
        staticBalls[i].position = (Vector2){ 0,980 };
    }
}
void addSecondBall(Bubble *secondBall, int colorNum) {
    secondBall->isScored = 0;
    secondBall->colorNum = colorNum;
    secondBall->isChecked = false;
    secondBall->position.x = startingPosition.x;
    secondBall->position.y = startingPosition.y +80;
    secondBall->isVisible = true;
    secondBall->isMoving = false;
    secondBall->isFalling = false;
    secondBall->velocity = (Vector2){ 0,0 };
    switch (secondBall->colorNum)
    {
    case -1:
        secondBall->color = YELLOW;
        secondBall->secondColor = ORANGE;
        break;
    case 0:
        secondBall->color = BLUE;
        secondBall->secondColor = SKYBLUE;
        break;
    case 1:
        secondBall->color = RED;
        secondBall->secondColor = PINK;
        break;
    case 2:
        secondBall->color = DARKGREEN;
        secondBall->secondColor = GREEN;
        break;
    default:
        secondBall->color = BLACK;
        break;
    }
}
void swapColors(Bubble* firstBall, Bubble* secondBall) {
    Color firstHold = firstBall->color;
    Color secondHold = firstBall->secondColor;
    int numberHold = firstBall->colorNum;
    firstBall->color = secondBall->color;
    firstBall->secondColor = secondBall->secondColor;
    firstBall->colorNum = secondBall->colorNum;
    secondBall->color = firstHold;
    secondBall->secondColor = secondHold;
    secondBall->colorNum = numberHold;
}
void drawScoreBar(Bubble staticBalls[],Rectangle scoreBar, Texture2D freeStar, Texture2D goldStar) {
    
    DrawRectangleRounded(scoreBar, 6, 2, GRAY);
    int barWidth = (calculateScore(staticBalls) * 200) / 3000;
    Texture2D firstStar = freeStar;
    Texture2D secondStar = freeStar;
    Texture2D thirdStar = freeStar;

    if (calculateScore(staticBalls) > 1500 && calculateScore(staticBalls) < 2500) {
        firstStar = goldStar;
    }
    else if (calculateScore(staticBalls) >= 2500 && calculateScore(staticBalls) <= 3000) {
        firstStar = goldStar;
        secondStar = goldStar;
    }
    if (calculateScore(staticBalls) > 3000) {
        barWidth = 200;
        firstStar = goldStar;
        secondStar = goldStar;
        thirdStar = goldStar;
    }
    Rectangle fill = {
        .x = scoreBar.x,
        .y = scoreBar.y,
        .width = barWidth,
        .height = scoreBar.height
    };
    DrawRectangleRounded(fill, 10, 10, GOLD);
    DrawTexture(firstStar, scoreBar.x + 90, scoreBar.y - 10, WHITE);
    DrawTexture(secondStar, scoreBar.x + 145, scoreBar.y - 10, WHITE);
    DrawTexture(thirdStar, scoreBar.x + 185, scoreBar.y - 10, WHITE);
}
