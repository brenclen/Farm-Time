#include <stdio.h>
#include "raylib.h"
#include <cstdlib>
#include <iostream>
#include <deque>
#include <raymath.h>
#include "config.h"
#include "grass.h"
#include "tree.h"

using namespace std;    

bool TimePassed(double interval) {
    double currentTime = GetTime();
    if(currentTime-lastUpdateTime>=interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Character {
    public:
    int x = 0;
    int y = 0;
    int xTile = 0;
    int yTile = 1;
    int direction = 0;

    Image frontSpritesheet;
    Image front;
    Texture frontTexture;
    Image backSpritesheet;
    Image back;
    Texture backTexture;
    Image leftSpritesheet;
    Image left;
    Texture leftTexture;
    Image rightSpritesheet;
    Image right;
    Texture rightTexture;
    Texture currentTexture;
    Character() {
        frontSpritesheet = LoadImage("../resources/characters/bren/walk/front/spritesheet.png");
        backSpritesheet = LoadImage("../resources/characters/bren/walk/back/spritesheet.png");
        leftSpritesheet = LoadImage("../resources/characters/bren/walk/left/spritesheet.png");
        rightSpritesheet = LoadImage("../resources/characters/bren/walk/right/spritesheet.png");
        front = ImageFromImage(frontSpritesheet, Rectangle{8,0,16,32});
        back = ImageFromImage(backSpritesheet, Rectangle{8,0,16,32});
        left = ImageFromImage(leftSpritesheet, Rectangle{8,0,16,32});
        right = ImageFromImage(rightSpritesheet, Rectangle{8,0,16,32});
        UnloadImage(frontSpritesheet);
        UnloadImage(backSpritesheet);
        UnloadImage(leftSpritesheet);
        UnloadImage(rightSpritesheet);
        frontTexture = LoadTextureFromImage(front);
        backTexture = LoadTextureFromImage(back);
        leftTexture = LoadTextureFromImage(left);
        rightTexture = LoadTextureFromImage(right);
        UnloadImage(front);
        UnloadImage(back);
        UnloadImage(left);
        UnloadImage(right);
        currentTexture = frontTexture;
    }
    ~Character() {
        UnloadTexture(frontTexture);
        UnloadTexture(backTexture);
    }
    void draw() {
        DrawTexture(currentTexture, x, y, WHITE);
    }
    void movement(int walkable[][48]) {
        if(IsKeyDown(KEY_DOWN) && TimePassed(.2)) {
            if(walkable[yTile+1][xTile]==0) {
                y+=16;
                yTile+=1;
            }
            currentTexture = frontTexture;
            direction = 0;
        }
        else if(IsKeyDown(KEY_UP) && TimePassed(.2)) {
            if(walkable[yTile-1][xTile]==0) {
                y-=16;
                yTile -=1;
            }
            currentTexture = backTexture;
            direction = 2;
        }
        else if(IsKeyDown(KEY_LEFT) && TimePassed(.2)) {
            if(walkable[yTile][xTile-1]==0) {
                x-=16;
                xTile -= 1;
            }
            currentTexture = leftTexture;
            direction = 1;
        }
        else if(IsKeyDown(KEY_RIGHT) && TimePassed(.2)) {
            if(walkable[yTile][xTile+1]==0) {                
                x+=16;
                xTile += 1;
            }
            currentTexture = rightTexture;
            direction = 3;
        }        
    }
    void update(int walkable[][48]) {
        movement(walkable);
    }
};
class Inventory {
    public:
    int inventory[9] = {};
    int woodCount = 0;
    void draw() {
        DrawRectangle(cellSize*cellHorizontal/2-(cellSize*cellHorizontal/8), cellSize*cellVertical-32, cellSize*cellHorizontal/4, 32, DARKBROWN);
        if(inventory[0] == 1) {
            DrawText(TextFormat("%d x Wood", woodCount), cellSize*cellHorizontal/2-(cellSize*cellHorizontal/9),cellSize*cellVertical-26, 20, RAYWHITE);
        }
    }
};
class Game {
    public:
    Grass grass = Grass();
    Character main = Character();
    Tree tree = Tree();
    Inventory inventory = Inventory();
    int walkable[48][48] = {};
    
    void createTreeHitboxMap() {
        int treeNum = 1;
        for(int row = 0; row<16;row++) {
            for(int col = 0; col<16;col++) {
                if(tree.treeMap[row][col]==1 || tree.treeMap[row][col]==2) {
                    treeNum = 1;
                    for(int i =1;i<=3;i++) {
                        for(int j =0;j<=2;j++) {
                            walkable[(3*row)+i][(3*col)+j] = treeNum;
                            treeNum++;
                        }
                    }
                }
            }
        }
    }

    void printTreeHitboxMap() {
        printf("00{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7}\n");
        for(int row = 0; row<=47;row++) {
            if(row<10) {
                printf("0");
            }
            printf("%d", row);
            printf("{");
            for(int col = 0; col<=47;col++) {
                printf("%d", walkable[row][col]);
                if(col!=47) {
                    printf(", ");
                }
            }
            printf("},\n");
        }
        printf("};\n");
    }

    void update() {
        main.update(walkable);
        if(treeChoppable() && IsKeyPressed(KEY_SPACE)) {
            treeChop();
        }
    }
    void draw() {
        main.draw();
        tree.draw();
        inventory.draw();
        if(treeChoppable()) {
            //cout << "choppable" << endl;
            DrawText("would you like to chop tree?", (cellSize*main.xTile)+21,(cellSize*main.yTile)-32,7,BLACK);
        }
    }
    bool treeChoppable() {
        if(main.direction == 0) {
            for(int i = 1;i<=3;i++) {
                if(walkable[main.yTile+1][main.xTile]==i) {
                    return true;
                }
            }
        }
        else if(main.direction == 1) {
            for(int i = 3;i<=9;i+=3) {
                if(walkable[main.yTile][main.xTile-1]==i) {
                    return true;
                }
            }
        }
        else if(main.direction == 2) {
            for(int i = 7;i<=9;i++) {
                if(walkable[main.yTile-1][main.xTile]==i) {
                    return true;
                }
            }
        }
        else if(main.direction == 3) {
            for(int i = 1;i<=7;i+=3) {
                if(walkable[main.yTile][main.xTile+1]==i) {
                    return true;
                }
            }
        }
        return false;
    }
    void deleteTree(int posY, int posX) {
        walkable[posY][posX] = 0;
        walkable[posY][posX+1] = 0;
        walkable[posY][posX+2] = 0;
        walkable[posY+1][posX] = 0;
        walkable[posY+1][posX+1] = 0;
        walkable[posY+1][posX+2] = 0;
        walkable[posY+2][posX] = 0;
        walkable[posY+2][posX+1] = 0;
        walkable[posY+2][posX+2] = 0;
    }
    void treeChop() {
        //facing down
        if(main.direction == 0) {
            if(walkable[main.yTile+1][main.xTile] == 1) {
                deleteTree(main.yTile+1,main.xTile);
            }
            if(walkable[main.yTile+1][main.xTile] == 2) {
                deleteTree(main.yTile+1,main.xTile-1);
            }
            if(walkable[main.yTile+1][main.xTile] == 3) {
                deleteTree(main.yTile+1,main.xTile-2);
            }
            tree.treeMap[main.yTile/3][main.xTile/3] = 0;
            inventory.inventory[0] = 1;
            inventory.woodCount +=1;
        }
        else if(main.direction == 1) {
            if(walkable[main.yTile][main.xTile-1] == 3) {
                deleteTree(main.yTile,main.xTile-3);
            }
            if(walkable[main.yTile][main.xTile-1] == 6) {
                deleteTree(main.yTile-1,main.xTile-3);
            }
            if(walkable[main.yTile][main.xTile-1] == 9) {
                deleteTree(main.yTile-2,main.xTile-3);
            }
            tree.treeMap[(main.yTile-1)/3][(main.xTile-1)/3] = 0;
            inventory.inventory[0] = 1;
            inventory.woodCount +=1;
        }
        else if(main.direction == 2) {
            if(walkable[main.yTile-1][main.xTile] == 7) {
                deleteTree(main.yTile-3,main.xTile);
            }
            if(walkable[main.yTile-1][main.xTile] == 8) {
                deleteTree(main.yTile-3,main.xTile-1);
            }
            if(walkable[main.yTile-1][main.xTile] == 9) {
                deleteTree(main.yTile-3,main.xTile-2);
            }
            tree.treeMap[(main.yTile-2)/3][main.xTile/3] = 0;
            inventory.inventory[0] = 1;
            inventory.woodCount +=1;
        }
        else if(main.direction == 3) {
            if(walkable[main.yTile][main.xTile+1] == 1) {
                deleteTree(main.yTile,main.xTile+1);
            }
            if(walkable[main.yTile][main.xTile+1] == 4) {
                deleteTree(main.yTile-1,main.xTile+1);
            }
            if(walkable[main.yTile][main.xTile+1] == 7) {
                deleteTree(main.yTile-2,main.xTile+1);
            }
            tree.treeMap[(main.yTile-1)/3][(main.xTile+2)/3] = 0;
            inventory.inventory[0] = 1;
            inventory.woodCount +=1;
        }
    }
};

int main() {
    InitWindow(cellSize*(cellHorizontal), cellSize*cellVertical, "farm time");
    SetWindowPosition(50, 50);   
    SetTargetFPS(60);
    Game game = Game();
    game.createTreeHitboxMap();
    game.printTreeHitboxMap();

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){170,246,131,0});
        game.grass.draw();
        game.update();
        game.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}