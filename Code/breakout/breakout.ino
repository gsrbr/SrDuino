// Jogo "breakout" com o display Nokia 5110
//Alteracoes e adaptacoes:Cogumelinho e gsrbr
//
//Baseado no codigo original de Daniel Quadros *****/

//#define TESTE

// Conexões do display ao Arduino
#define PIN_SCE   11
#define PIN_RESET 12
#define PIN_DC    10
#define PIN_SDIN  9
#define PIN_SCLK  8

// Conexão dos botões
#define BOTAO_ESQ 7
#define BOTAO_DIR 4

// Seleção de dado ou comando
#define LCD_CMD   LOW
#define LCD_DAT   HIGH

// Tamanho da tela
#define LCD_DX    84
#define LCD_DY    48

// Parametros para o jogo
#define NLIN_BLOCOS  20
#define TAM_RAQ      8

// Imagem da tela
static byte tela [LCD_DX][LCD_DY/8];

// Controle do jogo
static int posRaq;          // Posicao da raquete
static int xBola, yBola;    // Posicao da bola
static int dxBola, dyBola;  // Velocidade da bola
static int nBlocos;         // Número de blocos a apagar

// Envia um byte para o controlador do display
// dc:   LCD_CMD ou LCD_DAT
// data: byte a enviar
void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

// Iniciação do display
void LcdInitialise(void)
{
  // define a direção dos pinos de E/S
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  
  // conexões dos botões (com pullup interno)
  pinMode(BOTAO_ESQ, INPUT);
  digitalWrite(BOTAO_ESQ, HIGH);
  pinMode(BOTAO_DIR, INPUT);
  digitalWrite(BOTAO_DIR, HIGH);
  
  // executa um reset do controlador
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  
  // envia os comandos de iniciação
  LcdWrite( LCD_CMD, 0x21 ); // LCD Extended Commands.
  LcdWrite( LCD_CMD, 0xb0 ); // Set LCD Vop (Contraste)
  LcdWrite( LCD_CMD, 0x04 ); // Set Temp coefficent
  LcdWrite( LCD_CMD, 0x14 ); // LCD bias mode 1:48
  LcdWrite( LCD_CMD, 0x20 ); // LCD Basic Commands.
  LcdWrite( LCD_CMD, 0x0c ); // LCD no modo normal  
}

// Limpa a tela
void LcdClear(void)
{
  // posiciona ponteiro no inicio da memória
  LcdWrite( LCD_CMD, 0x40);  
  LcdWrite( LCD_CMD, 0x80);
  
  // preenche a memória com zeros
  for (int index = 0; index < LCD_DX * LCD_DY / 8; index++)
  {
    LcdWrite(LCD_DAT, 0x00);
  }
  for (int x = 0; x < LCD_DX; x++)
    for (int y = 0; y < LCD_DY/8; y++)
      tela [x][y] = 0;
}


// Altera um ponto da tela
void LcdSetPixel (int x, int y, int valor)
{
  int by = y >> 3;
  y = y & 7;
  if (valor)
    tela [x][by] |= 1 << y;
  else
    tela [x][by] &= ~(1 << y);
  LcdWrite( LCD_CMD, 0x40 + by);  
  LcdWrite( LCD_CMD, 0x80 + x);
  LcdWrite(LCD_DAT, tela[x][by]);
}

// Informa valor de ponto da tela
int LcdGetPixel (int x, int y)
{
  int by = y >> 3;
  y = y & 7;
  return tela[x][by] & (1 << y);
}

// Coloca uma nova bola em jogo
void NovaBola ()
{
  yBola = LCD_DY - 3;
  xBola = random (1, LCD_DX-1);
  dxBola = 1;
  dyBola = -1;
  LcdSetPixel (xBola, yBola, 1);
  LcdSetPixel (xBola+1, yBola, 1);
  LcdSetPixel (xBola, yBola+1, 1);
  LcdSetPixel (xBola+1, yBola+1, 1);
}

// Inicia um novo jogo
void NovoJogo ()
{
  int x, y;
  
  LcdClear();
  for (y = 0; y < NLIN_BLOCOS; y++)
    for (x = 0; x < LCD_DX; x++)
      if (((y & 3) != 3) && ((x & 3) != 3))
        LcdSetPixel (x, y, 1);
  posRaq = (LCD_DX - TAM_RAQ) / 2;
  for (x = 0; x < TAM_RAQ; x++)
    LcdSetPixel (posRaq+x, LCD_DY - 1, 1);
  nBlocos = NLIN_BLOCOS * LCD_DX / 16;
  NovaBola ();
}

// Apaga um bloco
void ApagaBloco (int x, int y)
{
  // Determina o ponto que bateu
  if (dyBola == 1)
    y++;
  if (dxBola == 1)
    x++;
  // Determina o canto superior esquerdo do bloco
  x = x & ~3;
  y = y & ~3;
  // Apaga
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      LcdSetPixel (x+i, y+j, 0);
}

// Testa se bateu em um bloco
int BateuBloco (int x, int y)
{
  return LcdGetPixel(x, y) || LcdGetPixel(x+1, y) ||
         LcdGetPixel(x, y+1) || LcdGetPixel(x+1, y+1);
}

// Move a bola
// Retorna true se bola saiu do campo
int MoveBola ()
{
  int nx, ny;
  nx = xBola + dxBola;
  ny = yBola + dyBola;
  LcdSetPixel (xBola, yBola, 0);
  LcdSetPixel (xBola+1, yBola, 0);
  LcdSetPixel (xBola, yBola+1, 0);
  LcdSetPixel (xBola+1, yBola+1, 0);
  if (nx < 0)
  {
    nx = 0;
    dxBola = 1;
  } else if (nx == LCD_DX-1)
  {
    nx--;
    dxBola = -1;
  }
  if (ny < 0)
  {
    ny = 0;
    dyBola = 1;
  } else if (ny == LCD_DY-1)
    return true;  // bola fugiu
  if (ny == (LCD_DY-2))
  {
    #ifdef TESTE
    if (1)
    #else
    if (LcdGetPixel(nx, ny+1))
    #endif
    {
      // bateu na raquete
      ny -= 2;
      dyBola = -1;
    }
  }  
  else if (BateuBloco(nx, ny))
  {
    // bateu em um bloco
    ApagaBloco (nx, ny);
    nBlocos--;
    dyBola = -dyBola;
    nx = xBola;
    ny = yBola;
  }
  xBola = nx;
  yBola = ny;
  LcdSetPixel (xBola, yBola, 1);
  LcdSetPixel (xBola+1, yBola, 1);
  LcdSetPixel (xBola, yBola+1, 1);
  LcdSetPixel (xBola+1, yBola+1, 1);
  return false;
}

// Move a raquete conforme o botão apertado
void MoveRaquete ()
{
  if ((digitalRead(BOTAO_ESQ) == LOW) && (posRaq > 0))
  {
    posRaq--;
    LcdSetPixel (posRaq, LCD_DY-1, 1);
    LcdSetPixel (posRaq+TAM_RAQ, LCD_DY-1, 0);
  }
  else if ((digitalRead(BOTAO_DIR) == LOW) && (posRaq < (LCD_DX - TAM_RAQ)))
  {
    LcdSetPixel (posRaq, LCD_DY-1, 0);
    LcdSetPixel (posRaq+TAM_RAQ, LCD_DY-1, 1);
    posRaq++;
  }
}


//=====================================
// Pontos de entrada padrão do Arduino
//=====================================
void setup(void)
{
  LcdInitialise();
  NovoJogo();
}

void loop(void)
{
  MoveRaquete ();
  delay (50);
  MoveRaquete ();
  delay (50);
  MoveRaquete ();
  if (MoveBola ())
  {
    delay (3000);
    NovaBola ();
  }
  if (nBlocos == 0)
  {
    delay (5000);
    NovoJogo ();
  }
}


