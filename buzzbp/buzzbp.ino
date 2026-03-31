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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PHOTORES 36 // pin de la Photorésistance
#define BUZZ 17 // pin du Buzzer
#define BP 23 // pin du Bouton Poussoir
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     16 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_WAIT_FOR_TIMER 7

unsigned long waitFor(int timer, unsigned long period) {
    static unsigned long last_period[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches
    unsigned long current = micros() / period;             // numéro de période
    unsigned long delta   = current - last_period[timer];  // gère le wrap-around
    if (delta) last_period[timer] = current;               // mise à jour si déclenchement
    return delta;                                          // nombre de periode depuis le dernier appel
}

// Définition d'un structure de boite à lettre

enum {EMPTY, FULL};

struct mailbox_t {
  int state;
  int val;
};

//--------- définition de la tache Led

struct ctx_led_t {
  int timer;                                              // n° du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
  bool blink;
};

void init_led(struct ctx_led_t * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  ctx->blink = true;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void step_led(struct ctx_led_t * ctx, struct mailbox_t* mb, volatile struct mailbox_t* mb_stop) {
  if (mb_stop->state == FULL) { // lit la boite si elle est pleine
    ctx->blink = !ctx->blink; // inverser le clignottement
    Serial.println(ctx->blink);
    mb_stop->state = EMPTY;  // vide la boite à lettre
  }
  if (mb->state == FULL) { // lit la boite si elle est pleine
    ctx->period = 1000000/mb->val; // change la période avec la valeur dans la boite
    mb->state = EMPTY;  // vide la boite à lettre
  }
  if(!ctx->blink || !waitFor(ctx->timer, ctx->period)) return;          // sort s'il y a moins d'une période écoulée
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
  int counter;
};

void init_oled(struct ctx_oled_t* ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->counter = 0;

  Wire.begin(4, 15);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextSize(2); // taille de la police
  display.setTextColor(WHITE); // couleur de la police
  display.setCursor(10, 10); // positionnement du curseur
}

void step_oled(struct ctx_oled_t *ctx, struct mailbox_t* mb) {
  if (mb->state == FULL) { // lit la boite s'il est pleine
    ctx->counter = mb->val; // change la valeur à afficher
    mb->state = EMPTY;  // vide la boite à lettre
  }
  if (!(waitFor(ctx->timer,ctx->period))) return;

  display.clearDisplay(); // on vide le buffer (on écrase l'ancienne image)
  display.setCursor(10, 10); // on repositionne le curseur (print le déplace)
  display.print(ctx->counter); // on ajoute le compteur dans le buffer d'affichage
  display.print("%"); // on ajoute le compteur dans le buffer d'affichage
  display.display(); // on affiche le buffer
}

//--------- definition de la tache Lum

struct ctx_lum_t {
  int timer;
  unsigned long period;
  int lum;
};

void init_lum(struct ctx_lum_t* ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->lum = 0;

  // Définition du pin
  pinMode(PHOTORES, INPUT);
}

void step_lum(struct ctx_lum_t* ctx, struct mailbox_t* mbOled, struct mailbox_t* mbLed){
  if (!(waitFor(ctx->timer,ctx->period))) return;
  int lum = analogRead(PHOTORES);

  if (mbOled->state == EMPTY){
    // écrit dans la boîte
    mbOled->val = map(lum, 0, 4095, 0, 100);
    mbOled->state = FULL;
  }

  if (mbLed->state == EMPTY){
    // écrit dans la boîte
    int x = map(lum, 0, 4095, 1, 25);
    mbLed->val = x;
    mbLed->state = FULL;
  }
}

//--------- définition de la tache Bp

struct ctx_bp_t {
  int timer;                                              // n° du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // periode de lecture
  int pin;                                                // numéro de la broche sur laquelle est le Bouton Poussoir
  bool state;                                             // état du bouton (1=enfoncé, 0=relaché)
};

void init_bp(struct ctx_bp_t * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->state = false;
  pinMode(pin, INPUT_PULLUP);
}

void step_bp(struct ctx_led_t * ctx, struct mailbox_t* mb, volatile struct mailbox_t* mb) {
  if (ctx->state) {
    if (!waitFor(ctx->timer, ctx->period * 10)) return; // évite le phénomène de rebond, attend + longtemps
  }
  else if(!waitFor(ctx->timer, ctx->period)) return; // sort s'il y a moins d'une période écoulée

  bool old_state = ctx->state;
  ctx->state = digitalRead(ctx->pin);
  if (ctx->state && !old_state){
    // Front montant
    if (mb->state == 0){
      mb->state = 1;
      mb->val = 1;
    }
  }
}

//--------- définition de la tache Buzz

struct ctx_buzz_t {
  int timer;                                              // n° du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                   // période à laquelle on change l'alimentation du buzzer
  int pin;                                                // numéro de la broche sur laquelle est le Buzzer
  int state;                                              // état du buzzer (1=alimenté, 0=pas alimenté)
};

void init_buzz(struct ctx_bp_t * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->state = 0;
  pinMode(pin, OUTPUT);
}

void step_buzz(struct ctx_led_t * ctx, struct mailbox_t* mb, volatile struct mailbox_t* mb) {
  if (mb->state == 1 && mb->val == 1) ctx->period = ((ctx->period - 50) + 10) % 100 + 50 // change la fréquence
  if(!ctx->state !waitFor(ctx->timer, ctx->period)) return; // sort s'il y a moins d'une période écoulée
  ctx->state = 1 - ctx->state; // alterne entre 1 et 0;
  digitalWrite(ctx->pin, ctx->state);
}

//--------- Déclaration des contexte de tâches

struct ctx_led_t Led1;
struct ctx_mess_t Mess1;
struct ctx_mess_t Mess2;
struct ctx_oled_t Oled;
struct ctx_lum_t Lum;
struct ctx_bp_t Bp;


//--------- Déclaration des boîtes à lettre

struct mailbox_t mbOled = {.state = EMPTY, .val = 0};
struct mailbox_t mbLed = {.state = EMPTY, .val = 0};
struct mailbox_t mbStopLed = {.state = EMPTY}; // volatile because it will be updated in the ISR function
volatile struct mailbox_t mb_stop = {.state = EMPTY};
struct mailbox_t mbBuzz = {.state = EMPTY, .val = 0};

//--------- Déclaration pour les Interruptions

void serialEvent(){
  // Détecte l'évènement au clavier
  if (Serial.available()){
    char inChar = Serial.read();
    if (inChar == 's') mb_stop.state = FULL;
  }
}


//--------- Setup et Loop

void setup() {
  init_led(&Led1, 0, 100000, LED_BUILTIN);               // Led est exécutée toutes les 100ms 
  init_mess(&Mess1, 1, 1000000, "bonjour");              // Mess est exécutée toutes les secondes 
  init_mess(&Mess2, 2, 2000000, "Salut");
  init_oled(&Oled, 3, 1000000);
  init_lum(&Lum, 4, 500000); // lit toutes les 0,5 secondes
  init_bp(&Bp, 5, 10000, BP); // lit le bouton poussoir toutes les 10ms
  init_buzz(&Buzz, 6, 100/2, &mbBuzz); // envoie une fréquence de 10kHz sur le buzzer

  // attach interruption to the ISR function sendMail
  attachInterrupt(digitalPinToInterrupt(2), serialEvent, FALLING); // 2->pin, sendMail->ISR function, FALLING -> trigger on falling edge
}

void loop() {
  step_mess(&Mess1);
  step_mess(&Mess2);

  step_led(&Led1, &mbLed, &mb_stop);
  step_oled(&Oled, &mbOled);
  step_lum(&Lum, &mbOled, &mbLed);
  step_bp(&Bp, &mbBuzz);
  step_buzz(&Buzz, &mbBuzz);
}
