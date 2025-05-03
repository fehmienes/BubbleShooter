#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define NUMBER_OF_STATIC_BALLS 80


typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool isMoving;
    bool isVisible;
    bool isChecked;
    int colorNum;
    Color color;
    Color secondColor;
} Bubble;

bool isCollision(Bubble* movingBall, Bubble* staticBall);
bool isExplosion(Bubble* bubble, Bubble staticBalls[]);
void createNewBall(Bubble* newBubble, int colorNum);
void drawScreen(Bubble* movingBalls, Bubble staticBalls[]);
void waitTimer(double second);
bool checkFreeBall(Bubble* firstBall, Bubble staticBalls[]);
void checkReset(Bubble staticBalls[]);

int staticBallNumber = 0;
const int radius = 15;
const float speed = 3.5;
const int screenWidth = 600;
const int screenHeight = 950;
Vector2 startingPosition = { 300, 831};
int nextBallNum = 0;


int main(void)
{

    InitWindow(screenWidth, screenHeight, "Raylib Top Fırlatmaca");
    
    
    Bubble movingBalls[10] = { {screenWidth / 2, screenHeight * 7 / 8}, {0,0}, false, false, 0 };
    Bubble staticBalls[NUMBER_OF_STATIC_BALLS] = {{screenWidth / 2, screenHeight / 2}, {0,0} , false, false , 0};

    movingBalls[nextBallNum] = (Bubble){ {screenWidth / 2, screenHeight * 7 / 8}, {0,0}, false, true , 0 , 0, BLUE, SKYBLUE};

    int staticX = radius;
    int staticY = 60;
    int row = 0;
    bool isExplosionStart = false;
    float timeForWaiting;


    for (int i = 0; i < NUMBER_OF_STATIC_BALLS - 2; i++) {
        staticBalls[i].position.x = staticX;
        staticBalls[i].position.y = staticY;
        staticBalls[i].colorNum = GetRandomValue(0,2);
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
        staticX += radius*2 + 2;
        if (staticX >= screenWidth - radius * 2) {
            row++;
            staticY += radius * 2;
            if (row % 2 == 0)
                staticX = radius;
            else
                staticX = radius*2;
        }
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()){

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            movingBalls[nextBallNum].velocity = Vector2Subtract(mousePos,movingBalls[nextBallNum].position);
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
        if (movingBalls[nextBallNum].position.x <= 10 || movingBalls[nextBallNum].position.x >= screenWidth - 10){
            movingBalls[nextBallNum].velocity.x *= -1;
        }

        if (movingBalls[nextBallNum].position.y <= 10 || movingBalls[nextBallNum].position.y >= screenHeight - 10) {
            movingBalls[nextBallNum].velocity.y *= -1;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("Top Sayisi: %d, %d", nextBallNum, movingBalls[nextBallNum].colorNum), 5, screenHeight - 35, 30, DARKGRAY);
        DrawFPS(10, 10);
        if (isCollision(&movingBalls[nextBallNum], &staticBalls)) {
            timeForWaiting = 0.5f;
            isExplosionStart = true;
            //drawScreen(&movingBalls[nextBallNum], staticBalls);

        }
        if (isExplosionStart) {
            timeForWaiting -= GetFrameTime();
            if (timeForWaiting <= 0.0f) {
                isExplosion(&movingBalls[nextBallNum], staticBalls);
                for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++){
                    if (!checkFreeBall(&staticBalls[i], staticBalls)) {
                        staticBalls[i].isVisible = false;
                    }
                    
                    checkReset(staticBalls);
                }
                nextBallNum++;
                isExplosionStart = false;
                createNewBall(&movingBalls[nextBallNum], GetRandomValue(0,2));
            }
        }
        drawScreen(&movingBalls[nextBallNum], &staticBalls);
        
        
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
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
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++){
        if (movingBall->isVisible && staticBall[i].isVisible) {
            if (CheckCollisionCircles(movingBall->position, radius, staticBall[i].position, radius)) {
                movingBall->velocity.x = 0;
                movingBall->velocity.y = 0;
                movingBall->isMoving = false;
                if (movingBall->position.x <= staticBall[i].position.x) {
                    //topun sol altı
                    if (movingBall->position.y < staticBall[i].position.y + radius) {
                        movingBall->position.x = staticBall[i].position.x - radius * 2 - 2;
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
                        movingBall->position.x = staticBall[i].position.x + radius * 2 + 2;
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
    }
    return result;
}
bool isExplosion(Bubble* firstBubble, Bubble staticBalls[]) {
    if (!firstBubble->isVisible) return false;

    bool exploded = false;

    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
        if (!staticBalls[i].isVisible) continue;
        Vector2 range = Vector2Subtract(firstBubble->position, staticBalls[i].position);
        float distance = Vector2Length(range);

        if (distance <= radius * 2 + 5 && firstBubble->colorNum == staticBalls[i].colorNum) {

            firstBubble->isVisible = false;
            isExplosion(&staticBalls[i], staticBalls);
            exploded = true;
        }
    }
    return exploded;
}
void drawScreen(Bubble* movingBalls, Bubble staticBalls[]) {
    DrawLine(0, 60 - radius, screenWidth, 60 - radius, BLANK);
    if (movingBalls->isVisible) {
        DrawCircleGradient(movingBalls->position.x, movingBalls->position.y, radius, movingBalls->secondColor, movingBalls->color);
        DrawCircleLinesV(movingBalls->position, radius, BLACK);

    }
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
        if (staticBalls[i].isVisible) {
            DrawCircleGradient(staticBalls[i].position.x, staticBalls[i].position.y, radius, staticBalls[i].secondColor, staticBalls[i].color);
            DrawCircleLinesV(staticBalls[i].position, radius, BLACK);
        }
    }
}
void waitTimer(double second) {
    float waitTimer = 0.5f;
}
bool checkFreeBall(Bubble* firstBall, Bubble staticBalls[]) {
    if (CheckCollisionCircleLine(firstBall->position, radius + 3, (Vector2) { 0, 60 }, (Vector2) { screenWidth, 60 })) {
        return true;
    }
    else{
        for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++) {
            if (staticBalls[i].isChecked || !staticBalls[i].isVisible) {
                continue;
            }
            if (CheckCollisionCircles(firstBall->position, radius + 3, staticBalls[i].position, radius + 3)) {
                firstBall->isChecked = true;
                if (checkFreeBall(&staticBalls[i], staticBalls)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void checkReset(Bubble staticBalls[]) {
    for (int i = 0; i < NUMBER_OF_STATIC_BALLS; i++){
        staticBalls[i].isChecked = false;
    }
}