// Projeto 37 – Jogo do Ping Pong com LCD Nokia 5110 
//Edited by Cogumelinho and gsrbr

// Setando os pinos do LCD Nokia 5110 no Arduino Uno
#define PIN_SCE   11
#define PIN_RESET 12
#define PIN_DC    10
#define PIN_SDIN  9
#define PIN_SCLK  8

#define PIN_BUZZER 3

#define pot 2

#define LCD_C     LOW
#define LCD_D     HIGH

// Inicializa o LCD com apenas 504 pixels de resolução.
#define LCD_X     84
#define LCD_Y     6

int barWidth = 14;  // Comprimento das barras.
int barHeight = 4;  // Largura das barras.
int ballPerimeter = 5;    // Tamanho da bola.
unsigned int bar1X = 0;
unsigned int bar1Y = 0;
unsigned int bar2X = 0;
unsigned int bar2Y = LCD_Y * 8 - barHeight;
int ballX = 0;
int ballY = 0;
boolean isBallUp = false;
boolean isBallRight = true;

byte pixels[LCD_X][LCD_Y];
unsigned long lastRefreshTime;
const int refreshInterval = 150;
byte gameState = 3; // Estágios do game.
byte ballSpeed = 4; // Velocidadeda bola.
byte player1WinCount = 0;
byte player2WinCount = 0;
byte hitCount = 0;

void setup(){  
  
  pinMode(PIN_BUZZER, OUTPUT);
  // Inicializa as funções abaixo.
  LcdInitialise();
  restartGame();
pinMode(pot, OUTPUT);
digitalWrite(pot,HIGH);
}
void loop(){    
  
  unsigned long now = millis();
  if(now - lastRefreshTime > refreshInterval){
      update();
      refreshScreen();
      lastRefreshTime = now;  
  }  
}


// Função reinciar jogo.
void restartGame(){
  
   ballSpeed = 1;
   gameState = 1;
   ballX = random(0, 60);
   ballY = 20;
   isBallUp = false; 
   isBallRight = true;
   hitCount = 0;
   
  }


// Função atualzar tela do LCD.
void refreshScreen(){

  if(gameState == 1){
    for(int y = 0; y < LCD_Y; y++){
     for(int x = 0; x < LCD_X; x++){
       byte pixel = 0x00;
       int realY = y * 8;
           
 // Inicia a bola na tela.
 if(x >= ballX && x <= ballX + ballPerimeter -1 && ballY + ballPerimeter > 
 realY && ballY < realY + 8 ){
             byte ballMask = 0x00;
             for(int i = 0; i < realY + 8 - ballY; i++){
               ballMask = ballMask >> 1;
               if(i < ballPerimeter)
                 ballMask = 0x80 | ballMask;
             }
             pixel = pixel | ballMask;
           }
           
// Inicia a barra na tela.
if(x >= bar1X && x <= bar1X + barWidth -1 && bar1Y + barHeight > realY &&
bar1Y < realY + 8 ){
             byte barMask = 0x00;
             for(int i = 0; i < realY + 8 - bar1Y; i++){
               barMask = barMask >> 1;
               if(i < barHeight)
                 barMask = 0x80 | barMask;
             }
             pixel = pixel | barMask;
           }
           
if(x >= bar2X && x <= bar2X + barWidth -1 && bar2Y + barHeight > realY 
&& bar2Y < realY + 8 ){
             byte barMask = 0x00;
             for(int i = 0; i < realY + 8 - bar2Y; i++){
               barMask = barMask >> 1;
               if(i < barHeight)
                 barMask = 0x80 | barMask;
             }
             pixel = pixel | barMask;
           }
           LcdWrite(LCD_D, pixel);
         }
    }
  } else if(gameState == 2){
    // Faz a tratativa dos estágios do jogo.
    int pinOut = 8;
    int freq = 150;
    int duration = 1000;
    tone(pinOut, freq, duration);
  
  }
   
}

void update(){
    
  if(gameState == 1){
     int barMargin = LCD_X - barWidth;
  // Lê os joysticks e posiciona as barras no LCD.
     int joystick1 = analogRead(A0); 
     int joystick2 = analogRead(A1);
     bar1X = joystick1 / 2 * LCD_X / 512;
     bar2X = joystick2 / 2 * LCD_X / 512;
    
     if(bar1X > barMargin) bar1X = barMargin;
     if(bar2X > barMargin) bar2X = barMargin;
     
     // Movimento da bola.
     if(isBallUp)
       ballY -= ballSpeed;
     else
       ballY += ballSpeed;
     if(isBallRight)
       ballX += ballSpeed;
     else
       ballX -= ballSpeed;
       
     // Verifica se ouve colisões.  
     if(ballX < 1){
       isBallRight = true;
       ballX = 0;
     }
     else if(ballX > LCD_X - ballPerimeter - 1){
       isBallRight = false;
       ballX = LCD_X - ballPerimeter;
     }
     if(ballY < barHeight){
   
   // Faz com que a bola ping para a barra 1.
       if(ballX + ballPerimeter >= bar1X && ballX <= bar1X + barWidth){ 
         isBallUp = false;
         if(ballX + ballPerimeter/2 < bar1X + barWidth/2)
           isBallRight = false;
         else
           isBallRight = true;
         ballY = barHeight;
         if(++hitCount % 10 == 0 && ballSpeed < 5) 
           ballSpeed++;
       }else{   
         // Jogador 2 é o vencedor.
         gameState = 2;
         player2WinCount++;
       }
     }
     if(ballY + ballPerimeter > LCD_Y * 8 - barHeight){
   
   // Faz com que a bola ping para a barra 2.
       if(ballX + ballPerimeter >= bar2X && ballX <= bar2X + barWidth){ 
         isBallUp = true; 
         if(ballX + ballPerimeter/2 < bar2X + barWidth/2)
           isBallRight = false;
         else
           isBallRight = true;
         ballY = LCD_Y * 8 - barHeight - ballPerimeter;
         if(++hitCount % 10 == 0 && ballSpeed < 5) 
           ballSpeed++;
       }
       else{ // Jogar 1 é o vencedor.
         gameState = 2;
         player1WinCount++;
       }
     }
  }else if(gameState == 2){
      for(int i =0; i < 4; i++){
        LcdWrite(LCD_C, 0x0D );  // LCD no modo inverso.
        delay(300);
        LcdWrite(LCD_C, 0x0C );  
        delay(300);
      }
      restartGame(); // Chama da função para resetar o game.
  }  
}

// Inicializa o LCD.
void LcdInitialise(void){  

  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  delay(200);
  digitalWrite(PIN_RESET, LOW);
  delay(500);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // Comandos ampliados do LCD.
  LcdWrite(LCD_C, 0xB1 );  // Configura o contraste do LCD. 
  LcdWrite(LCD_C, 0x04 );  // Configura o tempo de coeficiente.
  LcdWrite(LCD_C, 0x14 );  
  LcdWrite(LCD_C, 0x0C );  // LCD em modo normal.
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x80 ); 
  LcdWrite(LCD_C, 0x40 ); 
  LcdWrite(LCD_C, 0x0C );
}

void LcdWrite(byte dc, byte data){

  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}
