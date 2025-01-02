#include <stdlib.h>
#include "raylib.h"
#include "resource_dir.h"

#define PADDLE_SPEED 400.0f
#define MAX_BALL_SPEED 5.0f

const float WIDTH = 1280.0f;
const float HEIGHT = 800.0f;

typedef struct Paddle {
	Rectangle area;
	int score;
	enum player_t {LEFT, RIGHT } player;
} Paddle;

typedef struct Ball {
	Vector2 pos;
	Vector2 angle;
	int radius;
    float speed;
} Ball;

// declare functions
void UpdatePaddle(Paddle *paddle, float dt);
void UpdateBall(Ball *b, float delta);
float RandomFloat(float Min, float Max);


int main ()
{
	SetTargetFPS(60);
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(WIDTH, 800, "Raylib PONG");
	SearchAndSetResourceDir("resources");

	// paddles
	Paddle P1;
	P1.area = (Rectangle){ 0, HEIGHT/2, 20, 110 };
	P1.score = 0;
	P1.player = LEFT;

	// paddles
	Paddle P2;
	P2.area = (Rectangle){ WIDTH-20, HEIGHT/2, 20, 110 };
	P2.score = 0;
	P2.player = RIGHT;

    // the ball
    Ball B;
    float dx, dy;
    // x is random value between 1 and 2, if 1 then +100, else -100
    if ( rand() % 2 == 1 ) { dx = 100.0f; } else { dx = -100.0f; };
    // y is random value between -50 and +50
    dy = RandomFloat(-50, 50);

    B.angle = (Vector2){ dx, dy };
    B.pos = (Vector2){ WIDTH/2, HEIGHT/2 };
    B.radius = 20.f;
    B.speed = 2.0f;

	// game loop
	while (!WindowShouldClose())
	{
		// paddle logic
		float dt = GetFrameTime();
		UpdatePaddle(&P1, dt);
		UpdatePaddle(&P2, dt);
        UpdateBall(&B, dt);
		
		BeginDrawing();

			// set background colour
			ClearBackground(GRAY);

			// draw objects
            DrawText(TextFormat("P2.area.y: %02f", P2.area.y), 200, 160, 20, BLUE);
            DrawText(TextFormat("P2.area.width: %02f", P2.area.width), 200, 140, 20, BLUE);
            DrawText(TextFormat("B.radius: %02f", B.radius), 200, 120, 20, BLUE);

			DrawRectangleRec(P1.area, RED);
			DrawRectangleRec(P2.area, BLUE);

            DrawCircle(B.pos.x, B.pos.y, B.radius, WHITE);
			
		EndDrawing();
    }

	// cleanup
	CloseWindow();
	return 0;
}

float RandomFloat(float Min, float Max) {
    return (Max - Min) * (float)rand() / (float)RAND_MAX + Min;
}

void UpdateBall(Ball *b, float delta)
{
    b->pos.x = b->pos.x + b->angle.x * (delta * b->speed);
    b->pos.y = b->pos.y + b->angle.y * (delta * b->speed);
}

void UpdatePaddle(Paddle *paddle, float delta)
{
	// paddle input
	if (paddle->player == RIGHT) {
        // handle upper/lower boundaries
		if ( IsKeyDown(KEY_UP) && (paddle->area.y >= 0) )
        {
            paddle->area.y -= PADDLE_SPEED*delta;
        }
	    if ( IsKeyDown(KEY_DOWN) && (paddle->area.y + paddle->area.height <= HEIGHT) ) 
        {
            paddle->area.y += PADDLE_SPEED*delta;
        }
	}

	if (paddle->player == LEFT) {
		if (IsKeyDown(KEY_W) && (paddle->area.y >= 0) )
        {
            paddle->area.y -= PADDLE_SPEED*delta;
        }
	    if (IsKeyDown(KEY_S) && (paddle->area.y + paddle->area.height <= HEIGHT))
        {
            paddle->area.y += PADDLE_SPEED*delta;
        }
	}

}

