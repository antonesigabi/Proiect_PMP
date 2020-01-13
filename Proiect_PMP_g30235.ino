#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int b1;

//Variabile ajutatoare pentru claritatea codului
#define pin_RW 10
#define pin_Potentiometru 12


#define dimensiuneRand 16
#define pozitie_om_orizontal 1
#define pozitie_om_run1 1
#define pozitie_om_run2 2
//avem mai multe pozitii pe care omuletul le va avea pe parcursul
//sariturii. Astfel, pe traiectoria sa se vor afla 8 pozitii pana la
//executarea unui salt complet
#define jump_1 3
#define jump_2 4
#define jump_3 5
#define jump_4 6
#define jump_5 7
#define jump_6 8
#define jump_7 9
#define jump_8 10




static char randSus[dimensiuneRand + 1]; 
static char randJos[dimensiuneRand + 1];
static bool started = false;

void graphic_units(){

  static byte graphic_unit[]={
    //pozitie fuga 1
    B01100,
    B01100,
    B00000,
    B01110,
    B11100,
    B01100,
    B11010,
    B10011,
    //pozitie fuga 2
    B01100,
    B01100,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B01110,
    //sari
    B01100,
    B01100,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    //cazi
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B00000,
    B00000,
    B00000,
  };
  for(int i=0; i<7; i++){
    lcd.createChar(i+1, &graphic_unit[i * 8]);
  }

  //initializam vectorii de mapa cu spatii goale
  for(int i = 0; i < dimensiuneRand; i++){  
       randSus[i] = ' ';
       randJos[i] = ' ';
  }
}


//rutina atasata intreruperii
void buttonPush(){
     started = true;
}


bool cazuriJoc(byte poz, char* randSus, char* randJos, int score){
  
  char pozSus = randSus[1];
  char pozJos = randJos[1];
  char auxpozSus, auxpozJos;
  switch(poz){
     case 0: //empty
      auxpozSus = ' ';
      auxpozJos = ' ';
      break;
     case pozitie_om_run1:
      auxpozSus = ' ';
      auxpozJos = 1; //fugi poz 1
      break;
    case pozitie_om_run2:
      auxpozSus = ' ';
      auxpozJos = 2; //fugi poz 2
      break;
    case jump_1:
    case jump_8:
      auxpozSus = ' ';
      auxpozJos = 3; //jump
      break;
    case jump_2:
    case jump_7:
      auxpozSus = ' ';
      auxpozJos = 4; //partea de jos a omuletului cand sare
      break;
    case jump_3:
    case jump_4:
    case jump_5:
    case jump_6: 
      auxpozSus = 3; //jump
      auxpozJos = ' ';
      break;
  }   
  
 bool crash = false;
  
  if(auxpozSus != ' '){
     randSus[1] = auxpozSus;
    if(pozSus == ' '){
        crash = false;
    }
    else{
        crash = true; 
    }
  }
  
  if(auxpozJos != ' '){
     randJos[1] = auxpozJos;
      if(pozJos == ' '){
        crash |= false;
    }
    else{
        crash |= true; 
    }
    
  }
  
  byte cifre_scor = (score > 9999) ? 5 : (score > 999) ? 4 : (score > 99) ? 3 : (score > 9) ? 2 : 1;
  
  randSus[dimensiuneRand] = '\0';
  randJos[dimensiuneRand] = '\0';
  char aux = randSus[16-cifre_scor];
  randSus[16-cifre_scor] = '\0';
  lcd.setCursor(0,0);
  lcd.print(pozSus);
  randSus[16-cifre_scor] = aux;
  lcd.setCursor(0,1);
  lcd.print(pozJos);
  
  lcd.setCursor(16 - cifre_scor, 0);
  lcd.print(score);
  
  randSus[1] = auxpozSus;
  randJos[1] = auxpozJos;
  
  return crash;
}


void generareObstacole(char *traseu, byte obstacolNou){
  for(int i = 0; i < dimensiuneRand; i++){
     char currentPoz = traseu[i];
     char urmator;
    if(i == dimensiuneRand - 1){
        urmator = obstacolNou;
    }
    else{
        urmator = traseu[i+1];
    }
    
    switch(currentPoz){
        case ' ':
          if(urmator == 5){
            traseu[i] = 6;  
          }
          else{
            traseu[i] = 0;
          }
          break;
        case 5:
          if(urmator == ' '){
               traseu[i] = 7;
          } 
          else{
               traseu[i] = 5;
          }
            break;
        case 6:
          traseu[i] = 5;
          break;
        case 7:
          traseu[i] = ' ';
          break;
    }
    
  }

}

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_RW, OUTPUT);
  digitalWrite(pin_RW, LOW);
  pinMode(pin_Potentiometru, OUTPUT);
  digitalWrite(pin_Potentiometru, LOW);
  
  pinMode(4, INPUT_PULLUP);

  attachInterrupt(0, buttonPush, FALLING);
  
  graphic_units();
  
  lcd.begin(16,2);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  static byte pozitie_om = pozitie_om_run1;
  static bool playing = false;
  static bool preGame = false;
  static int scor = 0;
  static byte obstacolNou = 0;
  static byte lungimeObstacolUrmator = 1;
  
  
  if(!playing){
    cazuriJoc((preGame) ? 0 : pozitie_om, randSus, randJos, scor);
    if(preGame){
        lcd.setCursor(0,0);
        lcd.print("PRESS START");
    }
     delay(250);
     preGame = !preGame;
     if(started){
     graphic_units();
     pozitie_om = pozitie_om_run1;
     playing = true;
     started = false;
     scor = 0;
    }
    return;
  }
  
  //shiftam traseul la stanga pentru a crea iluzia cursivitatii
  if(obstacolNou == 1){
    generareObstacole(randJos, 5);  
  }
  else{
    generareObstacole(randJos, ' ');
  }
  
  if(obstacolNou == 2){
    generareObstacole(randSus, 5); 
  }
  else{
    generareObstacole(randSus, ' ');
  }
  
  //generam obstacole random care sa fie shiftate dinspre dreapta la stanga
  if(--lungimeObstacolUrmator == 0){
    if(obstacolNou == 0){
      if(random(3) == 0){
         obstacolNou = 1;
      }
      else{
          obstacolNou = 2;
      }
      lungimeObstacolUrmator = 2 + random(5);
    }
    else{
      obstacolNou = 0;
      lungimeObstacolUrmator = 5 + random(5);
    }
  }
  
  if(started){
     if(pozitie_om <= 2) pozitie_om = 3; //jump 
     started = false;
  }
  
  if(cazuriJoc(pozitie_om, randSus, randJos, scor)){
    playing = false; //coliziune, ceau joc
  }
  else{
    if(pozitie_om == 2 || pozitie_om == jump_8){
       pozitie_om = 1; 
    }
    else if( (pozitie_om >= jump_3 && pozitie_om <= jump_5) && randJos[1] != ' '){
         pozitie_om = jump_8;
    }
    else if (pozitie_om >= 9 && randJos[1] == ' '){
       pozitie_om = jump_5;
    }
    else{
        pozitie_om++;
    }
    scor++;
    
 }
delay(100);
}
