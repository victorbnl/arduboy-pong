// ==============================
//     Includes
// ==============================

// Includes {
#include "Arduboy2.h"
#include <ArduboyTones.h>
// }

// ==============================
//     Variables
// ==============================

// Arduboy {
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
// }

// Constants {
#define SCALE 3

#define BALL_SPEED 0.25

#define PAD_SPEED 0.5

#define PAD_HEIGHT (7 * SCALE)
#define PAD_WIDTH SCALE

#define BALL_HEIGHT SCALE
#define BALL_WIDTH SCALE

#define Y_MIN 0
#define Y_MAX (HEIGHT - PAD_HEIGHT)

#define MIDDLE_LINE_WIDTH 1

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define BUTTON_DELAY 200
// }

// Variables {
int current_frame; // 0: menu, 1: game

float paddleAY;
float paddleBY;

float ballX;
float ballY;
float ballVx;
float ballVy;

int Ascore;
int Bscore;

bool running;

bool gameOverTonePlayed;
// }

// ==============================
//     Main functions
// ==============================

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(10);

  Serial.begin(9600);

  reset();
}

void loop() {
  if (current_frame == 0) {
    menuLoop();
  } else if (current_frame == 1) {
    gameLoop();
  }
}

// ==============================
//     Frame functions
// ==============================

void menuLoop() {
  if (arduboy.pressed(A_BUTTON)) {
    current_frame++;
    delay(BUTTON_DELAY);
  }
  if (arduboy.pressed(UP_BUTTON)) {
    arduboy.audio.on();
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    arduboy.audio.off();
  }

  arduboy.clear();
  arduboy.setCursor(WIDTH/2-CHAR_WIDTH*10, HEIGHT/2-CHAR_HEIGHT*2);
  arduboy.println("=== Arduboy Pong ===");
  arduboy.setCursor(WIDTH/2-CHAR_WIDTH*8, HEIGHT/2-CHAR_HEIGHT*1);
  arduboy.println("Press A to start");
  arduboy.setCursor(WIDTH/2-CHAR_WIDTH*6, HEIGHT/2+CHAR_HEIGHT*0);
  arduboy.println("Up: audio on");
  arduboy.setCursor(WIDTH/2-CHAR_WIDTH*7, HEIGHT/2+CHAR_HEIGHT*1);
  arduboy.println("Down: audio off");
  arduboy.display();
}

void gameLoop() {
  // Pause
  if (arduboy.pressed(A_BUTTON)) {
    if (running) {
      running = false;
      arduboy.setCursor(WIDTH/2-CHAR_WIDTH*2.5, HEIGHT/2-CHAR_HEIGHT);
      arduboy.print("Pause");
      arduboy.display();
    } else {
      running = true;
    }
    delay(BUTTON_DELAY);
  }

  // Reset
  if (arduboy.pressed(B_BUTTON)) {
    reset();
  }

  // Game over?
  if (ballX < 0 || ballX > WIDTH) {
    running = false;
    arduboy.setCursor(WIDTH/2-CHAR_WIDTH*4.5, HEIGHT/2-CHAR_HEIGHT);
    arduboy.print("Game over");
    arduboy.display();
    arduboy.setRGBled(5, 0, 0);
    if (!gameOverTonePlayed) {
      sound.tone(200, 750);
      gameOverTonePlayed = true;
    }
    delay(1000);
    reset();
  }

  if (running) {
    // Controls
    if (arduboy.pressed(UP_BUTTON) && (paddleAY > Y_MIN)) {
      paddleAY -= PAD_SPEED;
    }
    if (arduboy.pressed(DOWN_BUTTON) && (paddleAY < Y_MAX)) {
      paddleAY += PAD_SPEED;
    }

    // Move paddle B
    if (ballY-PAD_HEIGHT/2 < 0) {
      paddleBY = 0;
    } else if (ballY+PAD_HEIGHT/2 >= HEIGHT) {
      paddleBY = HEIGHT-PAD_HEIGHT;
    } else {
      paddleBY = ballY-PAD_HEIGHT/2;
    }

    // Move ball
    ballX += ballVx;
    ballY += ballVy;
    
    // The ball touches paddle A
    if (ballX < PAD_WIDTH && (ballY >= paddleAY && ballY <= paddleAY+PAD_HEIGHT)) {
      ballVx = BALL_SPEED;
      Ascore++;
      sound.tone(300, 100);
    }
  
    // The ball touches paddle B
    if (ballX > WIDTH-PAD_WIDTH && (ballY >= paddleBY && ballY <= paddleBY+PAD_HEIGHT)) {
      ballVx = BALL_SPEED * -1;
      Bscore++;
      sound.tone(400, 100);
    }

    // The ball touches the top & bottom borders
    if (ballY <= 0 || ballY >= HEIGHT) {
      ballVy = ballVy * -1;
    }
  
    // Drawings
    arduboy.clear();
    // Draw middle line
    arduboy.fillRect(WIDTH/2-MIDDLE_LINE_WIDTH/2-1, 0, MIDDLE_LINE_WIDTH, HEIGHT, 1);
    // Draw score A
    int digits = 0;
    if (Ascore == 0) {
      digits = 1;
    } else {
      int tmp = Ascore;
      while (tmp) {
        tmp /= 10;
        digits++;
      }
    }
    arduboy.setCursor(WIDTH/2-CHAR_WIDTH*digits-SCALE, SCALE);
    arduboy.print(Ascore);
    // Draw score B
    arduboy.setCursor(WIDTH/2+SCALE, SCALE);
    arduboy.print(Bscore);
    // Draw pad A
    arduboy.fillRect(0, paddleAY, PAD_WIDTH, PAD_HEIGHT, 1);
    // Draw pad B
    arduboy.fillRect(WIDTH-PAD_WIDTH, paddleBY, PAD_WIDTH, PAD_HEIGHT, 1);
    // Draw ball
    arduboy.fillRect(ballX-BALL_WIDTH/2, ballY, BALL_HEIGHT, BALL_WIDTH, 1);
    arduboy.display();
  }
}

// ==============================
//     Utility functions
// ==============================

void reset() {
  ballY = HEIGHT/2;
  ballX = WIDTH/2-BALL_WIDTH/2;
  
  ballVx = 0;
  ballVy = 0;
  
  delay(100);
  ballVx = BALL_SPEED;
  ballVy = BALL_SPEED;
  
  ballVx = BALL_SPEED;
  
  // Random ball y direction
  if (rand() % 2 == 1) {
    ballVy = BALL_SPEED;
  } else {
    ballVy = BALL_SPEED * -1;
  }
  
  Ascore = 0;
  Bscore = 0;
  
  arduboy.setRGBled(0, 0, 0);
  
  gameOverTonePlayed = false;
  
  running = true;
}
