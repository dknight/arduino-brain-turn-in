/**
 * Brain turn-in
 * Version 1.0
 * by Dmitri Smirnov <smirnov.dmitri@gmail.com>
 * 2017-02-20
 * 
 * Simple math game. Check if math expression true or false.
 */

#include <LiquidCrystal.h>

#define CORRECT_BUTTON_PIN       6
#define WRONG_BUTTON_PIN         7
#define CORRECT_LED_PIN          8
#define WRONG_LED_PIN           13
#define INIT_LED_PIN             9

#define LOADING_DELAY_MS      1000
#define ANSWER_DELAY_MS       1500
#define BTN_DEBOUNCE_MS         20

// Power of 10, dont use hight than 4 or use long type.
// How large are numbers
#define NUM_POWER                1

// Initiaize LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Switch variables
byte correctSwitchState = 0;
byte prevCorrectSwitchState = 0;
byte wrongSwitchState = 0;
byte prevWrongSwitchState = 0;

// + 0
// - 1
// * 2
// / 3
byte operation = 0;

// stores math sign +, -, *, /
char math_sign;

// Operation numbers
int n1, n2;

// Stores sthe answer
int answer;

// Is error to trick human?
byte error = 0;

// Is question given or not. Answer or question mode.
byte quest_given = 0;

// Score variables
int wins = 0;
int loses = 0;

void setup() {

  //Serial.begin(9600);
  
  // Init random
  // randomRead makes noise???
  randomSeed(millis());
  
  // Setup pins
  pinMode(CORRECT_BUTTON_PIN, INPUT);
  pinMode(WRONG_BUTTON_PIN, INPUT);
  pinMode(CORRECT_LED_PIN, OUTPUT);
  pinMode(WRONG_LED_PIN, OUTPUT);
  pinMode(INIT_LED_PIN, OUTPUT);
  
  // Initialize ready state and emulate loading :)
  digitalWrite(INIT_LED_PIN, HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  delay(LOADING_DELAY_MS);
  lcd.print("Ready!");
  lcd.setCursor(0, 1);
  lcd.print("Push button");
  digitalWrite(INIT_LED_PIN, LOW);
}

void loop() {

  // Read correct and wrong buttons
  correctSwitchState = digitalRead(CORRECT_BUTTON_PIN);
  wrongSwitchState   = digitalRead(WRONG_BUTTON_PIN);

  // If one of the buttons is pressed then start game.
  if (button_pressed(correctSwitchState, prevCorrectSwitchState) == 1 || button_pressed(wrongSwitchState, prevWrongSwitchState) == 1) {
    
    // If in question mode then generate and print question.
    if (quest_given == 0) {
      generate_question();
    
    // If in answer mode press button to answer the question.  
    } else {
      answer_question();
      
      // Delay. User shouldn't press anything before new question is given.
      delay(ANSWER_DELAY_MS);
      
      // After answer generate new question.
      generate_question();
    }
  }

  // A bit dealy to avoid button debouce
  // Actually should be used without delay, but ok...
  delay(BTN_DEBOUNCE_MS);

  // Swap buttons pin states.
  prevCorrectSwitchState = correctSwitchState;
  prevWrongSwitchState = wrongSwitchState;
}

/**
 * Generates and print the question to LCD.
 */
void generate_question() {

  // Shut down LEDs
  digitalWrite(CORRECT_LED_PIN, LOW);
  digitalWrite(WRONG_LED_PIN, LOW);
    
  // Generate random numbers
  n1 = random(pow(10, NUM_POWER));
  n2 = random(pow(10, NUM_POWER));

  // Check if has error or not.
  // TODO: use more complicated algorithm here.
  error = random(2);

  // Get random operation and generate question.
  operation = random(2);
  switch (operation) {
    case 0:
      answer = n1 + n2;
      math_sign = '+';
      break;
    case 1:
      answer = n1 - n2;
      math_sign = '-';
      break;
    default:
      // do something default;
      break;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(n1);
  lcd.print(math_sign);
  lcd.print(n2);
  lcd.print("=");
  
  // Replace answer with facek one to trick a player.
  if (error != 0) {
    answer = answer + random(-pow(10, NUM_POWER)/2, pow(10, NUM_POWER)/2);
  }
  
  lcd.print(answer);
  lcd.print("?");

  // Set game state to answer mode.
  quest_given = 1;
}

/**
 * Answer the question.
 */
void answer_question() {
  
  // Check wich button is pressed.
  if (correctSwitchState == HIGH) {
    print_answer(error == 0);
  } else if (wrongSwitchState == HIGH) {
    print_answer(error == 1);
  }
  quest_given = 0;
}

/**
 * Print answer correct/wrong, show the score and light the LED.
 */
void print_answer(boolean correct) {
  if (correct) {
    digitalWrite(CORRECT_LED_PIN, HIGH);
    digitalWrite(WRONG_LED_PIN, LOW);
    lcd.clear();
    lcd.print("Correct");
    wins += 1;
    print_score();
  } else {
    digitalWrite(CORRECT_LED_PIN, LOW);
    digitalWrite(WRONG_LED_PIN, HIGH);
    lcd.clear();
    lcd.print("Wrong");
    loses += 1;
    print_score();
  }
}

/**
 * Prints score.
 */
void print_score() {
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(wins);
  lcd.print(":");
  lcd.print(loses);
}

/**
 * Check if button is pressed
 * 
 * @int buttonState
 * @int lastButtonState
 * @returns byte retval
 */
byte button_pressed(int buttonState, int lastButtonState) {
  byte retval = 0;
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      retval = 1;
    } else {
      // if the current state is LOW then the button
      // wend from on to off:
      retval = 0;
    }
  }
  return retval;
}

