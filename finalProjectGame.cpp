//
//  finalProjectGame.cpp
//  NYUCodebase
//
//  Created by Peter Smondyrev on 12/14/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include <stdio.h>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "vector"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


#define fixedTimeStep 0.0166666f
#define Max_TimeSteps 6
#define mapWidth 130
#define mapHeight 20
#define tileSize 0.2
#define gravity -9.81

#define SPRITE_COUNT_X 30
#define SPRITE_COUNT_Y 16

unsigned char levelData[mapHeight][mapWidth];


// x position = x* 0.2,  y position = 2.0 - 0.2*y

//Code not for this code

struct button{
    int locationX;
    int locationY;
    bool On;
    float timer;
};


// structs for our keys
struct keyys{
    int spriteValue;
    int positionX;
    int positionY;
    bool inStorage;
    int doorXIndex;
    int doorYIndex;
};



// 14, 15, 44, 45
void generateKeyLocation(vector<keyys>& keyList){
    if(keyList.size() != 3){
        return;
    }
    int ind1 = int(keyList[0].positionY);
    int ind2 = int(keyList[0].positionX);
    int var = 118 - keyList[0].spriteValue - keyList[1].spriteValue - keyList[2].spriteValue;
    
    levelData[ind1][ind2] = char(var);
    
}


void buttonSwitch(button& swt){
    if(swt.On){
        return;
    }
    levelData[swt.locationY][swt.locationX] = char(int(levelData[swt.locationY][swt.locationX])+60);
    swt.On = true;
    swt.timer = 0.0;
}



enum objecttype{hero, enemy, hazard, rock, jumpPad, keys, openButtons};

//-------------------------------------------------

class gameObjects{
    objecttype type;
    
    int spriteLocation;
    float positionX;
    float positionY;
    float velocityX;
    float velocityY;
    
    vector<keyys> keyValues;    //This is only for the second tileMap
    int index;
    
    
    vector<button> buttonLocation;
    
    float direction;
    bool jumper;
    
    float accelerationY;
    bool SocialStatus;  //This is only for the heroObject, villians don't have to worry about getting killed
    bool finishStatus;  // This bool is only for the finish Line;
    bool pursuit;
    
    
public:
    bool collide(float& varX, float& varY){
        //varX and varY are the indexes of the matrices, 0.2 is the uniform length of a side of a tile
        
        if(levelData[int(varY)][int(varX)] == '0'){
            return false;
        }
        
        float ed1= float(int(varX));
        float ed2 = float(int(varX)+1);
        
        float edy1 = float(int(varY));
        float edy2 = float(int(varY)+1);
        
        if(ed1 < varX && varX < ed2){
            if(varY > edy1 && varY < edy2){
                return true;
            }
        }
        return false;
    }
    
    void fall(){
        jumper=true;
    }
    
    bool spikeColTwo(float& varX, float& varY){
        float top = float(int(varY))+0.6;
        float bot = top + 0.4;
        
        if(varX > float(int(varX)) && varX < float(int(varX)+1)){
            if(varY > top && varY < bot){
                return true;
            }
        }
        return false;
    }

    void death(){
        SocialStatus = false;
        return;
    }
 
    // //button values are 74, 75, 104, 105    off switches
    //on switches are 134, 135, 164, 165
    bool isItASwitch(float& varY, float& varX){
        if(int(levelData[int(varY)][int(varX)]) == 74){
            return true;
        }
        
        if(int(levelData[int(varY)][int(varX)]) == 75){
            return true;
        }

        if(int(levelData[int(varY)][int(varX)]) == 104){
            return true;
        }

        if(int(levelData[int(varY)][int(varX)]) == 105){
            return true;
        }
        
        return false;
    }
    
    void upDateButtonTime(float& elapsed){
        for(int i = 0; i < buttonLocation.size(); i++){
            if(buttonLocation[i].On){
                buttonLocation[i].timer += elapsed;
            }
            if(buttonLocation[i].timer > 40.0 && buttonLocation[i].On){
                buttonLocation[i].On = false;
                buttonLocation[i].timer = 0.0;
                int oldVar = int(levelData[buttonLocation[i].locationY][buttonLocation[i].locationX]);
                levelData[buttonLocation[i].locationY][buttonLocation[i].locationX] = char(oldVar-60);
            }
        }
        
        
    }
    
    void collisions(){
        //positionX and positionY are both float variables for the table.  WE convert them into values for the vertices.
        float rightEdge = positionX + 1.0;
        float botEdge = positionY + 1.0;
        
        float botR1Side = positionY +0.2;
        float botR2Side = positionY + 0.8;
        
        float topSideRef1 = positionX+ 0.2;
        float topSideRef2 = positionX+ 0.8;
        
        
        // jumpPad
        
        
        if(int(levelData[int(botEdge)][int(topSideRef2)]) == 233){
            velocityY = 12.0;
            positionY -= 0.0001;
        }
        
        //button switches
        if(isItASwitch(botEdge, topSideRef2) || isItASwitch(botEdge, topSideRef1)){
            for(int i = 0; i < buttonLocation.size(); i++){
                if(buttonLocation[i].locationY == int(botEdge) && buttonLocation[i].locationX == int(topSideRef2)){
                    buttonSwitch(buttonLocation[i]);
                    i = int(buttonLocation.size());
                }
                if(buttonLocation[i].locationY == int(botEdge) && buttonLocation[i].locationX == int(topSideRef1)){
                    buttonSwitch(buttonLocation[i]);
                    i = int(buttonLocation.size());
                }
            }
        }
        
        //trapdoor code
        if(int(levelData[int(botEdge)][int(topSideRef2)]) == 38){
            levelData[int(botEdge)][int(topSideRef2)] = char(194);
        }

        
        
        //leftCollisions
        if(collide(positionX, botR1Side) || collide(positionX, botR2Side)){
            if(int(levelData[int(botEdge)][int(topSideRef1)]) != 70){
                positionX = float(int(positionX)+1);
                positionX+= 0.00001;
            }
            
        }
        
        //rightCollisions
        if(collide(rightEdge, botR1Side) || collide(rightEdge, botR2Side)){
            if(int(levelData[int(botEdge)][int(topSideRef1)]) != 70){
                positionX = float(int(positionX));
                positionX-= 0.00001;
            }
        }
        
        //topCollisions
        if(collide(topSideRef1, positionY) || collide(topSideRef2, positionY)){
            velocityY = 0.0;
            positionY = float(int(positionY)+1);
            positionY += 0.002;
            
        }
        
        //bottomCollisions
        if(collide(topSideRef1, botEdge) || collide(topSideRef2, botEdge)){
            if(int(levelData[int(botEdge)][int(topSideRef1)]) == 70){
                if(spikeColTwo(topSideRef1, botEdge) && spikeColTwo(topSideRef2, botEdge)){
                    death();
                }
            }
            if(int(levelData[int(botEdge)][int(topSideRef1)]) != 70){
                jumper = false;
                positionY = float(int(positionY));
                
            }
        }
        
        if(int(levelData[int(botEdge)][int(positionX)]) == 194 && int(levelData[int(botEdge)][int(positionX)+1]) == 194){
            fall();
        }

        
    }

    
    //-------------------------------------------------
    
    
    
    //The villians pursuitFunctions
    bool inSight(float& playerVarX, float& playerVarY){
        float leftEdge = positionX - 3.0;
        float rightEdge = positionX + 4.0;
        float topEdge = positionY + 3.0;
        float bottomEdge = positionY - 2.0;
        
        if(playerVarX < rightEdge && playerVarX > leftEdge){
            if(playerVarY < topEdge && playerVarY > bottomEdge){
                return true;
            }
            if(playerVarY < topEdge && playerVarY > bottomEdge){
                return true;
            }
        }
        else if(playerVarX+0.2 > leftEdge && playerVarX + 0.2 < rightEdge){
            if(playerVarY < topEdge && playerVarY > bottomEdge){
                return true;
            }
            if(playerVarY < topEdge && playerVarY > bottomEdge){
                return true;
            }
        }
        return false;
    }
    
    void patrol(float& elapsed){
        positionX += velocityX*elapsed*direction;
    }
    
    void pursue(float playerVarX, float playerVarY){    //
        if(inSight(playerVarX, playerVarY)){
            
        }
    }
    //----------------------
    
    void jump(){
        if(jumper == false){
            velocityY = 3.0;
            jumper = true;
        }
    }
    
    // 14, 15, 44, 45
    void keyPickUp(){
        int rightEdge = int(positionX)+1;
        int leftEdge = rightEdge - 1;
        int topEdge = int(positionY);
        int botEdge = topEdge + 1;
        
        int righ = int(positionX) + 2;
        
        int var = int(levelData[int(positionY)][righ]);
        int varL = int(levelData[int(positionY)][leftEdge]);
        
        for(int i = 0; i < keyValues.size(); i++){
            if(int(levelData[keyValues[i].doorYIndex][keyValues[i].doorXIndex]) == var){
                keyValues[i].spriteValue = var;
                levelData[int(positionY)][righ] = char(194);
                i = int(keyValues.size());
            }
            if(int(levelData[keyValues[i].doorYIndex][keyValues[i].doorXIndex]) == varL){
                keyValues[i].spriteValue = varL;
                levelData[int(positionY)][leftEdge] = char(194);
                i = int(keyValues.size());
            }
        }
        
    }

    void openDoor(keyys& keyIte){
        levelData[keyIte.doorYIndex][keyIte.doorXIndex] = char(193);
        keyIte.inStorage = false;
    }
    
    
    void jumpFall(float& elapsed){
        velocityY += elapsed*gravity;
        positionY += velocityY*elapsed - (0.5*gravity*(elapsed*elapsed));
    }
    
    void objMove(float& elapsed){
        positionX += velocityX*direction*elapsed;
    }
    
    void finishLine(){
        if(int(levelData[int(positionY)][int(positionX)]) == 130){
            finishStatus = true;
        }
        if(int(levelData[int(positionY)][int(positionX)+1]) == 130){
            finishStatus = true;
        }
        if(int(levelData[int(positionY)+1][int(positionX)]) == 130){
            finishStatus = true;
        }
        if(int(levelData[int(positionY)+1][int(positionX)+1]) == 130){
            finishStatus = true;
        }

    }
    
    //194, 195, 224, 225
    void doorOpen(){
        float leftEd = positionX;
        float rightEd = leftEd + 1.3;
        float botEd = positionY + 0.2;
        float topEd = botEd + 1.2;
        
        if(collide(botEd, rightEd) && levelData[int(botEd)][int(rightEd)]){
            
        }
        
        if(collide(botEd, leftEd) && levelData[int(botEd)][int(rightEd)]){
            
        }
        
        if(collide(topEd, rightEd) && levelData[int(botEd)][int(rightEd)]){
            
        }
        if(collide(topEd, leftEd) && levelData[int(botEd)][int(rightEd)]){
            
        }

        
        
    }

    
    // door values are 194, 195, 224, 225
    void openDoorLock(){   //This function is only for teh second tile map in our game
        
        float righEdge = positionX + 0.8;
        float bot = positionY + 0.2;
        float top = bot + 0.6;
        
        if(int(levelData[int(bot)][int(righEdge)]) != 194 || int(levelData[int(top)][int(righEdge)]) != 194){
            return;
        }
        if(int(levelData[int(bot)][int(righEdge)]) != 195 || int(levelData[int(top)][int(righEdge)]) != 195){
            return;
        }
        if(int(levelData[int(bot)][int(righEdge)]) != 224 || int(levelData[int(top)][int(righEdge)]) != 224){
            return;
        }
        if(int(levelData[int(bot)][int(righEdge)]) != 225 || int(levelData[int(top)][int(righEdge)]) != 225){
            return;
        }
        
        if(keyValues.size() == 4){
            if(collide(bot, righEdge) || collide(top, righEdge)){
                for(int i =0; i < keyValues.size(); i++){
                    openDoor(keyValues[i]);
                }
                
            }
        }
    }
    
    
    //-----------------------
    //Movement for PlayerObject
    void move(float& elapsed){  //function to update positions
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        direction = 0.0;
        if(keys[SDL_SCANCODE_W]){
            jump();
        }
        
        if(keys[SDL_SCANCODE_D]){
            direction = 1.0;

        }
        
        if(keys[SDL_SCANCODE_A]){
            direction = -1.0;
        }

        if(keys[SDL_SCANCODE_RETURN]){
            finishLine();
        }
        if(keys[SDL_SCANCODE_K]){
            
        }
        
        if(keys[SDL_SCANCODE_P]){
           keyPickUp();
        }

        //-------------------------------
        
        upDateButtonTime(elapsed);  // updates Button time so we keep track of time.
        if(jumper){jumpFall(elapsed);}
        objMove(elapsed);
        
        
        //-------------------
        //collision detection
        collisions();
        
        
    }
    
    
    void draw(){    // Function that is responsible for drawing the character
        
    }
    
    
};





int main(int argc, char *argv[]){
    return 0;
}

