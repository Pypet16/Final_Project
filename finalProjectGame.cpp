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


struct keys{
    int spriteValue;
    float positionX;
    float positionY;
};


void generateKeyLocation(vector<keys>& keyList){
    if(keyList.size() != 3){
        return;
    }
    int ind1 = int(keyList[0].positionY);
    int ind2 = int(keyList[0].positionX);
    levelData[ind1][ind2] = char(15);
    
}


bool spikeCollisions(float& varX, float& varY){
    float left = float(int(varX));
    float bot = float(int(varY)+1);
    float top = bot - 0.3;
    float right = left + 1.0;
    
    if(left < varX && varX < right){
        if(varY > top && varY < bot){
            return true;
        }
    }
    return false;

}

void buttonSwitch(button& swt){
    levelData[swt.locationY][swt.locationX] = char(int(levelData[swt.locationY][swt.locationX])+60);
    swt.On = true;
    swt.timer = 0.0;
}



enum objecttype{hero, enemy, hazard, rock, jumpPad, keys, openButtons};

class objects{
    int spriteLocation;
    float positionX;
    float positionY;
    float velocityY;
    
    float objectDimensionsOrigin;
    
    bool fall;
    bool objectPresent;
    
    float timeSwitchIsOn;
    bool switchOn;
    
    objecttype numb;
    
public:
    //----Object Action Function
    objecttype returnObjType(){return numb;}
    
    void keyPickedUp(){
        levelData[int(positionY)][int(positionX)] = char(0);
    }
    
    
    //ButtonSwitches
    void buttonSwitch(){
        if(switchOn){return;}
        timeSwitchIsOn = 0.0;
        switchOn = true;
    }
    void buttonSwitchTime(float& elap){
        timeSwitchIsOn += elap;
        if(elap > 15.0){
            switchOn = false;
        }
        
    }
    
    //Stone functions
    void trapDoorActivate(){
        levelData[int(positionY)][int(positionX)] = char(0);
    }
    
    
    float jumpPad(){
        return 3.0;
    }
    //-------------------------------------------------
    //collision with objects
    
    bool collideObject(float& varX, float& varY){

        float varTopY = varY;
        float varPointX = varX;
        
        
        if(varPointX > positionX && varPointX < positionX+tileSize){
            if(varTopY > positionY && varTopY < positionY + tileSize){
                return true;
            }
            if(varY > positionY && varY < positionY + tileSize){
                return true;
            }
        }
        if(varX > positionX && varX < positionX+tileSize){
            if(varTopY > positionY && varTopY < positionY + tileSize){
                return true;
            }
            if(varY > positionY && varY < positionY + tileSize){
                return true;
            }
        }
        
        return false;
    }
    
    
};

//-------------------------------------------------

class gameObjects{
    objecttype type;
    int spriteLocation;
    float positionX;
    float positionY;
    float velocityX;
    float velocityY;
    
    vector<int> keyValues;
    vector<button> buttonLocation;
    
    float direction;
    bool jumper;
    
    float accelerationY;
    bool SocialStatus;  //This is only for the heroObject, villians don't have to worry about getting killed
    bool pursuit;
    
    
public:
    bool collideOb(objects& obj){
        bool result;
        result = obj.collideObject(positionX, positionY);
        objecttype samp = obj.returnObjType();
        
        if(samp == hazard){
            death();
        }
        
        if(samp == rock){
            obj.trapDoorActivate();
        }
        
        if(samp == jumpPad){
            velocityY = obj.jumpPad();
        }
        if(samp == keys){
            obj.keyPickedUp();
        }
        
        if(samp == openButtons){
            //makes button switch turn on
            obj.buttonSwitch();
        }
        return result;
    }
    
    void collideObj(float& varY, float& varX){
        if(int(levelData[int(varY)][int(varX)]) == 70 ){
            
        }
        
        if(int(levelData[int(varY)][int(varX)]) == 396 ){
            
        }

        if(int(levelData[int(varY)][int(varX)]) == 283 ){
            
        }

        //key values are 44, 45, 14, 15
        if(int(levelData[int(varY)][int(varX)]) == 70 ){
            
        }

        
        
    }
    
    
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

    void collisions(){
        //positionX and positionY are both float variables for the table.  WE convert them into values for the vertices.
        float rightEdge = positionX + 1.0;
        float botEdge = positionY + 1.0;
        
        float botR1Side = positionY +0.2;
        float botR2Side = positionY + 0.8;
        
        float topSideRef1 = positionX+ 0.2;
        float topSideRef2 = positionX+ 0.8;
        
        if(int(levelData[int(botEdge)][int(topSideRef2)]) == 233){
            velocityY = 12.0;
            positionY -= 0.0001;
        }
        
        if(int(levelData[int(botEdge)][int(topSideRef2)]) == 74 || int(levelData[int(botEdge)][int(topSideRef1)]) == 74){
            buttonSwitch(buttonLocation[0]);
        }
        
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
    
    void keyPickUp(){
        int rightEdge = int(positionX)+1;
        int leftEdge = rightEdge - 1;
        int topEdge = int(positionY);
        int botEdge = topEdge + 1;
        
        int righ = int(positionX) + 2;

        if(int(levelData[int(positionY)][righ]) == 14){
            levelData[int(positionY)][righ] = char(194);
            keyValues.push_back(14);
        }
        if(int(levelData[int(positionY)][righ]) == 15){
            levelData[int(positionY)][righ] = char(194);
            keyValues.push_back(15);
        }
        if(int(levelData[int(positionY)][righ]) == 44){
            levelData[int(positionY)][righ] = char(194);
            keyValues.push_back(44);
        }
        if(int(levelData[int(positionY)][righ]) == 45){
            levelData[int(positionY)][righ] = char(194);
            keyValues.push_back(45);
        }
    }

    
    
    void jumpFall(float& elapsed){
        velocityY += elapsed*gravity;
        positionY += velocityY*elapsed - (0.5*gravity*(elapsed*elapsed));
    }
    
    void objMove(float& elapsed){
        positionX += velocityX*direction*elapsed;
    }
    
    
    
    //-----------------------
    //Movement for GameObject
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
            
        }
        if(keys[SDL_SCANCODE_P]){
           keyPickUp();
        }

        //-------------------------------
        
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

