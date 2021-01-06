const int BUTTON_YELLOW = 2;  // Yellow button input pin
const int BUTTON_WHITE = 3;  // White button input pin
const int BUTTON_GREEN = 4;  // Green button input pin
const int BUTTON_RED = 5;  // Red button input pin
const int LED_YELLOW  = 10; // Yellow LED pin
const int LED_WHITE = 11; // White LED pin
const int LED_GREEN = 12; // Green LED pin
const int LED_RED = 13; // Red LED pin

int sequence[15]={10,12,11,13,10,10,11,13,12,11,11,13,10,12,10};   //Array to set the blink sequence 
short int i,point_count, point, j; 

const int STATE_START_GAME = 0;  // Initial state
const int STATE_PICK_RND_SEQUENCE = 1;  // Pick a random sequence of LEDs
const int STATE_SHOW_RND_SEQUENCE = 2;  // Show the randomly selected sequence of LED flashes
const int STATE_READ_PLAYER_GUESS = 3;  // Read the player's guess
const int STATE_VERIFY_GUESS = 4;  // Check the guess against the random sequence
const int STATE_GUESS_CORRECT = 5;  // Player guessed correctly
const int STATE_GUESS_INCORRECT = 6;  // Player guessed incorrectly

const int MAX_DIFFICULTY_LEVEL = 9;  // Maximum difficulty level (LED flash sequence length)

// Array used to store the generated random sequence
int randomSequence[MAX_DIFFICULTY_LEVEL];

// Array used to store the player's guess
int playerGuess[MAX_DIFFICULTY_LEVEL];
// A counter to record the number of button presses made by the player
int numButtonPresses;

// The current state the game is in
int currentState;
int nextState;

// The difficulty level (1...MAX_DIFFICULTY_LEVEL)
// (Do not set to zero!)
int difficultyLevel;
float realTime;
int long ranDelay = 0;

void setup() {
  Serial.begin(9600);
  difficultyLevel = 1;
  currentState = STATE_START_GAME;
  nextState = currentState;
  numButtonPresses = 0;
  for (int i = BUTTON_YELLOW; i <= BUTTON_RED; i++) {
    pinMode(i, INPUT);
  }
                                                           // Initialise LEDs
  for (int i = LED_YELLOW; i <= LED_RED; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  Serial.println("Press Yellow for Reactions press Red for Memory ");
  while(digitalRead(BUTTON_YELLOW) == LOW && digitalRead(BUTTON_RED) == LOW){;}
  
  if (digitalRead(BUTTON_YELLOW) == HIGH)      {Reactions();}
  else if (digitalRead(BUTTON_RED) == HIGH)    {Memory();}
 // else if (digitalRead(BUTTON_GREEN) == HIGH)  {Serial.print("Grön knapp");}
 // else if (digitalRead(BUTTON_WHITE) == HIGH)  {Serial.print("Vit knapp"); } 
}

void Reactions(){                    //Calling game start routine
  
  for(j=0;j<1;j++)
  {                  //Repeating game sequence twice for a total of 30 points
    Serial.println("press a key");   //next 3 lines are for randomizing the randomgenerator.
    while (Serial.available() == 0);
    randomSeed(micros() +  Serial.read());
  }
 realTime = millis();
    for (int i = 0; i<15; i++)
    {
      int pos = random(15);
      int t = sequence[i];  
      sequence[i] = sequence[pos];
      sequence[pos] = t;
    }

    for (int i = 0; i<15; i++)
    {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(sequence[i]);
    }
    delay(2000);
  Serial.println("GO");
    for(i=0;i<=14;i++)
    {               //Starting game sequence
      realTime = 0;
      ranDelay = random(5000);
      delay(ranDelay);
      //realTime = millis();
      digitalWrite(sequence[i],HIGH); //dioden tänds
      while(digitalRead(BUTTON_YELLOW) == LOW && digitalRead(BUTTON_RED) == LOW && digitalRead(BUTTON_GREEN) == LOW && digitalRead(BUTTON_WHITE) == LOW){
        realTime++;
      }
      //delay(900);
       Serial.print("Your time was: ");
      //realTime = millis()-realTime;
      Serial.print(realTime/1000,2);
      Serial.println(" ms");
      //delay(2000);
      digitalWrite(sequence[i],LOW);
    }
  
  /*int timer = 100; // The higher the number, the slower the timing.

  for (int i = 10; i < 14; i++) 
  {
    digitalWrite(i, HIGH); // turn the pin on:
    delay(timer);
    digitalWrite(i, LOW); // turn the pin off:
  }

  for (int i = 13; i <= 10; i--) 
  {
    digitalWrite(i, HIGH);
    delay(timer);
    digitalWrite(i, LOW);-
  }

  for (int i = 13; i >= 10; i--) 
  { 
    digitalWrite(i, HIGH); // turn the pin on:
    delay(timer);
    digitalWrite(i, LOW); // turn the pin off:
  }
      
  for (int i = 10; i <= 10; i++) 
  {
    digitalWrite(i, HIGH);
    delay(timer);
    digitalWrite(i, LOW);
  }*/

    blockUntilRelease(BUTTON_YELLOW);
}




  /*for (int i = BUTTON_YELLOW; i <= BUTTON_RED; i++) {
    pinMode(i, INPUT);
  }
                                                           // Initialise LEDs
  for (int i = LED_YELLOW; i <= LED_RED; i++) {
    pinMode(i, OUTPUT);
  }*/
  void Memory(){
    while(1){
  switch ( currentState ) {
    case STATE_START_GAME:
      delay(1500);                                      // Give player time to get ready
      if( difficultyLevel == 1 ){
        Serial.println("LEVEL 1");
      }
      nextState = STATE_PICK_RND_SEQUENCE;
      break;
    case STATE_PICK_RND_SEQUENCE:
      generateRndSequence();
      nextState = STATE_SHOW_RND_SEQUENCE;
      break;
    case STATE_SHOW_RND_SEQUENCE:
      showRndSequence();                                // Display the rnd sequence to the player
      nextState = STATE_READ_PLAYER_GUESS;
      break;
    case STATE_READ_PLAYER_GUESS:
      readPlayerGuess();                               // Poll buttons and record each button press                                               
      if( numButtonPresses >= difficultyLevel ){       // If all inputs have been made then verify the guess
        numButtonPresses = 0;
        nextState = STATE_VERIFY_GUESS;
      }
      break;
    case STATE_VERIFY_GUESS:                         // Check player's guess against the generated random sequence
      if ( verifyGuess() ) {
        nextState = STATE_GUESS_CORRECT;
      }else{
        nextState = STATE_GUESS_INCORRECT;
      }
      break;
    case STATE_GUESS_CORRECT:
      Serial.println("YOUR GUESS WAS CORRECT");   // Player was right. Increase difficulty level and goto start game
      difficultyLevel++;
      if ( difficultyLevel > MAX_DIFFICULTY_LEVEL )
        difficultyLevel = MAX_DIFFICULTY_LEVEL;
      nextState = STATE_START_GAME;
      Serial.print("LEVEL ");
      Serial.println(difficultyLevel);
      break;
    case STATE_GUESS_INCORRECT:                                             // Player was wrong. Sound buzzer, show correct sequence, set difficulty level to 1 and re-start game
      //soundBuzzer();
      int timer = 100; // The higher the number, the slower the timing.
      for (int i = LED_YELLOW; i < LED_RED + 1; i++) { 
        digitalWrite(i, HIGH); // turn the pin on:
        delay(timer);
        digitalWrite(i, LOW); // turn the pin off:
      }

      for (int i = LED_RED; i <= LED_YELLOW; i--) {
        digitalWrite(i, HIGH);
        delay(timer);
        digitalWrite(i, LOW);
      }

      for (int i = LED_RED; i >= LED_YELLOW; i--) { 
        digitalWrite(i, HIGH); // turn the pin on:
        delay(timer);
        digitalWrite(i, LOW); // turn the pin off:
      }
      
      for (int i = LED_YELLOW; i <= LED_YELLOW; i++) {
        digitalWrite(i, HIGH);
        delay(timer);
        digitalWrite(i, LOW);
      }
      Serial.println("\nYOUR GUESS WAS INCORRECT, GAME OVER!");
      //Serial.println("Do you want to start over? Press GREEN or RED (yes/no)\n");
      //if(digitalRead(BUTTON_GREEN) == HIGH){
      Serial.println("Back to Level 1...\n");
      difficultyLevel = 1;
      nextState = STATE_START_GAME;
      break;
  }
  currentState = nextState;
}
}



void generateRndSequence() {                      // Generate a random sequence of LED pin numbers
  for (int i = 0; i < difficultyLevel; i++) {
    randomSequence[i] = rndLEDPin();
  }
}

void showRndSequence() {                          // Show random sequence
  for (int i = 0; i < difficultyLevel; i++) {
    flashLED(randomSequence[i], true);
  }
}


void readPlayerGuess() {                         // Read a button press representing a guess from player
  if ( digitalRead(BUTTON_YELLOW) == HIGH ) {
    playerGuess[numButtonPresses] = LED_YELLOW;
    numButtonPresses++;
    flashLED(LED_YELLOW, true);
    Serial.println("YELLOW");
    blockUntilRelease(BUTTON_YELLOW);
  } else if ( digitalRead(BUTTON_WHITE) == HIGH ) {
    playerGuess[numButtonPresses] = LED_WHITE;
    numButtonPresses++;
    flashLED(LED_WHITE, true);
    Serial.println("WHITE");
    blockUntilRelease(BUTTON_WHITE);
  } else if ( digitalRead(BUTTON_GREEN) == HIGH ) {
    playerGuess[numButtonPresses] = LED_GREEN;
    numButtonPresses++;
    flashLED(LED_GREEN, true);
    Serial.println("GREEN");
    blockUntilRelease(BUTTON_GREEN);
  } else if ( digitalRead(BUTTON_RED) == HIGH ) {
    playerGuess[numButtonPresses] = LED_RED;
    numButtonPresses++;
    flashLED(LED_RED, true);
    Serial.println("RED");
    blockUntilRelease(BUTTON_RED);
  }
}

void blockUntilRelease(int buttonNumber) {
while ( digitalRead(buttonNumber) == HIGH );
}

                                                    // Compare the guess with the random sequence and return true if identical
bool verifyGuess(){
  bool identical = true;
  for (int i = 0; i < difficultyLevel; i++){
    if ( playerGuess[i] != randomSequence[i] ){
      identical = false;
      break;
    }
  }
  return identical;
}
  

void flashLED(int ledPinNum, bool playSound){       // Flash the LED on the given pin
  digitalWrite(ledPinNum, HIGH);
  delay(1000);
  digitalWrite(ledPinNum, LOW);
  delay(500);
}

int rndLEDPin() {                                  // Get a random LED pin number
  return random(LED_YELLOW, LED_RED + 1);
}
