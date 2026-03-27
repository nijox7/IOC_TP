// --------------------------------------------------------------------------------------------------------------------
// Multi-tâches cooperatives : solution basique mais efficace :-)
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// unsigned int waitFor(timer, period) 
// Timer pour taches périodiques 
// configuration :
//  - MAX_WAIT_FOR_TIMER : nombre maximum de timers utilisés
// arguments :
//  - timer  : numéro de timer entre 0 et MAX_WAIT_FOR_TIMER-1
//  - period : période souhaitée
// retour :
//  - nombre de périodes écoulées depuis le dernier appel
// --------------------------------------------------------------------------------------------------------------------

// Include et define pour l'écran Oled
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 16 // MODIF 4 -> 16

#define MAX_WAIT_FOR_TIMER 4
unsigned long waitFor(int timer, unsigned long period) {
    static unsigned long last_period[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches
    unsigned long current = micros() / period;             // numéro de période
    unsigned long delta   = current - last_period[timer];  // gère le wrap-around
    if (delta) last_period[timer] = current;               // mise à jour si déclenchement
    return delta;                                          // nombre de periode depuis le dernier appel
}

//--------- définition de la tache Led

struct ctx_led_t {
  int timer;                                              // n° du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
};

void init_led(struct ctx_led_t * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void step_led(struct ctx_led_t * ctx) {
  if (!waitFor(ctx->timer, ctx->period)) return;          // sort s'il y a moins d'une période écoulée
  digitalWrite(ctx->pin,ctx->etat);                       // ecriture
  ctx->etat = 1 - ctx->etat;                              // changement d'état
}

//--------- definition de la tache Mess

struct ctx_mess_t {
  int timer;                                              // numéro de timer utilisé par WaitFor
  unsigned long period;                                   // periode d'affichage
  char mess[20];
};

void init_mess(struct ctx_mess_t* ctx, int timer, unsigned long period, const char * mess) {
  ctx->timer = timer;
  ctx->period = period;
  strcpy(ctx->mess, mess);
  Serial.begin(9600);                                     // initialisation du débit de la liaison série
}

void step_mess(struct ctx_mess_t *ctx) {
  if (!(waitFor(ctx->timer,ctx->period))) return;         // sort s'il y a moins d'une période écoulée
  Serial.println(ctx->mess);                              // affichage du message
}


//--------- definition de la tache Oled

struct ctx_oled_t {
  int timer;
  unsigned long period;
  int count;
};

void init_oled(struct ctx_oled_t* ctx, int timer, unsigned long period){
  ctx->count = 0;
  ctx->period = period;

  // Initialisation de l'écran
  Adafruit_SSD1306 display(OLED_RESET);
  Wire.begin(4, 15); // pins SDA, SCL
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)=
}

void step_oled(struct ctx_oled_t* ctx){
  if (!(waitFor(ctx->timer, ctx->period))) return;
  ctx->count = (ctx->count + 1) % 70;

  // Write on Oled
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(ctx->count);
  display.display();
}

//--------- Déclaration des contexte de tâches

struct ctx_led_t Led1;
struct ctx_mess_t Mess1;
struct ctx_mess_t Mess2;
struct ctx_oled_t Oled;

//--------- Setup et Loop

void setup() {
  init_led(&Led1, 0, 100000, LED_BUILTIN);    // Led est exécutée toutes les 100ms 
  init_mess(&Mess1, 1, 1000000, "bonjour");   // Mess est exécutée toutes les secondes 
  init_mess(&Mess2, 2, 2000000, "Salut");     
  init_oled(&Oled, 3, 1000000);               // rafraichit l'écran toutes les secondes
}

void loop() {
  step_led(&Led1);
  step_mess(&Mess1);
  step_mess(&Mess2);
  step_oled(&Oled);
}
