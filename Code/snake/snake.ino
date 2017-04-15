//Programa: Snake com Arduino Uno
//Alteracoes e adaptacoes: FILIPEFLOP, Cogumelinho e gsrbr
//
//Baseado no codigo original de Abhinav Faujdar *****/
 
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>
 
//Inicializa o display
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);
 
#define LEFT 7
#define DOWN 5
#define RIGHT 4
#define UP 6
#define speakerPin 3
 
//Verifica em qual direcao a cobra esta se movendo
boolean dl = false, dr = false, du = false, dd = false;
 
int x[50], y[50], i, slength, tempx = 10, tempy = 10, xx, yy;
unsigned int high;
uint8_t bh, bl;
int xegg, yegg;
int freq, tb;
int l, r, u, d, p;
unsigned long time = 280, beeptime = 50;
int score = 0, flag = 0;
 
void setup()
{
  Serial.begin(9600);
  display.begin();
 
  //Informacoes iniciais display
  display.setContrast(50);
  display.clearDisplay();
  display.drawRoundRect(0, 0, 84 , 25, 1, 2);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(12, 6);
  display.println("SNAKE");
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(12, 29);
  display.println("COGUMELINHO");
  display.setCursor(13, 29);
  display.println("COGUMELINHO");
  display.setCursor(0, 40);
  display.println("Se Inscreva");
 
  display.display();
  delay(2000);
 
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(speakerPin, OUTPUT);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);
 
  slength = 8;
  xegg = (display.width()) / 2;
  yegg = (display.height()) / 2;
  display.clearDisplay();
  display.drawRect(0, 0, 84, 48, 1);
  display.setCursor(4, 2);
  display.print("P:     R:");
  display.setCursor(22, 2);
  display.print(score);
  display.setCursor(64, 2);
  display.print(high);
  display.drawRect(0, 0, 84, 11, 1);
  //Define coordenadas iniciais
  for (i = 0; i <= slength; i++)
  {
    x[i] = 25 - 3 * i;
    y[i] = 30;
  }
  for (i = 0; i < slength; i++)  //Draw the snake
  {
    display.drawCircle(x[i], y[i], 1, BLACK);
  }
  display.display();
  dr = true;
}
 
void loop()
{
  movesnake();
}
 
void movesnake()
{
  //Leitura do valor dos botoes
  l = digitalRead(LEFT);
  d = digitalRead(DOWN);
  r = digitalRead(RIGHT);
  u = digitalRead(UP);
 
  if (flag == 0)
  {
    direct();
  }
 
  if (millis() % time == 0)
  {
    if (flag == 0)
    {
      if (dr == true) {
        tempx = x[0] + 3;
        tempy = y[0];
      }
      if (dl == true) {
        tempx = x[0] - 3;
        tempy = y[0];
      }
      if (du == true) {
        tempy = y[0] - 3;
        tempx = x[0];
      }
      if (dd == true) {
        tempy = y[0] + 3;
        tempx = x[0];
      }
    }
    flag = 0;
    //Verifica se a cobra esta nas mesmas coordenadas do ovo
    checkgame();
    checkegg();
 
    //Altera as coordenadas de todos os pontos da cobra
    for (i = 0; i <= slength; i++)
    {
      xx = x[i];
      yy = y[i];
      x[i] = tempx;
      y[i] = tempy;
      tempx = xx;
      tempy = yy;
    }
    //Desenha a cobra e o ovo nas novas coordenadas
    drawsnake();
  }
}
 
void checkgame()
{
  for (i = 1; i < slength; i++)
  {
    //Verifica se o recorde foi batido e
    //mostra na tela o novo valor
    high = (((0xff00 + bh) << 8) + bl);
    if (score > high)
    {
      high = score;
      bh = (high >> 8);
      bl = high & 0xff;
      display.fillRect(61, 1, 20, 9, 0);
      display.setCursor(65, 2);
      display.print(high);
    }
    //Verifica se tocou nas bordas
    if ((x[0] <= 1 || x[0] >= 83) || (y[0] <= 12 || y[0] >= 47) || (x[i] == x[0] && y[i] == y[0]) )
    {
      //Jogo terminado. Mostra informacoes na tela
      display.clearDisplay();
      display.fillRoundRect(0, 0, 84 , 31, 1, 2);
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(18, 1);
      display.print("GAME");
      display.setCursor(18, 16);
      display.print("OVER");
      display.setTextColor(BLACK);
      display.setTextSize(1);
      display.setCursor(12, 33);
      display.print("PLACAR");
      display.setCursor(60, 33);
      display.print(score);
      display.setCursor(12, 41);
      display.print("RECORDE");
      display.setCursor(60, 41);
      display.print(high);
      display.display();
      //Aguarda 5 segundos e reinicia o jogo
      delay(5000);
 
      //Apaga a tela
      display.clearDisplay();
      //Retorna aos valores iniciais
      slength = 8;
      score = 0;
      time = 280;
      redraw();
    }
  }
}
 
void checkegg()      //Snake meets egg
{
  //Verifica se a cobra está nas mesmas coordenadas do ovo
  if (x[0] == xegg or x[0] == (xegg + 1) or x[0] == (xegg + 2) or x[0] == (xegg - 1))
  {
    if (y[0] == yegg or y[0] == (yegg + 1) or y[0] == (yegg + 2) or y[0] == (yegg - 1))
    {
      //Incrementa a cobra em uma posição, incrementa o placar e
      //aumenta a velocidade do jogo, diminuindo o tempo
      score += 1;
      display.fillRect(21, 1, 20, 9, 0);
      display.setCursor(22, 2);
      display.print(score);
      slength += 1;
      if (time >= 90)
      {
        time -= 10;
      }
      display.fillRect(xegg, yegg, 3, 3, WHITE);
      display.display();
      beep(35, beeptime);
      //Cria novo ovo em posicao aleatoria
      xegg = random(2, 80);
      yegg = random(15, 40);
    }
  }
}
 
void direct()
{
  //Altera o movimento caso uma tecla tenha sido pressionada
  if (l == LOW and dr == false)
  {
    dl = true; du = false; dd = false;
    //Salva novas coordenadas em Tempx, Tempy
    tempx = x[0] - 3;
    tempy = y[0];
    flag = 1;
  }
  else if (r == LOW and dl == false)
  {
    dr = true; du = false; dd = false;
    tempx = x[0] + 3;
    tempy = y[0];
    flag = 1;
  }
  else if (u == LOW and dd == false)
  {
    du = true; dl = false; dr = false;
    tempy = y[0] - 3;
    tempx = x[0];
    flag = 1;
  }
  else if (d == LOW and du == false)
  {
    dd = true; dl = false; dr = false;
    tempy = y[0] + 3;
    tempx = x[0];
    flag = 1;
  }
}
 
void drawsnake()
{
  display.fillRect(xegg, yegg, 3, 3, BLACK);
  display.drawCircle(x[0], y[0], 1, BLACK);
  display.drawCircle(x[slength], y[slength], 1, WHITE);
  display.display();
}
 
void redraw()
{
  display.drawRect(0, 0, 84, 48, 1);
  display.drawRect(0, 0, 84, 48, 1);
  display.setCursor(4, 2);
  display.print("P:     R:");
  display.drawRect(0, 0, 84, 11, 1);
  display.fillRect(21, 1, 20, 9, 0);
  display.setCursor(22, 2);
  display.print(score);
  display.fillRect(61, 1, 20, 9, 0);
  display.setCursor(65, 2);
  display.print(high);
 
  xegg = (display.width()) / 2;
  yegg = (display.height()) / 2;
  dl = false, dr = false, du = false, dd = false;
  dr = true;
  display.setCursor(4, 2);
  display.print("P:     R:");
  display.drawRect(0, 0, 84, 11, 1);
  //Retorna as coordenadas iniciais
  for (i = 0; i <= slength; i++)
  {
    x[i] = 25 - 3 * i;
    y[i] = 30;
  }
  tempx = 33 - 3 * i;
  tempy = 30;
  display.display();
}
 
void beep (int freq, long tb)
{
  int x;
  long delayAmount = (long)(500 / freq);
  long loopTime = (long)(tb / (delayAmount * 2));
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delay(delayAmount);
    digitalWrite(speakerPin, LOW);
    delay(delayAmount);
  }
  delay(2);
}
