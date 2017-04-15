/* 
 * Arduitochi V0.4.6 by JKA Network for Arduino
 * Edited by Cogumelinho and gsrbr
 */

#include <SPI.h> //LCD communication
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>
#include "imagenesbmp.h" //File imagenesbmp.h containing image arrays of arduitochi


//Arduino pins and screenconfig
const byte snd_pin = 3; //Sound pin
const byte btnA_pin = 7;
const byte btnB_pin = 4;
const byte btnC_pin = 5;
//Screen
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12); //Screen (SCLK,DIN,D/C,CS,RST)
const bool scr_rotate = 0; //If screen is inverted (up down), change to 0 for view right
const byte scr_contrast = 55; //Screen contrast (0 to 100)
//End Arduino config

byte menu[3] = {0,0,0}; //This records where are you (Position of menus and submenus)
byte ingame = 0; //If its playing, this is != 0 and interrupts all normal activities
byte usingobject[2] = {0,0}; //If its using object, this is {1,item}
byte posx = 15;
byte posy = 15;

byte hungry = 2;
byte happiness = 3;
byte poop = 0; //Has poop?
bool dirty = 0; //If arduitochi doesnt go to bathroom in much time, they makes dirty
unsigned int money = 0;
byte weight = 5; //Weight of ardu
bool items[20]; //Array of 1 or 0. The position of the array is the "object". 1 is bought, 0 or NULL it not.
bool outsideitems[20]; //Like before, but outside items
bool isoutside = 0; //Is ardu outside?

//Constants for decreasing values like hunger (With num is the "limit" for decresing, without it, is the actual number. When var equals numvar,ocurrs
const int numlowerhungry = 1250; 
const int numlowerhappiness = 1600;
const int numdopoop = 900; 
int lowerhappiness = 0;
int lowerhungry = 0;
int dopoop = 0; 
//

//
unsigned long timeevolve[2]; //This have the seconds since last "evolution", and time moving. {time elapsed, time last evolve};
//

int varproc = 0; //Process time counting var (For eating and standby)
bool vardirection = 0; //Stores horizontal direction of arduitochi 
bool buyit = 0; //This turns 1 when I want to buy something
byte shopitems[2] = {0,1}; //In shop items
const int shopprices[] = {100,900}; //Prices of the items
byte actual_ardu[3] = {0,1,0}; //The arduitochi you have {stage, tama,sex}


byte soundid = 0; //Var for sound playing, if its 0, means none
byte soundstate = 0; //State of the sound. 0 is not playing nothing
byte ralensignal[3] = {0,0,0}; //This is for knowing how much time runs in multitasking. (In SIGNAL(TIMER0_COMPA_vect) works in 10,50,1000ms {10,5,20})
int beenalone = 0; //how much time arduitochi has been alone
bool vardie = 0; //Arduitochi die var

byte limitbuttontime = 0; //To avoid "Double-triple clic" with some weird phisical buttons and arduino

void setup(){

  randomSeed(analogRead(0)); //For random operations

  ADCSRA = 0; //Disable ADC (Disable analog reads after this for energy save. Not used.)
  
  //This makes the interrupt SIGNAL(TIMER0_COMPA_vect) { } every 1 ms, doing some "multitasking"/background process
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  //
  
  display.begin();
  /* Setup pull-up buttons */
  pinMode(btnA_pin, INPUT); //btnA
  pinMode(btnB_pin, INPUT); //btnB 
  pinMode(btnC_pin, INPUT); //btnC
  digitalWrite(btnA_pin, HIGH); //Pull up buttons
  digitalWrite(btnB_pin, HIGH);
  digitalWrite(btnC_pin, HIGH);
  /* End setup pull-up buttons */
  
  display.setRotation(scr_rotate * 2); //Rotate display 180 degrees if rotatescr = 1
  display.setContrast(scr_contrast); //Screen contrast
  display.display(); // show adafruit splashscreen
  delay(350);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Arduitochi");
  display.setCursor(0, 10);
  display.print("V0.4.6");
  display.setCursor(0, 20);
  display.print("JKA Network");

  if (EEPROM.read(0) == 0){ //If save not found, found, new game
  display.setCursor(0, 35);
  display.print("New game");
  display.display();
  delay(750);
  display.clearDisplay();
  display.setCursor(12, 10);
  display.print("It's a egg?");
  display.drawBitmap(34, 38, egg_bmp, 16, 10, 1);
  display.display();
  delay(2100);
  display.clearDisplay();
  actual_ardu[2] = random(0,2); //¿It will be a boy or a girl?
  display.setCursor(12, 12);
  if (actual_ardu[2] == 0){display.print("Is a boy!");}else{display.print("Is a girl!");}
  display.drawBitmap(38, 40, tama01_bmp, 16, 8, 1); //16x8
  display.display();
  delay(800);
  }
  
  if (EEPROM.read(0) != 0){  //If save 'file' found
  display.setCursor(0, 35);
  display.print("Loading...");
  display.display();
  eeprom_load();
  }
  delay(600);
  pinMode(A0, INPUT); //Thinking for what is this
}




void loop() {
  if (limitbuttontime > 0){limitbuttontime--;} //For be able to press a button in a short time (Avoids bad clicks detected by Arduino)
  if (vardie == 0) { //If it's alive
    if (ingame == 0) {
      display.clearDisplay(); //Erase display
      dibujarpantalla();
      pasodeltiempo();
      beenalone++;
      display.display(); //Put updates on display
      evolve(); //It checks if it has to evolve
    }else if(ingame != 0) {
      display.clearDisplay();
      gameprocess();
      display.display();
    }
    delay(100); //10FPS , limiting program too to 10 loops/sec

  }
  if (vardie == 1) { //If it's dead
     display.clearDisplay();
     display.setCursor(12,4);
     display.print("Arduitochi");
     display.setCursor(18,12);
     display.print("Has died");
     display.drawBitmap(34, 26, grave_bmp, 16, 15, 1); //R.I.P. bmp
     display.display();
     soundid = 10;
     delay(30000);
  }
}

void btnA() {
  beenalone = 0; //You make arduitochi not alone
  
  if (ingame == 0) { //if it's not playing, normal procedures
    if (menu[1] == 0) {
      menu[0] = menu[0] + 1; //If nothing is selected, continue selecting
      return;
    }
    if (menu[1] != 0 && menu[2] == 0) {
      menu[1] = menu[1] + 1; //Change option for menu[1]
      return;
    }
    if (menu[2] != 0 && menu[0] == 4) {
      menu[2] = menu[2] + 1; //Change option in game/shop menu
      return;
    }
  } //End normal use of buttons

  if (ingame != 0){gamebtn1();}
}

void btnB() {
  beenalone = 0;
  if (ingame == 0) { 
  //Only executes the first coindicence
    if (menu[0] == 4 && menu[1] == 1 && menu[2] != 0) {if (weight >= 6){weight -= 2;varproc = 0;ingame = menu[2];return;}else{soundid=1;return;}} //Load a game
    if (menu[0] == 4 && menu[1] == 2 && menu[2] != 0) {buyit = !buyit;return;} //Buy it
    if (menu[0] == 4 && menu[1] == 3 && menu[2] != 0) {usingobject[0] = 1;return;} //Play with item
    if (menu[0] == 6 && menu[2] == 1){menu[1]++;return;} 
    if (menu[0] == 2 && menu[1] != 0 && menu[2] == 1){varproc = 99;return;} //Faster eating
    if (menu[0] == 2 && menu[1] != 0) {menu[2] = 1;varproc = 0;return;} //Making ardu eat (Restore varproc to 0 for avoid bugs)
    if ((menu[0] == 3 || menu[0] == 4 || menu[0] == 6) && menu[1] != 0) {menu[2] = 1;varproc = 0;return;} //Select into food, bathroom,games,save. I reset varproc for item array proposses
    
    if (menu[0] != 0 && menu[1] == 0) {menu[1] = 1;menu[2] = 0;return;} //If an option is selected (menu[0] != 0)

  }

  if (ingame != 0){gamebtn2();}
}

void btnC() {
  beenalone = 0;
  if (usingobject[0] == 1){usingobject[0] = 0;return;}
  if (ingame == 0){
    if (menu[0] == 0) { //Make something cool
      //
    }
    if (menu[1] == 0) { //Cancel the "rectangle" over the option
      menu[0] = 0;
      return;
    }
    if (menu[1] != 0 && menu[2] == 0) { //Go back from a menu
      menu[1] = 0;
      return;
    }
    if (menu[0] == 4 && menu[2] != 0) { //Go back from the game/item select
      menu[2] = 0;
      return;
    }
    if (menu[0] == 6 && menu[2] != 0) { //Go back of all in options menu
      menu[2] = 0;
      menu[1] = 0;
      return;
    }
  }

  if (ingame != 0){gamebtn3();}
}

void dibujarpantalla() {
  display.drawLine(0, 10, 84, 10, BLACK); //x0,y0,x1,y1 //Separation line

  //Menu icons
  display.drawBitmap(1, 1, ardumenu_status_bmp, 10, 8, 1); //Status
  display.drawBitmap(14, 1, ardumenu_food_bmp, 10, 8, 1); //Eat
  display.drawBitmap(27, 1, ardumenu_cleaning_bmp, 10, 8, 1); //WC
  display.drawBitmap(40, 1, ardumenu_gameshop_bmp, 10, 8, 1); //Games/Shopping/Items
  display.drawBitmap(53, 1, ardumenu_mail_bmp, 10, 8, 1); //Mail
  display.drawBitmap(66, 1, ardumenu_options_bmp, 10, 8, 1); //Options
  //
  switch (menu[0]) {
    case 1:
      display.drawRect(0, 0, 12, 10, 1);
      break;
    case 2:
      display.drawRect(13, 0, 12, 10, 1);
      break;
    case 3:
      display.drawRect(26, 0, 12, 10, 1);
      break;
    case 4:
      display.drawRect(39, 0, 12, 10, 1);
      break;
    case 5:
      display.drawRect(52, 0, 12, 10, 1);
      break;
    case 6:
      display.drawRect(65, 0, 12, 10, 1);
      break;
  }
  if (menu[0] >= 7) {
    menu[0] = 0;
  }
  if (menu[1] == 0) { //If it's in standby
    if (isoutside == 0){
      ardudraw(posx, posy, 1);
      if (poop == 1) {
        display.drawBitmap(66, 40, poop_bmp, 10, 8, 1); //poop
      }
      if (dirty == 1) {
        display.drawBitmap(posx-4, posy-3, dirtylines_bmp, 8, 3, 1); //dirty
        display.drawBitmap(posx+8, posy-3, dirtylines2_bmp, 8, 3, 1); 
      }
    }
   if (isoutside == 1){
    outside_draw();
   }
  }
  if (menu[1] != 0) {
    switch (menu[0]) {
      case 1:
        menu_status_draw();
        break;
      case 2:
        menu_meal_draw();
        break;
      case 3:
        menu_wc_draw();
        break;
      case 4:
        menu_gameshop_draw();
        break;
      case 5: //Mail
        display.setCursor(20, 20);
        display.print("No mail");
        display.setCursor(20,28);
        display.print("for you");
        break;
      case 6:
        menu_options_draw();
        break;
    }
  }

}

void menu_status_draw() {
  if (menu[1] >= 3) {
    menu[1] = 1;
  }
  byte x = 0;
  switch (menu[1]) {
    case 1:
      display.setCursor(4, 12);
      display.print("Hungry:");
      for (x = 0; x < hungry; x++) {
        display.print("+");
      }
      for (byte y = x; y < 4; y++) {
        display.print("-");
      }
      display.setCursor(4, 22);
      display.print("Happy:");
      for (x = 0; x < happiness; x++) {
        display.print("+");
      }
      for (byte y = x; y < 4; y++) {
        display.print("-");
      }
      display.setCursor(4,32);
      display.print("Weight: ");
      display.print(weight);
    break;
    case 2:
      display.setCursor(4, 12);
      display.print("Money");
      display.drawBitmap(10, 20, moneybag_bmp, 12, 11, 1); //Money bag
      display.setCursor(25,22);
      display.print(money);
    break;
  }
}

void menu_meal_draw() {
  if (menu[1] >= 3) {
    menu[1] = 1;
  }
  if (menu[2] == 0) { //Mientras no ha elegido la comida
    display.setCursor(4, 12);
    display.print("Select food:");
    switch (menu[1]) {
      case 1:
        display.setCursor(4, 22);
        display.print("Soup");
        display.drawBitmap(38, 30, food_soup_bmp, 10, 8, 1);
        break;
      case 2:
        display.setCursor(4, 22);
        display.print("Rice");
        display.drawBitmap(38, 30, food_rice_bmp, 12, 8, 1);
        break;
    }
  }

  if (menu[2] != 0) //Eating
  {
    varproc++;
    display.setCursor(4, 12);
    display.print("Eating");
    posx = 15;
    posy = 25;
    ardudraw(posx, posy, 0);
    dibujocomida();
    if (varproc >= 30) {
      weight +=2;
      if (hungry < 5){hungry++;}
      varproc = 0;
      menu[2] = 0;
    }

  }
}

void menu_wc_draw()
{
  if (menu[2] == 0) {
    display.setCursor(12, 12);
    display.print("Go to WC");
    display.setCursor(12, 20);
    display.print("Go to bath");
    if (menu[1] >= 3) {
      menu[1] = 1;
    }
    switch (menu[1]) {
      case 1:
        display.setCursor(0, 12);
        display.print("->");
        break;
      case 2:
        display.setCursor(0, 20);
        display.print("->");
        break;
    }

  }
  if (menu[2] == 1)
  {
    switch (menu[1]) {
      case 1:
        if (poop == 0){soundid=1;menu[2] = 0;break;} //If no poop, no WC
        display.setCursor(4, 12);
        display.print("WC (8)");
        display.display();
        delay(1000);
        poop = 0;
        dopoop = 0;
        menu[1] = 0;
        menu[2] = 0;
        break;
      case 2:
        display.setCursor(4, 12);
        display.print("Shower (8)");
        display.display();
        delay(1000);
        dirty = 0;
        poop = 0;
        dopoop = 0;
        menu[1] = 0;
        menu[2] = 0;
        break;
    }
  }

}

void menu_gameshop_draw()
{
  if (menu[2] == 0) {
    display.setCursor(16, 12);
    display.print("Games");
    display.setCursor(16, 20);
    display.print("Shop");
    display.setCursor(16, 28);
    display.print("Items");
    display.setCursor(16, 36);
    if (isoutside == 0){ display.print("Go outside");}
    if (isoutside == 1){ display.print("Go inside");}
    if (menu[1] >= 5) {
      menu[1] = 1;
    }
    display.setCursor(4, 12+((menu[1]-1)*8));
    display.print("->");

  }
  if (menu[2] != 0 && usingobject[0] == 0)
  {
    switch (menu[1]) {
      case 1: //Games
        display.setCursor(16, 12);
        display.print("Jump");
        display.setCursor(16, 20);
        display.print("Avoid");
        display.setCursor(16, 28);
        display.print("Fall");
        if (actual_ardu[0] > 0){ //If it's not a baby
          display.setCursor(16, 36);
          display.print("Math");
          if (menu[2] >= 5) {
            menu[2] = 1;
          }
          }else{ //If it's a baby
          if (menu[2] >= 4) {
            menu[2] = 1;
          }
        }
        display.setCursor(4, 12+((menu[2]-1)*8));
        display.print("->");
      break;
      case 2: //Shop
      menu_sub_shop_draw();
      break;
      case 3: //Items
        varproc = 0;
       for (byte z = 0;z <= 19;z++){ //Calculate how much objets are buyed
          if (items[z] != 0){varproc++;}
          if (varproc == menu[2]){ //When is the item pos
            item_draw(z,0);
            usingobject[1] = z; //Store item temporaly
            varproc++; //No more coincidences adding 1 to varproc
          }
        }
        if (varproc < menu[2]){menu[2] = 1;} //If it not found anymore objects,reset list
        if (varproc == 0){ //No objets
          display.setCursor(10, 16);
          display.print("No objets");
          display.setCursor(10, 26);
          display.print("Buy some ;)");
        }
        varproc = 0; //For avoiding varproc being infinite
      break;
      case 4:
      isoutside = !isoutside;
      menu[2] = 0;
      menu[1] = 0;
      menu[0] = 0;
      break;
    } 
  }
  if (usingobject[0] == 1){
    using_item_draw();
  }

}


void menu_options_draw()
{
  if (menu[2] == 0) {
    display.setCursor(11,14);
    display.print("Options:");
    display.setCursor(11, 22);
    display.print("Save game");
    display.setCursor(11, 30);
    display.print("Erase game");
    if (menu[1] >= 3) {
      menu[1] = 1;
    }
    display.setCursor(0, 22+((menu[1]-1)*8));
    display.print("->");

  }
  if (menu[2] != 0)
  {
    switch (menu[1]) {
      case 1:
        display.setCursor(4, 22);
        display.print("Saving...");
        display.display();
        eeprom_save();
        delay(750);
        menu[2] = 0;
        menu[1] = 0;
        menu[0] = 0;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        display.setCursor(0, 22);
        display.print("Are you sure?");
        display.print("Accept repeatedly");
        display.display();
        break;
      case 7:
      case 8:
        eeprom_erase();
        display.setCursor(0, 22);
        display.print("Game erased");
        display.setCursor(0,38);
        display.print("Press reset");
        display.display();
        delay(1000);
    }
  }

}

/*
 * pasodeltiempo()
 * This function check status of arduitochi, makes "time" useful making hungry,sad,...
 */
void pasodeltiempo() {
  lowerhungry++;
  lowerhappiness++;
  if (lowerhungry >= numlowerhungry && hungry > 0) {
    hungry--;
    lowerhungry = 0;
    if (hungry == 0) {soundid=7;} //Not so many time for die
  }
  
  if (lowerhappiness >= numlowerhappiness && happiness > 0) {
    happiness--;
    lowerhappiness = 0;
    if (happiness == 0) {soundid=7;} //Arduitochi is sad
  }
  if (lowerhungry == numlowerhungry && hungry == 0) {soundid=7;}
  if (lowerhungry >= (2 * numlowerhungry) && hungry == 0) {
    vardie = 1;/*Die*/
  }

  if (dopoop < 3*numdopoop) {dopoop++;}
  if (dopoop == numdopoop){poop = 1;}
  if (dopoop == 3*numdopoop){dirty = 1;}
  if (beenalone == 300){menu[2] = 0;menu[1] = 0;menu[0] = 0;} //Exit menu if not using
  if (beenalone >= 500 && hungry != 0){ //If you left arduitochi alone a long time, and it's not near die
    display.fillRect(0,11,84,37,1);
    display.setTextColor(0);
    display.setCursor(50,22);
    display.print("Z");
    display.setCursor(56,18);
    display.print("z");
    display.setCursor(62,16);
    display.print("z");
    display.setTextColor(1);
    delay(200);//Less FPS
    }
}

void dibujocomida() {
  switch (menu[1]) {
    case 1:
      display.drawBitmap(25, 30, food_soup_bmp, 10, 8, 1);
      break;
    case 2:
      display.drawBitmap(25, 30, food_rice_bmp, 12, 8, 1);
      break;
      //Draw food
  }

}

void ardudraw(byte tamax, byte tamay, byte azar) { //Draw arduitochi
  if (azar == 1) {
    varproc++;
    if (varproc >= 12) { //Change random pos of arduitochi (It can be better xd)
      varproc = 0;
      posx = random(15, 55);
      posy = random(17, 32);
    }
  }
  if (azar == 2) { //Movimiento horizontal
    varproc++;
    if (varproc >= 9){
      varproc = 0;
    switch (vardirection){
      case 0: //Va a la derecha
      posx +=6;
      if (posx > 76){posx -= 12;vardirection = 1;}
      break;
      case 1: //Va a la izquierda
      posx -=6;
      if (posx < 16){posx += 12;vardirection = 0;}
      }
    }
  }
  //Draw arduitochi. Ardu has a max of 16x10 width. It adds a +Y if is smaller than 10
  switch (actual_ardu[0]) {
    case 0:
      switch(actual_ardu[1]){
        case 1:
        display.drawBitmap(tamax, tamay+2, tama01_bmp, 16, 8, 1); //16x8 . tamay+2 for putting in the "ground" like 16x10 tamas.
        break;
      }
    break;
    case 1:
      switch(actual_ardu[1]){
        case 1:
        if (actual_ardu[2] == 0){ //Sex, male
          display.drawBitmap(tamax, tamay, tama110_bmp, 16, 10, 1);
        }else{ //Female
          display.drawBitmap(tamax, tamay, tama111_bmp, 16, 10, 1);
        }
        break;
      }
    break;
  }
}

void menu_sub_shop_draw(){ //Inside of the shop
  //First, I think that can be random every..1 hour or so, but for the moment, all is displayed
  switch (buyit){
  case 0: //Seeing shop
  if (menu[2] >= 3){menu[2] = 1;}
  item_draw(shopitems[menu[2]-1],1);
  break;
  case 1:  //Buy it
  buyit = 0;
  if (money > shopprices[shopitems[(menu[2]-1)]]) {
    money = money - shopprices[shopitems[(menu[2]-1)]];
    items[menu[2]-1] = 1;
  }
  break;
  }
}

void item_draw(byte item,byte drawprice){ //This draw items and data (shop e item list)
  switch (item){
  case 0:
  display.setCursor(26, 12);
  display.print("Plant");
  display.drawBitmap(34, 22, item_00_icon_bmp, 12, 12, 1);
  break;
  case 1:
  display.setCursor(18, 12);
  display.print("Computer");
  display.drawBitmap(32, 22, item_01_icon_bmp, 16, 12, 1);
  break;
  }
  if (drawprice == 1){
  display.setCursor(32, 36);
  display.print(shopprices[shopitems[(menu[2]-1)]]);
  }
}

void outside_draw(){
  ardudraw(posx, 32, 2);
  for (byte z = 0;z < 84;z+=5){
    display.drawLine(z,48,z+4,48-4,1);
    display.drawLine(z+2,48,z+4,48-2,1);
    z += 5;
  }
  display.drawBitmap(0,32,outsideitem_00_bmp,16,16,1);
}


SIGNAL(TIMER0_COMPA_vect) { //Hardware interrupt every 1ms
  ralensignal[0]++;
  
  if (ralensignal[0] >= 10){ //Interrupt every 10ms for checking button status. Works great. limitbuttontime is for avoidind "Double clic" with some weird phisical buttons
    ralensignal[0] = 0;
    ralensignal[1]++;
    if (digitalRead(btnA_pin) == 0 && limitbuttontime == 0){btnA();limitbuttontime = 2;} //A button
    if (digitalRead(btnB_pin) == 0 && limitbuttontime == 0){btnB();limitbuttontime = 2;} //B button
    if (digitalRead(btnC_pin) == 0 && limitbuttontime == 0){btnC();limitbuttontime = 2;} //C button
  }
  
  if (ralensignal[1] >= 5){ //sound "multitask" works every 50ms and not ever 1ms, reducing the load
    ralensignal[1] = 0;
    ralensignal[2]++;
    switch (soundid){
    case 0: //No sound
    soundstate = 0;
    break;
    case 1: //'pi' sound
    if (soundstate == 0){tone(snd_pin, 440,100);soundid=0;break;}
    break;
    case 2: //'pi' + 1 score sound
    if (soundstate == 0){tone(snd_pin, 800,60);soundid=0;break;}
    break;
    case 3: //Game Over sound
    if (soundstate == 0){tone(snd_pin, 440);break;}
    if (soundstate == 2){noTone(snd_pin);tone(snd_pin, 420);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 6){noTone(snd_pin);tone(snd_pin, 380);break;}
    if (soundstate == 8){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    case 4: //Poop
    if (soundstate == 0){tone(snd_pin, 440);break;}
    if (soundstate == 2){noTone(snd_pin);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 440);break;}
    if (soundstate == 6){noTone(snd_pin);break;}
    if (soundstate == 8){noTone(snd_pin);tone(snd_pin, 440);break;}
    if (soundstate == 10){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    case 5: //Im alone
    if (soundstate == 0){tone(snd_pin, 400);break;}
    if (soundstate == 1){noTone(snd_pin);break;}
    if (soundstate == 2){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 3){noTone(snd_pin);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 5){noTone(snd_pin);break;}
    if (soundstate == 6){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 7){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    case 6: //Something has occurred
    if (soundstate == 0){tone(snd_pin, 440);break;}
    if (soundstate == 1){noTone(snd_pin);break;}
    if (soundstate == 2){noTone(snd_pin);tone(snd_pin, 440);break;}
    if (soundstate == 3){noTone(snd_pin);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 500);break;}
    if (soundstate == 6){noTone(snd_pin);break;}
    if (soundstate == 7){noTone(snd_pin);tone(snd_pin, 560);break;}
    if (soundstate == 8){noTone(snd_pin);break;}
    if (soundstate == 9){noTone(snd_pin);tone(snd_pin, 560);break;}
    if (soundstate == 10){noTone(snd_pin);break;}
    if (soundstate == 11){noTone(snd_pin);tone(snd_pin, 620);break;}
    if (soundstate == 12){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    case 7: //My life is in danger
    if (soundstate == 0){tone(snd_pin, 400);break;}
    if (soundstate == 1){noTone(snd_pin);break;}
    if (soundstate == 2){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 3){noTone(snd_pin);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 5){noTone(snd_pin);break;}
    if (soundstate == 6){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 7){noTone(snd_pin);break;}
    if (soundstate == 8){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 9){noTone(snd_pin);break;}
    if (soundstate == 10){noTone(snd_pin);tone(snd_pin, 400);break;}
    if (soundstate == 11){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    case 10: //I dead
    if (soundstate == 0){tone(snd_pin, 200);break;}
    if (soundstate == 4){noTone(snd_pin);tone(snd_pin, 180);break;}
    if (soundstate == 6){noTone(snd_pin);tone(snd_pin, 160);break;}
    if (soundstate == 8){noTone(snd_pin);tone(snd_pin, 140);break;}
    if (soundstate == 10){noTone(snd_pin);soundstate = 0;soundid=0;break;}
    break;
    }
    if (soundid != 0){soundstate++;} //For doing sounds, this go forward in the sound time
  }
  if (ralensignal[2] == 20){ //1 sec.
    ralensignal[2] = 0;
    timeevolve[0]++;
  }
}

void evolve(){
   switch (actual_ardu[0]){
    case 0:
      if ((timeevolve[0] - timeevolve[1])  > 130){ //130 sec min for evolving
        timeevolve[1] == timeevolve[0];
        evolvedelay();
        actual_ardu[0]++;
      }
   }
}

void evolvedelay(){
  for (varproc = 0;varproc < 2;varproc++){
  display.clearDisplay();
  display.setCursor(0,12);
  display.print("Evolving...!");
  ardudraw(38,38,0); 
  display.display();
  delay(1500);
  display.clearDisplay();
  display.setCursor(0,12);
  display.print("Evolving...!");
  display.drawRect(34, 34, 16, 12, 1);
  display.fillRect(34, 34, 16, 12, 1);
  display.display();
  delay(1500);
  }
}

void eeprom_load(){
  /* EEPROM map
   * 0 - Version
   * 1 - Hungry
   * 2 - Happiness
   * 3,4,5,6 - Money (2 position int, I keep 4 positions for be sure that it will not be replaced)
   * 7 - Weight
   * 8 - Age
   * 50 to 90 - Items (Yes, the rest is 'free',but is better to reserve free space)
   * 90 to 120 - House/ever visible items
   */
  EEPROM.get(1,hungry);
  EEPROM.get(2,happiness);
  EEPROM.get(3, money);
  EEPROM.get(7, weight);
  EEPROM.get(50, items); //From 50 to 90 
  EEPROM.get(90, outsideitems);
}

void eeprom_erase(){
  EEPROM.write(0,0); //Version to 0 makes aruitochi think that there is no game
}

void eeprom_save(){
  EEPROM.put(0,1); //Version
  EEPROM.put(1,hungry);
  EEPROM.put(2,happiness);
  EEPROM.put(3, money); //2 bytes int (I leave 4 in total just in case)
  EEPROM.put(7, weight);
  EEPROM.put(50, items);
  EEPROM.put(90, outsideitems);
}
