/* 
 * ardugames.ino - File for games
 * This file controls all when arduitochi is in a game, all vars, status, and games.
 */

//Game vars. If it are g_var there are . If it are g_xxx_yyy , xxx=Game 
byte g_score = 0;
byte g_randomnum = 0; //Var for storing a random thing

byte g_jump_jump = 0;
byte g_jump_jumping = 0;
int g_jump_wallpos = -12;

byte g_avoid_objy[] = {100,100,100,100};
byte g_avoid_objplace[] = {1,1,1,1};
byte g_avoid_tamapos = 37;
byte g_avoid_tempmin = 7;
byte g_avoid_ultpos = 0;
byte g_avoid_numran = 25;

byte g_fall_platformsX[] = {50,100,100,100}; //Platforms of game (X coordinate)
byte g_fall_platformsY[] = {42,100,100,100}; //Platforms of game (Y coordinate)
byte g_fall_ardupos[] = {50,30}; //X,Y tama

byte g_math_answer; //Correct answer
byte g_math_lastanswer; //Last answer
byte g_math_nanswer; //Button of correct answer (0,1,2)
byte g_math_options[3]; //Array, 3 options, 3 possible answers
byte g_math_waiting; //If its waiting for an answer (Used too if its 2 for end game)

void gameprocess(){ //This makes select one or another game
  if (ingame == 1){game_jump();}
  if (ingame == 2){game_avoid();}
  if (ingame == 3){game_fall();}
  if (ingame == 4){game_math();}
  //Show score
  display.setCursor(4,8);
  display.print(g_score); //Puntuacion
}


void game_jump(){
  if (g_jump_wallpos <= -12){g_jump_wallpos = 80;} //Put the wall right for go to ardu again
  g_jump_wallpos = g_jump_wallpos - 3; //Make go to left
  display.drawBitmap(g_jump_wallpos, 36, game_jump_wall_bmp, 12, 12, 1); //Wall draw
  switch (g_jump_jumping){
    case 1:
      g_jump_jump = g_jump_jump + 3;
      if (g_jump_jump > 16){g_jump_jumping = 2;}
      break;
    case 2:
      g_jump_jump = g_jump_jump - 2;
      if (g_jump_jump < 1){g_jump_jumping = 0;}
      break;
  }
  ardudraw(36,38-g_jump_jump,0);

  if (g_jump_wallpos == 38 && g_jump_jump <= 12){ //Check if ardu hit the wall
      losegame(0); //Game Over   
    }else{
      g_score++;soundid = 2;
  }
  switch (g_score){
    case 0:
    case 1:
    case 2:
    case 3:
    delay(80);
    break;
    case 4:
    case 5:
    case 6:
    case 7:
    delay(30);
    break;
    case 8:
    case 9:
    case 10:
    delay(10);
    break;
  }// Go without delay if its more than 10 points
  if (g_score > 30){losegame(1);}
}


void game_avoid(){
  for (varproc = 0; varproc <= 4;varproc++) //El uso de varproc es porque es una variable que sirve para poner valores para un proceso,asi ocupamos menos
  {
   if (g_avoid_objy[varproc] == 100 && random(0, g_avoid_numran) == 0 && g_avoid_tempmin > 7){ //Si el objeto se tiene que mover
    byte var = 0;
    while(var == 0){
      g_randomnum = random(0, 3);
      if (g_randomnum != g_avoid_ultpos){g_avoid_ultpos = g_randomnum;var = 1;}
    }
    if (g_randomnum == 0){g_avoid_objplace[varproc] = 14;}
    if (g_randomnum == 1){g_avoid_objplace[varproc] = 37;}
    if (g_randomnum == 2){g_avoid_objplace[varproc] = 61;}
    g_avoid_tempmin  = 0;
    g_avoid_objy[varproc] = 0;
    varproc=4; //Si se puso una en movimiento, no va a haber mas en el mismo for
    }
  }

  for (varproc = 0; varproc <= 4;varproc++)
  {
      if (g_avoid_objy[varproc] < 80){ //Ir bajando y dibujando el objeto
        g_avoid_objy[varproc] = g_avoid_objy[varproc]+3;
        display.drawBitmap(g_avoid_objplace[varproc], g_avoid_objy[varproc]-9, game_avoid_object_bmp, 11, 10, 1); //Object draw . y-9 used for make it appearing
      }
      if (g_avoid_objy[varproc] == 48+9){g_avoid_objy[varproc] = 100;g_score++;soundid=2;if(g_avoid_numran > 1){g_avoid_numran--;};} //Pos to 100 and +1 score
  
      if (g_avoid_objy[varproc] >= 26+9 && g_avoid_objy[varproc] < 45+9){
        if (g_avoid_objplace[varproc] == g_avoid_tamapos){losegame(0);} //Game Over
      }
  
  }

 ardudraw(g_avoid_tamapos,36,0);
  g_avoid_tempmin++;
 if (g_score <= 15){delay(50);}
 if (g_score > 40){losegame(1);}
}



void game_fall(){
  //I use varproc in for because its a reutilizable var for counting process things
  //First check collisions, before moving the platforms
  for (varproc = 0; varproc <= 3;varproc++){
    if (IsBetween(g_fall_ardupos[0],g_fall_platformsX[varproc] - 18, g_fall_platformsX[varproc] + 8) && (g_fall_ardupos[1] + 10 == g_fall_platformsY[varproc] || g_fall_ardupos[1] + 10 == g_fall_platformsY[varproc] + 1)){ //Test if arduitochi collides with a platform
      g_fall_ardupos[1] = g_fall_ardupos[1] - 1; //Up with the platform
      break;
    }
    if (varproc == 3){ //If its 3, it is because for finished, it isnt above any platform
      g_fall_ardupos[1] = g_fall_ardupos[1] + 1;
    }
  }
  
  if (g_randomnum <= 8) {g_randomnum++;} //Used here for not creating more than 1 platform at a time
  //See for where are the platforms (inside or outside) and move it
  for (varproc = 0;varproc <= 3;varproc++){
   if (g_fall_platformsY[varproc] >= 50 && g_randomnum >= 8 && random(1,4) == 1){ //Platform outside game
      g_fall_platformsX[varproc] = random(10, 54); //For position X
      g_fall_platformsY[varproc] = 47; //For position Y
      g_randomnum = 0;
    }else{ //Platform inside game
      g_fall_platformsY[varproc] = g_fall_platformsY[varproc] - 1; //Up
      if (g_fall_platformsY[varproc] <= 1){ //If reaches the top
        g_score++;
        g_fall_platformsY[varproc] = 100; //Restart platform
      }
    }
    display.drawLine(g_fall_platformsX[varproc] - 10, g_fall_platformsY[varproc], g_fall_platformsX[varproc] + 10, g_fall_platformsY[varproc], BLACK); //Draw it
  }
  
  if (g_fall_ardupos[1] < 2 || g_fall_ardupos[1] > 48){losegame(0);} //Out of screen (Y coordinate)
  
  ardudraw(g_fall_ardupos[0],g_fall_ardupos[1],0); //Draw ardu

}

void game_math(){
  if (g_math_waiting == 2){ /*Lose*/
    losegame(0);
  }
  if (g_math_answer == 0){g_math_answer = random(1,6);}
    if (g_math_waiting == 0){
      g_math_lastanswer = g_math_answer; //Save last answer
      g_math_answer = game_math_calcnumbers();

      g_math_nanswer = random(0,3); //For know where will put the correct answer
      
      for (varproc = 0;varproc <=2;varproc++){
        if (g_math_nanswer == varproc){
          g_math_options[varproc] = g_math_answer;
        }else{
          do{
            g_math_options[varproc] = game_math_calcnumbers();
          }while(g_math_options[varproc] == g_math_answer);
        }
      }
     g_math_waiting = 1;
    }
    display.setCursor(18,16);
    display.print(g_math_lastanswer);//Prints last answer, or first number
    if (g_math_lastanswer > g_math_answer){
      display.print("-");
      display.print(g_math_lastanswer - g_math_answer); 
    }else{
      display.print("+");
      display.print(g_math_answer - g_math_lastanswer); 
    }
    display.print("=?");
    display.setCursor(14,30);
    for (varproc = 0;varproc <=2;varproc++){
    display.print(g_math_options[varproc]);
    display.print("|");
    }
}

byte game_math_calcnumbers(){
  if (g_score > 30){
    g_randomnum = random(15,31);
  }else if (g_score > 20){
    g_randomnum = random(8,21);
  }else if (g_score > 10){
    g_randomnum = random(5,11);
  }else{
    g_randomnum = random(2,6);
  }
  if (g_math_lastanswer > g_randomnum){
  switch (random(1,3)){
    case 1://+
    return (g_math_lastanswer + g_randomnum);
    case 2:
    return (g_math_lastanswer - g_randomnum);
  }
  }else{
    return (g_math_lastanswer + g_randomnum);
  }
}


bool IsBetween (byte numberToCheck, byte bottom, byte top)
{
  return (numberToCheck >= bottom && numberToCheck <= top);
}


void gamebtn1(){
  if (ingame == 1){
  if (g_jump_jumping == 0){ 
    g_jump_jumping = 1;
    soundid = 1;
  }
  }
  if (ingame == 2){
    g_avoid_tamapos = 14;
  }
  if (ingame == 3){
    g_fall_ardupos[0] -=3;
  }

  if (ingame == 4){
    if (g_math_nanswer == 0){
      g_score++;
      g_math_waiting=0;
    }else{g_math_waiting = 2;}
  }
}

void gamebtn2(){
   if (ingame == 2){
    g_avoid_tamapos = 37;
  } 
  if (ingame == 3){
    g_fall_ardupos[0] +=3;
  }
  if (ingame == 4){
    if (g_math_nanswer == 1){
      g_score++;
      g_math_waiting=0;
    }else{g_math_waiting = 2;}
  }
}

void gamebtn3(){
  if (ingame == 1){
    closegame();
  }
  if (ingame == 2){
    g_avoid_tamapos = 61;
  }
  if (ingame == 4){
    if (g_math_nanswer == 2){
      g_score++;
      g_math_waiting=0;
    }else{g_math_waiting = 2;}
  }
}

void closegame(){
  if (ingame == 1){ //Reset vars
    g_jump_jump = 0;
    g_jump_jumping = 0;
    g_jump_wallpos = 100;
  }
  if (ingame == 2){ //Reset vars
    for (varproc = 0; varproc <= 4;varproc++){
      g_avoid_objy[varproc] = 100;
      g_avoid_objplace[varproc] = 1;
    }
    g_avoid_tamapos = 37;
  }

  if (ingame == 3){ //Reset vars
    g_fall_platformsX[0] = 50;
    g_fall_platformsY[0] = 42;
    g_fall_ardupos[0] = 50;
    g_fall_ardupos[1] = 30;
    for (varproc = 1;varproc <= 3;varproc++){
    g_fall_platformsX[varproc] = 100;
    g_fall_platformsY[varproc] = 100;
    }
  }
  if (ingame == 4){ //Reset vars
     g_math_answer = 0;
    }
  
  g_score = 0;
  g_randomnum = 0;
  varproc = 0;
  ingame = 0;
  menu[0] = 4;
  menu[1] = 1;
  menu[2] = 1;
  if (happiness < 5){happiness++;}
}

void losegame(byte win){ //1=win , 0=lose
  display.clearDisplay();

    if (win == 1){
      display.setCursor(4,8);
      display.print("You win!");
      display.display();
      delay(900);
    }else{
      display.setCursor(4,8);
      display.print("Game Over");
      display.display();
      soundid=3;
      delay(700);
    }
  display.setCursor(4,18);
  display.print("Score: ");
  display.print(g_score);
  display.display();
  delay(1000);
  display.clearDisplay();
  //Convert points to money (g_score is in this moment used for money). This will depend of the game playing
  g_score = g_score * 2;
  //
  for (byte v = 0;v < 2;v++){
  display.drawBitmap(10, 20, moneybag_bmp, 12, 11, 1); //Money
  display.setCursor(25,22);
  display.print("+");
  display.print(g_score);
  display.display();
  delay(750);
  display.clearDisplay();
  display.display();
  delay(700);
  }
    
  money = money + (g_score);
  display.clearDisplay();
  display.drawBitmap(10, 20, moneybag_bmp, 12, 11, 1); //Money
  display.setCursor(25,22);
  display.print(money);
  display.display();
  delay(1000);  
  closegame();
}
