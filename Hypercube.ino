// Hypercube
// K. M. Kroski
// 16 Dec 2016

// Settings
#define SIZE 3
#define MOVE_DELAY 2
#define FIRE_DELAY 5
#define BULLET_COUNT 3
#define BULLET_SPEED 3
#define ROOM_COUNT 18
#define ENEMY_COUNT 24
#define ENEMY_SPEED 1
#define ENEMY_DAMAGE 10
#define AMMO_COUNT 10
#define HEALTH_COUNT 8

// Constants
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define BULLET_X 0
#define BULLET_Y 1
#define BULLET_X_SPEED 2
#define BULLET_Y_SPEED 3
#define ROOM 0
#define ACTIVE 1
#define X 2
#define Y 3

#include <Arduboy.h>

Arduboy arduboy;

// Input Buffers
byte ubuf = 0;
byte rbuf = 0;
byte dbuf = 0;
byte lbuf = 0;
byte abuf = 0;
byte bbuf = 0;

byte state = 0;

// Player
byte px = 32;
byte py = 32;
byte po = UP;
int pl = 500;
int pa = 30;

// Bullets
int bullets[BULLET_COUNT][4] = {};

// Room
int rooms[ROOM_COUNT][4] = {};
int current_room = 0;

// Enemy
int enemies[ENEMY_COUNT][4] = {};
byte enemies_active = ENEMY_COUNT;

// Ammo
int ammo[AMMO_COUNT][4] = {};

// Health
int health[HEALTH_COUNT][4] = {};

void resetGame() {
  state = 0;
  current_room = 0;
  enemies_active = ENEMY_COUNT;
  
  px = 32;
  py = 32;
  po = UP;
  pl = 500;
  pa = 30;
  
  for (int i = 0; i < BULLET_COUNT; i++) {
    bullets[i][BULLET_X] = -1;
    bullets[i][BULLET_Y] = -1;
    bullets[i][BULLET_X_SPEED] = 0;
    bullets[i][BULLET_Y_SPEED] = 0;
  }

  generateRooms();
  generateEnemies();
  generateAmmo();
  generateHealth();
}

void generateRooms() {
  for (int i = 0; i < ROOM_COUNT; i++) {
    rooms[i][UP] = -1;
    rooms[i][RIGHT] = -1;
    rooms[i][DOWN] = -1;
    rooms[i][LEFT] = -1;
  }
  
  for (int i = 0; i < ROOM_COUNT * 2; i++) {
    byte dst = random(0, ROOM_COUNT);
    byte dir = random(0, 4);
    byte dir_i = 0;

    switch (dir) {
      case UP:
        dir_i = DOWN;
        break;
      case DOWN:
        dir_i = UP;
        break;
      case LEFT:
        dir_i = RIGHT;
        break;
      case RIGHT:
        dir_i = LEFT;
        break;
    }

    if (dst == i % ROOM_COUNT) {
      continue;
    } else if (rooms[dst][dir_i] != -1 || rooms[i % ROOM_COUNT][dir] != -1) {
      continue; 
    } else {
      rooms[dst][dir_i] = i % ROOM_COUNT;
      rooms[i % ROOM_COUNT][dir] = dst;
    }
  }
}

void generateEnemies() {
  for (int i = 0; i < ENEMY_COUNT; i++) {
    enemies[i][ROOM] = -1;
    enemies[i][ACTIVE] = 0;
    enemies[i][X] = 0;
    enemies[i][Y] = 0;
    
    byte rnd = 0;
    do {
      rnd = random(1, ROOM_COUNT);  // TODO: Avoid player's starting room
    } while (rooms[rnd][UP] == -1 && rooms[rnd][DOWN] == -1 && rooms[rnd][LEFT] == -1 && rooms[rnd][RIGHT] == -1);
    
    enemies[i][ROOM] = rnd;
    enemies[i][ACTIVE] = 1;
    enemies[i][X] = random(9, 54);
    enemies[i][Y] = random(9, 54);
  }
}

void generateAmmo() {
  for (int i = 0; i < AMMO_COUNT; i++) {
    ammo[i][ROOM] = -1;
    ammo[i][ACTIVE] = 0;
    ammo[i][X] = 0;
    ammo[i][Y] = 0;
    
    byte rnd = 0;
    do {
      rnd = random(0, ROOM_COUNT);
    } while (rooms[rnd][UP] == -1 && rooms[rnd][DOWN] == -1 && rooms[rnd][LEFT] == -1 && rooms[rnd][RIGHT] == -1);
    
    ammo[i][ROOM] = rnd;
    ammo[i][ACTIVE] = 1;
    ammo[i][X] = random(9, 54);
    ammo[i][Y] = random(9, 54);
  }
}

void generateHealth() {
  for (int i = 0; i < HEALTH_COUNT; i++) {
    health[i][ROOM] = -1;
    health[i][ACTIVE] = 0;
    health[i][X] = 0;
    health[i][Y] = 0;
    
    byte rnd = 0;
    do {
      rnd = random(0, ROOM_COUNT);
    } while (rooms[rnd][UP] == -1 && rooms[rnd][DOWN] == -1 && rooms[rnd][LEFT] == -1 && rooms[rnd][RIGHT] == -1);
    
    health[i][ROOM] = rnd;
    health[i][ACTIVE] = 1;
    health[i][X] = random(9, 54);
    health[i][Y] = random(9, 54);
  }
}

void drawHud() {
  arduboy.setTextSize(1);
  
  arduboy.setCursor(68, 0);
  arduboy.print("HP ");
  arduboy.print(pl);

  arduboy.setCursor(68, 10);
  arduboy.print("AM ");
  arduboy.print(pa);

  arduboy.setCursor(68, 20);
  arduboy.print("R# ");
  arduboy.print(current_room + 1);
  
  arduboy.setCursor(68, 30);
  arduboy.print("EN ");
  arduboy.print(enemies_active);
  arduboy.print("/");
  arduboy.print(ENEMY_COUNT);

  arduboy.setCursor(68, 47);
  arduboy.print("A: Shuffle");

  arduboy.setCursor(68, 57);
  arduboy.print("B: Shoot");
}

void drawRoom() {  
  // Top left corner
  arduboy.fillRect(0, 0, 24, 2, WHITE);
  arduboy.fillRect(0, 0, 2, 24, WHITE);

  // Bottom left corner
  arduboy.fillRect(0, 40, 2, 24, WHITE);
  arduboy.fillRect(0, 62, 24, 2, WHITE);

  // Top right corner
  arduboy.fillRect(40, 0, 24, 2, WHITE);
  arduboy.fillRect(62, 0, 2, 24, WHITE);

  // Bottom right corner
  arduboy.fillRect(62, 40, 2, 24, WHITE);
  arduboy.fillRect(40, 62, 24, 2, WHITE);

  if (rooms[current_room][UP] != -1) {
    arduboy.drawRect(24, 0, 16, 4, WHITE);
  } else {
    arduboy.fillRect(24, 0, 16, 4, WHITE);
  }

  if (rooms[current_room][DOWN] != -1) {
    arduboy.drawRect(24, 60, 16, 4, WHITE);
  } else {
    arduboy.fillRect(24, 60, 16, 4, WHITE);
  }

  if (rooms[current_room][LEFT] != -1) {
    arduboy.drawRect(0, 24, 4, 16, WHITE);
  } else {
    arduboy.fillRect(0, 24, 4, 16, WHITE);
  }

  if (rooms[current_room][RIGHT] != -1) {
    arduboy.drawRect(60, 24, 4, 16, WHITE);
  } else {
    arduboy.fillRect(60, 24, 4, 16, WHITE);
  }
}

void drawPlayer() {
  switch (po) {
    case UP:
      arduboy.fillTriangle(px, py - SIZE, px - SIZE, py + SIZE, px + SIZE, py + SIZE, WHITE);
      break;
    case RIGHT:
      arduboy.fillTriangle(px - SIZE, py - SIZE, px + SIZE, py, px - SIZE, py + SIZE, WHITE);
      break;
    case DOWN:
      arduboy.fillTriangle(px - SIZE, py - SIZE, px + SIZE, py - SIZE, px, py + SIZE, WHITE);
      break;
    case LEFT:
      arduboy.fillTriangle(px + SIZE, py - SIZE, px + SIZE, py + SIZE, px - SIZE, py, WHITE);
      break;
  }
}

void drawEnemies() {
  for (int i = 0; i < ENEMY_COUNT; i++) {
    if (enemies[i][ROOM] != current_room || enemies[i][ACTIVE] == -1) {
      continue;
    }

    if (enemies[i][ACTIVE] == 1) {
      arduboy.fillCircle(enemies[i][X], enemies[i][Y], SIZE, WHITE);
    } else if (enemies[i][ACTIVE] == 0) {
      arduboy.drawCircle(enemies[i][X], enemies[i][Y], SIZE, WHITE);
    }
  }
}

void drawAmmo() {
  for (int i = 0; i < AMMO_COUNT; i++) {
    if (ammo[i][ROOM] != current_room || ammo[i][ACTIVE] <= 0) {
      continue;
    }
    
    arduboy.drawRect(ammo[i][X] - 3, ammo[i][Y] - 3, 6, 6, WHITE);
  }
}

void drawHealth() {
  for (int i = 0; i < HEALTH_COUNT; i++) {
    if (health[i][ROOM] != current_room || health[i][ACTIVE] <= 0) {
      continue;
    }
    
    arduboy.drawRect(health[i][X] - 3, health[i][Y] - 1, 6, 2, WHITE);
    arduboy.drawRect(health[i][X] - 1, health[i][Y] - 3, 2, 6, WHITE);
  }
}

void drawBullets() {
  for (int i = 0; i < BULLET_COUNT; i++) {
    if (bullets[i][BULLET_X] != -1 && bullets[i][BULLET_Y] != -1) {
      bullets[i][BULLET_X] += bullets[i][BULLET_X_SPEED];
      bullets[i][BULLET_Y] += bullets[i][BULLET_Y_SPEED];

      arduboy.drawCircle(bullets[i][BULLET_X], bullets[i][BULLET_Y], 1, WHITE);
    }

    if (
      bullets[i][BULLET_X] < 0 || bullets[i][BULLET_X] > 63 || 
      bullets[i][BULLET_Y] < 0 || bullets[i][BULLET_Y] > 63
    ) {
      bullets[i][BULLET_X] = -1;
      bullets[i][BULLET_Y] = -1;
    }
  }
}

void updateEnemies() {
  // TODO: Make enemies move towards you
  
  for (int i = 0; i < ENEMY_COUNT; i++) {
    if (enemies[i][ROOM] != current_room || enemies[i][ACTIVE] == -1) {
      continue;
    }
    
    enemies[i][X] += random(-1 * ENEMY_SPEED, ENEMY_SPEED + 1);
    enemies[i][Y] += random(-1 * ENEMY_SPEED, ENEMY_SPEED + 1);

    if (enemies[i][X] - SIZE <= 2) { enemies[i][X] = 3 + SIZE; } 
    if (enemies[i][X] + SIZE >= 61) { enemies[i][X] = 60 - SIZE; } 
    if (enemies[i][Y] - SIZE <= 2) { enemies[i][Y] = 3 + SIZE; } 
    if (enemies[i][Y] + SIZE >= 61) { enemies[i][Y] = 60 - SIZE; } 
  }
}

void handleInput() {
  if (ubuf > 0) { ubuf--; }
  if (rbuf > 0) { rbuf--; }
  if (dbuf > 0) { dbuf--; }
  if (lbuf > 0) { lbuf--; }
  if (bbuf > 0) { bbuf--; }

  if (arduboy.pressed(UP_BUTTON) && ubuf == 0) {
    ubuf = MOVE_DELAY;

    po = UP;
    py--;
  }

  if (arduboy.pressed(RIGHT_BUTTON) && rbuf == 0) {
    rbuf = MOVE_DELAY;
    
    po = RIGHT;
    px++;
  }

  if (arduboy.pressed(DOWN_BUTTON) && dbuf == 0) {
    dbuf = MOVE_DELAY;
    
    po = DOWN;
    py++;
  }

  if (arduboy.pressed(LEFT_BUTTON) && lbuf == 0) {
    lbuf = MOVE_DELAY;

    po = LEFT;
    px--;
  }

  if (arduboy.pressed(A_BUTTON) && abuf == 0) {
    abuf = 10;

    arduboy.invert(true);
    generateRooms();
    delay(20);
    arduboy.invert(false);
  }

  if (arduboy.pressed(B_BUTTON) && bbuf == 0) {
    bbuf = FIRE_DELAY;

    for (int i = 0; i < BULLET_COUNT; i++) {
      if (bullets[i][BULLET_X] == -1 && bullets[i][BULLET_Y] == -1) {
        if (pa <= 0) {
          break;
        }
        
        pa--;

        bullets[i][BULLET_X] = px;
        bullets[i][BULLET_Y] = py;

        switch (po) {
          case UP:
            bullets[i][BULLET_X_SPEED] = 0;
            bullets[i][BULLET_Y_SPEED] = -1 * BULLET_SPEED;
            break;
          case RIGHT:
            bullets[i][BULLET_X_SPEED] = BULLET_SPEED;
            bullets[i][BULLET_Y_SPEED] = 0;
            break;
          case DOWN:
            bullets[i][BULLET_X_SPEED] = 0;
            bullets[i][BULLET_Y_SPEED] = BULLET_SPEED;
            break;
          case LEFT:
            bullets[i][BULLET_X_SPEED] = -1 * BULLET_SPEED;
            bullets[i][BULLET_Y_SPEED] = 0;
            break;
        }
        
        break;
      }
    }
  }
}

void checkPlayerRoomCollision() {
  // Vertical
  if (py - SIZE < 2 && (px - SIZE < 24 || px + SIZE > 40)) { py++; } 
  if (py + SIZE > 61 && (px - SIZE < 24 || px + SIZE > 40)) { py--; } 

  // Horizontal
  if (px - SIZE < 2 && (py - SIZE < 24 || py + SIZE > 40)) { px++; } 
  if (px + SIZE > 61 && (py - SIZE < 24 || py + SIZE > 40)) { px--; } 

  // Top door
  if (py - SIZE <= 0 && px - SIZE >= 24 && px + SIZE <= 40 && rooms[current_room][UP] != -1) {  
    current_room = rooms[current_room][UP];
    py = 60;
  } else if (py - SIZE <= 0) {
    py += 2;
  }

  // Bottom door
  if (py + SIZE >= 64 && px - SIZE >= 24 && px + SIZE <= 40 && rooms[current_room][DOWN] != -1) {  
    current_room = rooms[current_room][DOWN];
    py = 4;
  } else if (py + SIZE >= 64) {
    py -= 2;
  }

  // Left door
  if (px - SIZE <= 0 && py - SIZE >= 24 && py + SIZE <= 40 && rooms[current_room][LEFT] != -1) {  
    current_room = rooms[current_room][LEFT];
    px = 60;
  } else if (px - SIZE <= 0) {
    px += 2;
  }

  // Right door
  if (px + SIZE >= 64 && py - SIZE >= 24 && py + SIZE <= 40 && rooms[current_room][RIGHT] != -1) {  
    current_room = rooms[current_room][RIGHT];
    px = 4;
  } else if (px + SIZE >= 64) {
    px -= 2;
  }
}

void checkBulletEnemyCollision() {
  for (int i = 0; i < ENEMY_COUNT; i++) {
    if (enemies[i][ROOM] != current_room || enemies[i][ACTIVE] == -1) {
      continue;
    }
      
    for (int j = 0; j < BULLET_COUNT; j++) {
      if (bullets[j][BULLET_X] == -1 && bullets[j][BULLET_Y] == -1) {
        continue;
      }

      if (
        (abs(enemies[i][X] - bullets[j][BULLET_X]) * 2 < (SIZE * 2 + 2)) && 
        (abs(enemies[i][Y] - bullets[j][BULLET_Y]) * 2 < (SIZE * 2 + 2))
      ) {
        enemies[i][ACTIVE]--;

        if (enemies[i][ACTIVE] == -1) {
          enemies_active--;
        }

        if (enemies_active == 0) {
          state = 2;
        }
        
        bullets[j][BULLET_X] = -1;
        bullets[j][BULLET_Y] = -1;
      }
    }
  }    
}

void checkPlayerEnemyCollision() {
  for (int i = 0; i < ENEMY_COUNT; i++) {
    if (enemies[i][ROOM] != current_room || enemies[i][ACTIVE] == -1) {
      continue;
    }
    
    if (
      (abs(enemies[i][X] - px) * 2 < (SIZE * 2 + SIZE * 2)) && 
      (abs(enemies[i][Y] - py) * 2 < (SIZE * 2 + SIZE * 2))
    ) {
      pl -= ENEMY_DAMAGE;

      arduboy.invert(true);
      delay(20);
      arduboy.invert(false);

      if (pl <= 0) {
        pl = 0;

        state = 3;
      }
    }
  }    
}

void checkPlayerAmmoCollision() {
  for (int i = 0; i < AMMO_COUNT; i++) {
    if (ammo[i][ROOM] != current_room || ammo[i][ACTIVE] != 1) {
      continue;
    }
    
    if (
      (abs(ammo[i][X] - px) * 2 < (SIZE * 2 + SIZE * 2)) && 
      (abs(ammo[i][Y] - py) * 2 < (SIZE * 2 + SIZE * 2))
    ) {
      pa += 30;
      ammo[i][ACTIVE] = 0;
    }
  }    
}

void checkPlayerHealthCollision() {
  for (int i = 0; i < HEALTH_COUNT; i++) {
    if (health[i][ROOM] != current_room || health[i][ACTIVE] != 1) {
      continue;
    }
    
    if (
      (abs(health[i][X] - px) * 2 < (6 + SIZE * 2)) && 
      (abs(health[i][Y] - py) * 2 < (6 + SIZE * 2))
    ) {
      pl += 50;
      health[i][ACTIVE] = 0;
    }
  }    
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  
  resetGame();

  arduboy.clear();
}

void loop() {
  if (!arduboy.nextFrame()) { return; }

  arduboy.clear();

  if (abuf > 0) { abuf--; } // Moved here b/c A used in menus

  switch (state) {
    case 0:
      arduboy.setTextSize(2);
      arduboy.setCursor(4, 16);
      arduboy.print("HYPERCUBE");

      arduboy.setTextSize(1);
      arduboy.setCursor(4, 55);
      arduboy.print("Press A to start");

      if (arduboy.pressed(A_BUTTON) && abuf == 0) {
        abuf = 10;
        state = 1;
      }
      break;

    case 1:
      handleInput();
    
      drawHud();
    
      drawRoom();
      drawPlayer();
      checkPlayerRoomCollision();
    
      updateEnemies();
      drawEnemies();
    
      drawAmmo();
      checkPlayerAmmoCollision();
    
      drawHealth();
      checkPlayerHealthCollision();
      
      drawBullets();
      checkBulletEnemyCollision();
      checkPlayerEnemyCollision();
      break;
      
    case 2:
      arduboy.setTextSize(2);
      arduboy.setCursor(4, 16);
      arduboy.print("ALL CLEAR!");

      arduboy.setTextSize(1);
      arduboy.setCursor(4, 35);
      arduboy.print("XXX");
      arduboy.print(" points");
      
      arduboy.setCursor(4, 55);
      arduboy.print("Press A to restart");

      if (arduboy.pressed(A_BUTTON) && abuf == 0) {
        abuf = 10;
        resetGame();
      }
      break;
      
    case 3:
      arduboy.setTextSize(2);
      arduboy.setCursor(4, 16);
      arduboy.print("GAME OVER");

      arduboy.setTextSize(1);
      arduboy.setCursor(4, 35);
      arduboy.print("XXX");
      arduboy.print(" points");
      
      arduboy.setCursor(4, 55);
      arduboy.print("Press A to restart");

      if (arduboy.pressed(A_BUTTON) && abuf == 0) {
        abuf = 10;
        resetGame();
      }
      break;
  }

  arduboy.display();
}
