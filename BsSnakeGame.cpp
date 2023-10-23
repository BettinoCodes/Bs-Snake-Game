#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;
Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double intervalInitial = .25;
double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool ElementInVector(Vector2 element, Vector2 element2) {

    if (element.x == element2.x && element.y == element2.y)
    {
        return true;
    }
    return false;
}


bool EventTriggered(double interval)
{      
    interval = intervalInitial;
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            DrawRectangle(offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize, green);
        }
    }

    void setSpeed()
    {   
        speedMultiplier += 1.0;
    }

    void Update()
    {
        
        Vector2 newPosition = Vector2Add(body[0], direction);

        body.push_front(newPosition);
        
        if (addSegment == false)
        {
            body.pop_back();
        }
        else {
            addSegment = false;
        }
       
    }



    void Reset()
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

class Food
{

public:
    Vector2 position;

    Food(deque<Vector2> snakeBody)
    {
        position = GenerateRandomPos(snakeBody);
    }


    void Draw()
    {
        DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize,cellSize, cellSize, RED);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class PowerUps
{   
public:
    Vector2 position;

    Snake snake = Snake();
    Food food = Food(snake.body);

    PowerUps(deque<Vector2> snakeBody, Vector2 foodPosition)
        {

            position = GenerateRandomPos2(snakeBody, foodPosition);
        }

    Vector2 GenerateRandomCell2()
    {
        float x = GetRandomValue(3, cellCount - 4);
        float y = GetRandomValue(3, cellCount - 4);
        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPos2(deque<Vector2> snakeBody, Vector2 foodPosition)
    {
        Vector2 position = GenerateRandomCell2();
        while (ElementInDeque(position, snakeBody) || ElementInVector(position, foodPosition))
        {
            position = GenerateRandomCell2();
        }
        return position;
    }

    void Draw() {
        DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize, YELLOW);
    }

};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    PowerUps powerup = PowerUps(snake.body, food.position);
    bool running = true;
    int score = 0;
    
    void powerUp1() {
        Vector2 newPosition;
        int n = 0;
        snake.addSegment == true;

        while (n < 2) {

            Vector2 newPosition = Vector2Add(snake.body[0], snake.direction);
            snake.body.push_front(newPosition);

            if (snake.addSegment)
            {
                snake.body.pop_back();
            }
            score++;
            n++;
        }
        snake.addSegment = false;
    }

    void powerUp2() {
        interval1 -= .01;
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
        powerup.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
            CheckCollisionWithPowerUp();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            
        }
    }

    void CheckCollisionWithPowerUp()
    {
        if (Vector2Equals(snake.body[0], powerup.position))
        {
            int value = GetRandomValue(0, 1); 

            if (value == 0)
            {
                powerUp1();
            }
            else 
            {
                powerUp2();
            }

            powerup.position = powerup.GenerateRandomPos2(snake.body, food.position);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        intervalInitial = .25;
      
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (EventTriggered(intervalInitial))
        {
            allowMove = true;
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
        {
            game.snake.direction = { 0, -1 };
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
        {
            game.snake.direction = { 0, 1 };
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
        {
            game.snake.direction = { -1, 0 };
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
        {
            game.snake.direction = { 1, 0 };
            game.running = true;
            allowMove = false;
        }

        ClearBackground(BLUE);
        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, darkGreen);
        DrawText("WELCOME TO B's SNAKE MANIA ", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
