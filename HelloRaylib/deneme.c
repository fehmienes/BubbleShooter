#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>


#define MAX_MOVING_BUBBLES 30
#define MAX_STATIC_BUBBLE 400

typedef enum {
    screenMenu,
    screenScore,
    screenInfo,
    screenLevel,
    screenGame,
    screenOver
} GameScreen;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool isMoving;
    bool isVisible;
    bool isChecked;
    bool isFalling;
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
void drawScreen(Bubble* movingBalls, Bubble staticBalls[]);
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
    FILE* fPtr = NULL;
    
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

    Sound bubbleExpSound = LoadSound("pop.mp3");
    Sound clickSound = LoadSound("clickSound.mp3");
    Sound music = LoadSound("metin2.mp3");
    Sound trink = LoadSound("trink.mp3");

    Bubble movingBalls[MAX_MOVING_BUBBLES];
    Bubble staticBalls[MAX_STATIC_BUBBLE + MAX_MOVING_BUBBLES];

    Color textColor = WHITE;

    movingBalls[nextBallNum] = (Bubble){ {screenWidth / 2, screenHeight * 7 / 8}, {0,0}, false, true , 0 , 0,0, BLUE, SKYBLUE };

    int level = 1;
    int expLayer = 1;
    int score = 0;
    int counter = 0;
    int arrayOfCurrenColors[3] = { -1, -1, -1 };
    int lastListOfExp[100];
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
        else if (currentScreen == screenGame) {

            DrawTexture(background, 0, 0, WHITE);

            DrawText(TextFormat("Top Sayisi: %d, %d", nextBallNum, movingBalls[nextBallNum].colorNum), 5, screenHeight - 35, 30, DARKGRAY);
            DrawText(TextFormat("%d",movingBallsNum), startingPosition.x - 7, startingPosition.y + 30, 20, (Color) { 230, 220, 255, 255 } );


            DrawFPS(10, 10);
            if (isCollision(&movingBalls[nextBallNum], staticBalls)) {
                movingBallsNum--;
                timeForWaiting = 0.3f;
                isExplosionStart = true;
                addStaticBalls(&movingBalls[nextBallNum],staticBalls);
            }

            drawScreen(&movingBalls[nextBallNum], &staticBalls);
            //expQue son hali oluştu
            if (isExplosionStart) {
                timeForWaiting += GetFrameTime();

                if (timeForWaiting >= 0.5f) {
                    if (isExplosion(&staticBalls[MAX_STATIC_BUBBLE + nextBallNum], staticBalls)) {
                        for (int i = 0; i < numberOfGameBalls; i++){
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
            //expQue yu kontrol ettikten sonra içine gir
            if (isFallingStart) {

                if (indexCounter(expQue, 200) >= 3) {
                    timeForWaiting += GetFrameTime();
                    if (staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isVisible) {
                        PlaySound(bubbleExpSound);
                    }
                    staticBalls[MAX_STATIC_BUBBLE + nextBallNum].isVisible = false;
                    
                    if (timeForWaiting >= 0.6f) {
                        clearArray(lastListOfExp,100);
                        for (int i = 0; i < 200; i++) {
                            if (expQue[i] == -2) {
                                expLayer = expLayer - 1;
                                if (expLayer == 0) {
                                    break;
                                }
                            }
                            bool isThereInList = false;
                            int hold = 0;
                            for (int j = 0; j < 100;j++) {
                                if (expQue[i] == lastListOfExp[j] || expQue[i] == -2) {
                                    isThereInList = true;
                                    break;
                                }
                                hold = i;
                            }
                            if (!isThereInList) {
                                for (int i = 0; i < 100; i++) {
                                    if (lastListOfExp[i] == -1) {
                                        lastListOfExp[i] = expQue[hold];
                                        break;
                                    }
                                }
                            }
                        }
                        expLayer = indexCounter(lastListOfExp, 100);
                        if (expLayer != 0) {
                            for (int i = 0; i < 100;i++) {
                                if (lastListOfExp[i] == -1)
                                    break;
                                staticBalls[lastListOfExp[i]].isFalling = true;
                                staticBalls[lastListOfExp[i]].isVisible = false;
                            }
                            printf("\n");
                            for (int i = 0; i < 100;i++) {
                                if(lastListOfExp[i] != -1)
                                printf("%d, ", lastListOfExp[i]);
                            }
                            printf("\n");
                            timeForWaiting = 0.50f;
                        }
                        else {
                            timeForWaiting = 0.50f;
                            isFallingStart = false;
                            isUpdateStart = true;
                        }
                        
                        /*if (expQue[counter] != -1) {
                            if (!(staticBalls[expQue[counter]].isVisible)) {
                                counter++;
                            }
                            else {
                                PlaySound(bubbleExpSound);
                                staticBalls[expQue[counter]].isFalling = true;
                                staticBalls[expQue[counter]].isVisible = false;
                                counter++;
                                timeForWaiting = 0.50f;
                            }
                        }

                        else {
                            counter = 0;
                            timeForWaiting = 0.50f;
                            isFallingStart = false;
                            isUpdateStart = true;
                        }*/
                    }
                }

                else {
                    clearArray(expQue,200);
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
                            staticBalls[i].isFalling = true;
                            staticBalls[i].isVisible = false;
                        }
                        checkReset(staticBalls);
                    }
                    clearArray(expQue,200);
                    isUpdateStart = false;
                    isOrganised = true;
                }
            }
            if (isOrganised) {
                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.50f) {
                    updateBalls(staticBalls);
                    int highestY = 0;
                    int lowestY = 500;
                    for (int i = 0; i < numberOfGameBalls; i++) {
                        if (staticBalls[i].isVisible && staticBalls[i].position.y > highestY)
                            highestY = staticBalls[i].position.y;
                    }
                    for (int i = 0; i < numberOfGameBalls; i++) {
                        if (staticBalls[i].isVisible && staticBalls[i].position.y < lowestY)
                            lowestY = staticBalls[i].position.y;
                    }
                    if (highestY >= radius * 20 + 70 || lowestY > 75) {
                        isOrganised = false;
                        createBall = true;
                    }
                    timeForWaiting = 0.40f;
                }
            }
            if (createBall) {
                clearArray(arrayOfCurrenColors, 3);
                currentBallColors(arrayOfCurrenColors, staticBalls);
                int hold = 1;
                if (currenBallCounter(staticBalls) != 0) {
                    while (1) {
                        hold = GetRandomValue(0, 2);
                        if (arrayOfCurrenColors[hold] != -1) {
                            hold = arrayOfCurrenColors[hold];
                            break;
                        }
                    }
                    createNewBall(&movingBalls[++nextBallNum], hold);
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
                    if (timeForWaiting >= 1.30f) {

                        level++;
                        if (level <= 9) {
                            newLevel(arrayOfLevel, level, staticBalls);
                            nextBallNum = 0;
                            textColor = WHITE;
                            islevelUp = true;
                        }
                    }  
                }
                
            }
            DrawTexture(changeBall, 281, startingPosition.y + 24, WHITE);
            DrawTexture(topBar, 0, 0, WHITE);DrawText(TextFormat("%d", currenBallCounter(staticBalls)), 90, 35, 20, textColor);
            DrawText(TextFormat("SCORE"), 480, 25, 20, textColor);
            DrawText(TextFormat("LEVEL %d", level), 255, 15, 20, textColor);
            DrawText(TextFormat("%d", calculateScore(staticBalls)), 495, 50, 20, textColor);
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
        buttonInfo.colorText = BLACK;
    }

    textPosX = buttonInfo.position.x + spaceX;
    textPosY = buttonInfo.position.y + buttonInfo.size.y / 4 ;
    DrawRectangleRounded(button, 0.6, 2000, buttonInfo.colorBtn);
    DrawRectangleRoundedLinesEx(button, 0.6, 2, 2, BLACK);
    DrawText(buttonInfo.text, textPosX, textPosY, buttonInfo.fontSize, buttonInfo.colorText);




}
void createNewBall(Bubble* newBubble, int colorNum) {
    newBubble->colorNum = colorNum;
    newBubble->isChecked = false;
    newBubble->position = startingPosition;
    newBubble->isVisible = true;
    newBubble->isMoving = false;
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
void drawScreen(Bubble* movingBalls, Bubble staticBalls[]) {

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
        else {
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
                if (staticBalls[i].position.y > screenHeight + radius * 2) {
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
    int counter = 0;
    for (int i = 0; i < numberOfGameBalls; i++) {
        if (staticBalls[i].isVisible)
            counter += 1;
    }
    return (staticBallNum - counter)*100;
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
        if (staticBalls[i].isVisible && staticBalls[i].position.y >= 120 && staticBalls[i].colorNum != 3) {
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
    }
    else {
        printf("dosya okunamadı!");
    }
    for (int i = 0; i < MAX_STATIC_BUBBLE; i++) {
        staticBalls[i].colorNum = arrayOfLevel[i];
        if (staticBalls[i].colorNum == -1) {
            staticBalls[i].isVisible = false;
            continue;
        }
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
