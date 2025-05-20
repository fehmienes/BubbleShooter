#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define NUMBER_OF_STATIC_BALLS 180
#define NUMBER_OF_MOVING_BALLS 15


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

void addStaticBalls(Bubble* movingBall);
bool isCollision(Bubble* movingBall, Bubble* staticBall);
bool isExplosion(Bubble* bubble, Bubble staticBalls[]);
void createNewBall(Bubble* newBubble, int colorNum);
void drawScreen(Bubble* movingBalls, Bubble staticBalls[]);
void waitTimer(double second);
bool checkFreeBall(Bubble* firstBall, Bubble staticBalls[]);
void checkReset(Bubble staticBalls[]);
void createButton(Button buttonInfo, int spaceX);
bool isButtonClicked(Button button);
void clearArray(int* array);
int indexCounter(int* a, int sizeOfArray);
int currenBallCounter(Bubble staticBalls[]);
int calculateScore(Bubble staticBalls[]);
void updateBalls(Bubble staticBalls[]);

Bubble movingBalls[NUMBER_OF_MOVING_BALLS];
Bubble staticBalls[NUMBER_OF_STATIC_BALLS + NUMBER_OF_MOVING_BALLS];
int staticBallNumber = 0;
const int radius = 20;
const float speed = 12;
const int screenWidth = 600;
const int screenHeight = 950;
Vector2 startingPosition = { 300, 831 };
int nextBallNum = 0;
int expQue[NUMBER_OF_STATIC_BALLS];
int queCounter = 0;
int numberOfGameBalls = NUMBER_OF_STATIC_BALLS;

int main(void)
{
    int colorOfBubbles[NUMBER_OF_STATIC_BALLS];
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++){
        colorOfBubbles[i] = GetRandomValue(0, 2);
    }
    int scoreTableValues[10][2] = { {10000,10},{9400,8}, {8000,7}, {7800,7}, {7500,7}, {7200,7}, {6600,5}, {3800,4}, {2000,2}, {1200,1} };
    InitWindow(screenWidth, screenHeight, "Bubble Shooter");
    clearArray(expQue);
    GameScreen currentScreen = screenMenu;

    movingBalls[nextBallNum] = (Bubble){ {screenWidth / 2, screenHeight * 7 / 8}, {0,0}, false, true , 0 , 0,0, BLUE, SKYBLUE };

    Texture2D background = LoadTexture("arkaplan.png");
    Texture2D mainMenu = LoadTexture("mainmenu.png");
    Texture2D scoreTable = LoadTexture("scoreboard.png");
    Texture2D returnMenu = LoadTexture("returnMenu.png");
    Texture2D infoPage = LoadTexture("infoPage.png");
    Texture2D topBar = LoadTexture("topBar.png");
    int score = 0;
    int staticX = radius + 5;
    int staticY = 20*radius + 70;
    int row = 0;
    int counter = 0;
    bool isUpdateStart = false;
    bool isExplosionStart = false;
    bool isFallingStart = false;
    bool isOrganised = false;
    bool isBallReady = true;
    float timeForWaiting;

    Button infoBtn = {
        .position = (Vector2) { 160, 800},
        .size = (Vector2) {280,45},
        .fontSize = 30,
        .colorBtn = GRAY,
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

    //topların oluşturulması
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
        staticBalls[i].position.x = staticX;
        staticBalls[i].position.y = staticY;
        staticBalls[i].isFalling = false;
        staticBalls[i].colorNum = colorOfBubbles[i];
        staticBalls[i].isChecked = false;
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
        default:
            break;
        }
        staticBalls[i].isVisible = true;
        staticX += radius * 2 + 3;
        if (staticX > screenWidth - radius) {
            row++;
            staticY -= radius * 2;
            if (row % 2 == 0)
                staticX = radius + 5;
            else
                staticX = radius * 2 + 5;
        }
    }

    SetTargetFPS(60);

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
                currentScreen = screenGame;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(infoBtn)) {
                currentScreen = screenInfo;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(quitBtn)) {
                CloseWindow();
                return 0;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isButtonClicked(scoreBtn)) {
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
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isBallReady) {
                isBallReady = false;
                Vector2 mousePos = GetMousePosition();
                movingBalls[nextBallNum].velocity = Vector2Subtract(mousePos, movingBalls[nextBallNum].position);
                float lenght = Vector2Length(movingBalls[nextBallNum].velocity);
                if (lenght != 0) {
                    movingBalls[nextBallNum].velocity.x = (movingBalls[nextBallNum].velocity.x / lenght) * speed;
                    movingBalls[nextBallNum].velocity.y = (movingBalls[nextBallNum].velocity.y / lenght) * speed;
                    movingBalls[nextBallNum].isMoving = true;
                }
            }

            if (movingBalls[nextBallNum].isMoving) {
                movingBalls[nextBallNum].position = Vector2Add(movingBalls[nextBallNum].position, movingBalls[nextBallNum].velocity);
            }
            if (movingBalls[nextBallNum].position.x <= 10 || movingBalls[nextBallNum].position.x >= screenWidth - 10) {
                movingBalls[nextBallNum].velocity.x *= -1;
            }

            if (movingBalls[nextBallNum].position.y <= 10 || movingBalls[nextBallNum].position.y >= screenHeight - 10) {
                movingBalls[nextBallNum].velocity.y *= -1;
            }

            DrawText(TextFormat("Top Sayisi: %d, %d", nextBallNum, movingBalls[nextBallNum].colorNum), 5, screenHeight - 35, 30, DARKGRAY);
            DrawText(TextFormat("12"), startingPosition.x - 7, startingPosition.y + 30, 20, (Color) { 230, 220, 255, 255 } );


            DrawFPS(10, 10);
            if (isCollision(&movingBalls[nextBallNum], staticBalls)) {
                if (staticBalls[0].isVisible)
                    printf("Bubble(0) is visible, %d ve %d position\n",staticBalls[0].position.x,staticBalls[0].position.y);

                timeForWaiting = 0.0f;
                isExplosionStart = true;
                addStaticBalls(&movingBalls[nextBallNum]);
            }
            drawScreen(&movingBalls[nextBallNum], &staticBalls);

            //expQue son hali oluştu
            if (isExplosionStart) {
                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.5f) {
                    printf("Bubble(0) is visible, %d ve %d position\n", staticBalls[0].position.x, staticBalls[0].position.y);
                    if (isExplosion(&staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum], staticBalls)) {
                        for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++){
                            if (expQue[i] == -1) {
                                break;
                            }
                            isExplosion(&staticBalls[expQue[i]], staticBalls);
                        }
                        for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
                            printf("%d ,", expQue[i]);
                        }
                    }
                    printf("Bubble(0) is visible, %d ve %d position\n", staticBalls[0].position.x, staticBalls[0].position.y);
                    queCounter = 0;
                    checkReset(staticBalls);
                    isExplosionStart = false;
                    isFallingStart = true;
                }
            }
            //expQue yu kontrol ettikten sonra içine gir
            if (isFallingStart) {

                if (indexCounter(expQue, NUMBER_OF_STATIC_BALLS) >= 3) {
                    timeForWaiting += GetFrameTime();
                    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].isVisible = false;
                    if (timeForWaiting >= 0.6f) {
                        if (expQue[counter] != -1) {
                            if (!(staticBalls[expQue[counter]].isVisible)) {
                                counter++;
                            }
                            else {
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
                        }
                    }
                    
                }

                else {

                    printf("Bubble(0) is visible, %d ve %d position\n", staticBalls[0].position.x, staticBalls[0].position.y);
                    clearArray(expQue);

                    printf("Bubble(0) is visible, %d ve %d position\n", staticBalls[0].position.x, staticBalls[0].position.y);
                    createNewBall(&movingBalls[++nextBallNum], GetRandomValue(0, 2));
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
                    clearArray(expQue);
                    createNewBall(&movingBalls[++nextBallNum], GetRandomValue(0, 2));
                    isUpdateStart = false;
                    isOrganised = true;
                }
            }
            if (isOrganised) {
                timeForWaiting += GetFrameTime();
                if (timeForWaiting >= 0.50f) {
                    updateBalls(staticBalls);
                    int highestY = 0;
                    for (int i = 0; i < numberOfGameBalls; i++) {
                        if (staticBalls[i].isVisible && staticBalls[i].position.y > highestY)
                            highestY = staticBalls[i].position.y;
                    }
                    if (highestY >= radius * 20 + 70) {
                        isOrganised = false;
                    }
                    timeForWaiting = 0.40f;
                    isBallReady = true;
                }
            }
            DrawTexture(topBar, 0, 0, WHITE);DrawText(TextFormat("%d", currenBallCounter(staticBalls)), 90, 35, 20, WHITE);
            DrawText(TextFormat("SCORE"), 480, 25, 20, WHITE);
            DrawText(TextFormat("LEVEL %d", 1), 255, 15, 20, WHITE);
            DrawText(TextFormat("%d", calculateScore(staticBalls)), 495, 50, 20, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();

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

    /*Vector2 mousePos = GetMousePosition();
    if (!(movingBalls->isMoving) && mousePos.y < startingPosition.y - 30 && movingBalls->isVisible) {
        Vector2 aim = Vector2Subtract(mousePos, startingPosition);
        float lenghtOfAim = Vector2Length(aim);
        aim = (Vector2){ (aim.x / lenghtOfAim) * 1,(aim.y / lenghtOfAim) * 1 };
        DrawLineV(startingPosition, aim, GRAY);
        
        DrawText(TextFormat("nextbullnum : %d", nextBallNum), 20, 550, 16, WHITE);
    }*/
    if (movingBalls->isVisible) {
        DrawCircleGradient(movingBalls->position.x, movingBalls->position.y, radius, movingBalls->secondColor, movingBalls->color);
        DrawCircleLinesV(movingBalls->position, radius, BLACK); 

    }
    for (int i = 0; i < numberOfGameBalls; i++) {

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
                    staticBalls[i].position.y *= 1.02;
                    staticBalls[i].position.x *= 0.99;
                    DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
                    DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
                }
            }
            else {
                //BURADASIN DİREKT PATLAYAN TOPLARIN RENKLERİNİ SOLDURUP YOK EDECEKSİN
                staticBalls[i].position = Vector2Add(staticBalls[i].position, staticBalls[i].velocity);
                staticBalls[i].position.y *= 1.02;
                staticBalls[i].position.x *= 0.99;
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
            if (staticBalls[i].isChecked || !staticBalls[i].isVisible) {
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
void clearArray(int* array) {
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
        array[i] = -1;
    }
}


void checkReset(Bubble staticBalls[]) {
    for (int i = 0; i < numberOfGameBalls; i++) {
        staticBalls[i].isChecked = false;
    }
}

void addStaticBalls(Bubble* movingBall) {
    numberOfGameBalls++;
    movingBall->isVisible = false;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].position = movingBall->position;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].isVisible = true;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].isChecked = false;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].isFalling = false;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].isMoving = false;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].color = movingBall->color;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].colorNum = movingBall->colorNum;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].secondColor = movingBall->secondColor;
    staticBalls[NUMBER_OF_STATIC_BALLS + nextBallNum].velocity = (Vector2) { 0,0 };
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
        if (!(staticBalls[i].isVisible))
            counter += 100;
    }
    return counter;
}
void updateBalls(Bubble staticBalls[]) {
    int highestPos = 0;
    int lowestPos = 500;
    for (int i = numberOfGameBalls - 1; i >= 0; i--) {
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
    if (highestPos < 20 * radius + 70 && lowestPos <= 70) {
        for (int i = 0; i < numberOfGameBalls; i++) {
            staticBalls[i].position.y += radius * 2;
        }
        
    }
}
