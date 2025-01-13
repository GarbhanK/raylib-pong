
#include "raylib.h"

#define PADDLE_SPEED 400.0f
#define MAX_BALL_SPEED 5.0f

bool DEBUG = false;

const float HEIGHT = 600.0f;
const float WIDTH = 800.0f;
const Vector2 CENTER = { WIDTH/2, HEIGHT/2 };
const int MAX_SCORE = 3;

enum gamestate {
    START,
    PLAY,
    PAUSE,
    WIN
} GAMESTATE;

typedef struct Paddle {
	Rectangle area;
	int score;
	enum player_t {LEFT, RIGHT } player;
} Paddle;

typedef struct Ball {
	Vector2 pos;
	Vector2 angle; // dx and dy
	int radius;
    float speed;
} Ball;

// declare functions
void HandleGamestate(Ball *b, Paddle *p1, Paddle *p2);
void UpdatePaddle(Paddle *paddle, float dt);
void ResetPaddle(Paddle *p1, Paddle *p2);
void UpdateBall(Ball *b, float delta);
void BounceBall(Ball *b, int x, int y);
void ResetBall(Ball *b);
Vector2 RandomAngle();


int main ()
{
	SetTargetFPS(60);
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "Raylib PONG");

	// paddles
	Paddle P1;
    P1 = (Paddle) {
        .area = (Rectangle){ 0, HEIGHT/2, 20, 110 },
        .score = 0,
        .player = LEFT
    };

	// paddles
	Paddle P2;
    P2 = (Paddle) {
        .area = (Rectangle){ WIDTH-20, HEIGHT/2, 20, 110 },
        .score = 0,
        .player = RIGHT
    };

    // the ball
    Ball B;
    B = (Ball) {
        .angle = RandomAngle(),
        .pos = CENTER,
        .radius = 20.0f,
        .speed = 3.0f
    };

    // start screen
    GAMESTATE = START;

    // game loop
    while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

        HandleGamestate(&B, &P1, &P2); // controls START/PLAY/PAUSE logic

		UpdatePaddle(&P1, dt);
		UpdatePaddle(&P2, dt);
        UpdateBall(&B, dt);
		
        // scoring logic
        if (B.pos.x <= 0) {
            ResetBall(&B);
            P2.score += 1;
            if (P2.score == MAX_SCORE) {
                GAMESTATE = WIN;
            }
        }
        if (B.pos.x >= WIDTH) {
            ResetBall(&B);
            P1.score += 1;
            if (P1.score == MAX_SCORE) {
                GAMESTATE = WIN;
            }
        }

        // edge of play bounce
        if ( B.pos.y <= (0 + (int)B.radius) )
        {
           BounceBall(&B, 1, -1);
        }

        if ( B.pos.y >= (HEIGHT - (int)B.radius) )
        {
            BounceBall(&B, 1, -1);
        }

        // P1 paddle bounce
        if ( 
                (B.pos.x <= P1.area.width + B.radius) && 
                (B.pos.y > P1.area.y) && 
                (B.pos.y <= P1.area.y + P1.area.height) 
            )
        {
            BounceBall(&B, -1, 1);
            B.pos.x = B.pos.x + 5;
        }

        // P2 paddle bounce
        if ( 
                (B.pos.x >= WIDTH - P2.area.width - B.radius) &&
                (B.pos.y >= P2.area.y) &&
                (B.pos.y <= P2.area.y + P2.area.height)
            )
        {
            BounceBall(&B, -1, 1);
            B.pos.x = B.pos.x - 5;
        }

		BeginDrawing();
			// set background colour
			ClearBackground(RAYWHITE);

            if (GAMESTATE == START) {
                // draw title
                DrawText("PONG!", 300, 50, 50, BLACK);

                // draw controls instructions
                const char* p1Controls = "Player 1: press W or S to move the paddle";
                const char* p2Controls = "Player 2: press UP or DOWN to move the paddle";
                const char* startMsg = "Press ENTER to start the game!";
                int p1ControlsLen = MeasureText(p1Controls, 10);
                int p2ControlsLen = MeasureText(p2Controls, 10);
                int startMsgLen = MeasureText(startMsg, 10);

                DrawText(p1Controls, CENTER.x - (p1ControlsLen/2), 110, 10, BLACK);
                DrawText(p2Controls, CENTER.x - (p2ControlsLen/2), 130, 10, BLACK);
                DrawText(startMsg, CENTER.x - (startMsgLen/2), 150, 10, BLACK);

                // TODO: have black start screen, when start game shrink it to ball
                // TODO: add custom colorschemes/themes/courts
            }

            if (GAMESTATE == PLAY) {
                int scorecardLen = MeasureText("P1: 0     P2: 0", 20);
                DrawText(
                        TextFormat("P1: %i     P2: %i", P1.score, P2.score),
                        (CENTER.x) - (scorecardLen/2), 10, 20, BLACK
                );
                
                DrawLine(CENTER.x, 0, CENTER.x, HEIGHT, BLACK);
                // TODO: other pong graphical bits, like a videoball type court
            }

            if (GAMESTATE == PAUSE) {
                int pauseLen = MeasureText("PAUSE", 50);
                DrawText("PAUSE", (CENTER.x)-(pauseLen/2), CENTER.y, 50, BLACK);
            }

            if (GAMESTATE == WIN) {
                // print win message
                int winner;
                if (P1.score > P2.score) { winner = 1; } else { winner = 2;}

                const char *winMsg = TextFormat("Player %d wins!", winner);
                int winLen = MeasureText(winMsg, 50);
                DrawText(winMsg, (CENTER.x)-(winLen/2), CENTER.y, 50, BLACK);
            }

            if (DEBUG) {
                DrawText(TextFormat("fps: %i", GetFPS()), 200, 10, 10, BLACK);
                DrawText(TextFormat("gamestate: %i", GAMESTATE), 200, 20, 10, BLACK);
                DrawText(TextFormat("ball speed: %0.2f", B.speed), 200, 30, 10, BLACK);
            }

			// draw objects
            if (GAMESTATE != WIN)
            {
                DrawRectangleRec(P1.area, RED);
                DrawRectangleRec(P2.area, BLUE);
                DrawCircle(B.pos.x, B.pos.y, B.radius, BLACK);

                // TODO: add different map with extra collision bits
                //       iterate through array of recs and draw
                //       need extra collision code for them too
            }

		EndDrawing();
    }

	// cleanup
	CloseWindow();
	return 0;
}

void UpdateBall(Ball *b, float delta)
{
    if (GAMESTATE == PLAY) {
        b->pos.x = b->pos.x + b->angle.x * (delta * b->speed);
        b->pos.y = b->pos.y + b->angle.y * (delta * b->speed);
    }
}

void BounceBall(Ball *b, int x, int y)
{
    b->angle.x = x * b->angle.x;
    b->angle.y = y * b->angle.y;
    // TODO: play bounce SFX

    if (b->speed < 6.0f)
    {
        b->speed = b->speed + 0.2f;
    }
}

void ResetBall(Ball *b)
{
    // reset angle and set ball position to center of the screen
    b->pos = CENTER;
    b->angle = RandomAngle();
}

void ResetPaddle(Paddle *p1, Paddle *p2)
{
    // reset paddles positions to the middle of their range
    p1->area.x = 0;
    p1->area.y = HEIGHT/2;
    p2->area.x = WIDTH-20;
    p2->area.y = HEIGHT/2;
}

void UpdatePaddle(Paddle *paddle, float delta)
{
	// paddle input
	if ( (paddle->player == RIGHT) && (GAMESTATE == PLAY) ) {
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

	if ( (paddle->player == LEFT) && (GAMESTATE == PLAY) ) {
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

Vector2 RandomAngle()
{
    Vector2 angles;

    // dx, randomly choose either +100 or -100
    if ( GetRandomValue(1, 2) == 1 ) {
        angles.x = 100.0f;
    } else {
        angles.x = -100.0f;
    };
    
    // dy, random value between -50 and 50
    angles.y = (float)GetRandomValue(-50, 50);

    return angles;
}

void HandleGamestate(Ball *b, Paddle *p1, Paddle *p2)
{
    if (IsKeyPressed(KEY_TAB))
    {
        if (DEBUG) { DEBUG = false; } else { DEBUG = true; }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (GAMESTATE == START) { GAMESTATE = PLAY; } else { GAMESTATE = START; }
        ResetPaddle(p1, p2);
        ResetBall(b);
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        if (GAMESTATE == PLAY) { GAMESTATE = PAUSE; } else { GAMESTATE = PLAY; }
    }
}

