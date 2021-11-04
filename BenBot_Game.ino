/*
Colour reference against their random numbers
BLUE = 1
GREEN = 2
YELLOW = 3
RED = 4

Things to add maybe by Ben
-An amount of chances. e.g. 3 chances to get the sequence correct. A chance gets taken off you
if you press the wrong colour button during the reaction game.
-Time penalty for pressing an incorrect button during the reaction game. Currently you get -1 to score.
-Another game where it sets a colour and times how long it takes for you to press the button. x5
and take the average speed reading
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Set display object.

const int blueLEDnumber = 1; //NUMBER FOR COMPARING AGAINST THE RANDOM NUMBER
const int blueLEDpin = 2; //BLUE LED OUTPUT
const int blueSWITCHpin = 6; //BLUE SWITCH INPUT
const int greenLEDnumber = 2; //NUMBER FOR COMPARING AGAINST THE RANDOM NUMBER
const int greenLEDpin = 4; //GREEN LED OUTPUT
const int greenSWITCHpin = 11; //GREEN SWITCH INPUT
const int yellowLEDnumber = 3; //NUMBER FOR COMPARING AGAINST THE RANDOM NUMBER
const int yellowLEDpin = 5; //YELLOW LED OUTPUT
const int yellowSWITCHpin = 12; //YELLOW SWITCH INPUT
const int redLEDnumber = 4; //NUMBER FOR COMPARING AGAINST THE RANDOM NUMBER
const int redLEDpin = 3; //RED LED OUTPUT
const int redSWITCHpin = 10; //RED SWITCH INPUT
int sequenceCount = 0;
int guessCount = 0;
bool startGameText = false;
bool startGameLoop = false;
bool reactionGameLoop = false;
bool addColour = false;
bool guessColourSequ = false; //Used guessColour in the reaction game. Had to rename to this.
bool evaluateSequence = false;
bool plus1Game = false;
bool randomSequenceGame = false;
bool firstTextOn = true;
int actualSequence[100];
int guessSequence[100];
int cycleCount;
unsigned long cycleTime = 0;

//Reaction game variables.
bool reactionGame = false;
bool setColour = false;
bool guessColour = false;
bool blueReset = false;
bool checkResetBlue = false;
bool greenReset = false;
bool checkResetGreen = false;
bool yellowReset = false;
bool checkResetYellow = false;
bool redReset = false;
bool checkResetRed = false;
unsigned long reactionTimer = 0;
unsigned long reactionSecondCount = 60;
unsigned long setColourDelay = 0;
int reactionRandNumber = 0;
int reactionGameScore = 0;

void setup() {
  Serial.begin(9600);
  
  //Set the inputs and outputs. LED's are outputs and the button switches are inputs.
  pinMode(blueLEDpin, OUTPUT);
  pinMode(blueSWITCHpin, INPUT);
  pinMode(greenLEDpin, OUTPUT);
  pinMode(greenSWITCHpin, INPUT);
  pinMode(yellowLEDpin, OUTPUT);
  pinMode(yellowSWITCHpin, INPUT);
  pinMode(redLEDpin, OUTPUT);
  pinMode(redSWITCHpin, INPUT);
  
  //Set all the LED outputs to be low when it first starts up (lights off).
  digitalWrite(blueLEDpin, LOW);
  digitalWrite(greenLEDpin, LOW);
  digitalWrite(yellowLEDpin, LOW);
  digitalWrite(redLEDpin, LOW);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  //Clear the display buffer and display ben bot text for 5 seconds.
  display.clearDisplay();
  
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,10);
  display.println("Ben Bot");
  display.display();
  
  randomSeed(analogRead(0)); //Shuffle random numbers.
  //5 second delay. Delay is measured in milliseconds.
  delay(5000);
  
  //Set the reference time for cycling through the different menus in startGameText.
  cycleTime = millis();
}

void loop() {
  //Display start game text
  //Using a millis comparison to rotate game text.
  //Every 5 seconds it displays next game text.
  //Millis is used instead of delay as the button presses are being checked.
  if (!startGameText){
    if (millis() - cycleTime >= 2500 || firstTextOn){
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      if (cycleCount == 0){
      //First game type text
      display.setCursor(41,1);
      display.println(F("BLUE"));
      display.setCursor(30,17);
      display.println(F("PLUS 1"));
      display.display();
      firstTextOn = false;
      }
      else if (cycleCount == 1){
        //Second game type text
        Serial.println("second one");
        display.setCursor(35,1);
        display.println(F("GREEN"));
        display.setCursor(5,17);
        display.println(F("RANDOM SEQ"));
        display.display();
      }
      else if (cycleCount == 2){
        //Third game type text
        display.setCursor(29,1);
        display.println(F("YELLOW"));
        display.setCursor(17,17);
        display.println(F("REACTION"));
        display.display();
      }
      //Increments game type text. Doesnt allow it to get bigger than 2
      cycleCount++;
      if(cycleCount > 2){
        cycleCount = 0; //Loops back round to the first game type text
      }
      //Reset reference cycle time for the above menus.
      cycleTime = millis();
    }
  }
  
  //Check button presses to start different game types
  //Check blue button
  if (digitalRead(blueSWITCHpin)==HIGH){
    delay(50);//Any time there is a delay or millis comparison after a button press - its for something called switch debounce.
    if(digitalRead(blueSWITCHpin)==HIGH){
      //Start plus1 game
      startGameLoop = true; //Start While startGameLoop
      addColour = true; //Set flag to add first colour in the startGameLoop
      plus1Game = true; //Start the plus1 game
      startGameText = true;
      //Turn blue LED on for 1 second.
      digitalWrite(blueLEDpin, HIGH);
      delay(1000);
      digitalWrite(blueLEDpin, LOW);
    }
  }
  //Check green button
  else if (digitalRead(greenSWITCHpin)==HIGH){
    delay(50);
    if(digitalRead(greenSWITCHpin)==HIGH){
      //Start random sequence game
      startGameLoop = true; //Start While startGameLoop
      addColour = true; //Set flag to add first colour in the startGameLoop
      randomSequenceGame = true; //Start the random sequence game
      startGameText = true;
      digitalWrite(greenLEDpin, HIGH);
      delay(1000);
      digitalWrite(greenLEDpin, LOW);
    }
  }
  //Check yellow button
  else if (digitalRead(yellowSWITCHpin)==HIGH){
    delay(50);
    if(digitalRead(yellowSWITCHpin)==HIGH){
      //Start random sequence game
      reactionGameLoop = true;
      setColour = true;
      guessColour = false;
      reactionTimer = millis();
      setColourDelay = millis();
      reactionSecondCount = 60;
      reactionGameScore = 0;
      //Display
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      displayText();
      digitalWrite(yellowLEDpin, HIGH);
      delay(1000);
      digitalWrite(yellowLEDpin, LOW);
      delay(1000);
    }
  }

  /*
  The base loop (void Loop) controls the menu display and checks for button presses to choose a game.
  The startGameLoop is for the Plus1 and Random Sequence game.
  The reactionGameLoop is for the Reaction game.
  */
  while(startGameLoop){
    //Add colour to the array and display full sequence.
    if (addColour){
      ++sequenceCount; //This variable keeps track of the sequence number and increments on a correct sequence.
      /*
      Display what level a person is on. This is done on the next two steps because
      the screen shifts on either double or single digits to keep it in the middle.
      */
      display.clearDisplay();
      if(sequenceCount < 10){
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20,9);
        display.println(F("LEVEL"));
        display.setTextSize(3);
        display.setCursor(95,5);
        display.println(String(sequenceCount));
        display.display();
      }
      else{
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10,9);
        display.println(F("LEVEL"));
        display.setTextSize(3);
        display.setCursor(85,5);
        display.println(String(sequenceCount));
        display.display();
      }
      delay(2000);
      if (plus1Game){
        int randNumber = random(1,5);
        actualSequence[sequenceCount-1] = randNumber; //Adding random colour to end of array. Previous colours are the same.
      }
      else if (randomSequenceGame){
        for(int i = 0; i <= sequenceCount-1; i++){
          actualSequence[i] = random(1,5); //Add a random colour to each index of the array.
        }
      }
      //Display sequence in a for loop. For loops are a verry efficient way of doing a task multiple times in a loop.
      for(int i = 0; i <= sequenceCount-1; i++){
        int sequenceColour = actualSequence[i];
        /*
        Display the whole sequence every time in the random sequence game but only
        the last colour added to the sequence for the plus 1 game.
        */
        if (randomSequenceGame || plus1Game && i == sequenceCount -1){
          if (sequenceColour == blueLEDnumber){
            digitalWrite(blueLEDpin, HIGH);
            delay(1500);
            digitalWrite(blueLEDpin, LOW);
            delay(1000);
          }
          else if(sequenceColour == greenLEDnumber){
            digitalWrite(greenLEDpin, HIGH);
            delay(1500);
            digitalWrite(greenLEDpin, LOW);
            delay(1000);
          }
          else if(sequenceColour == yellowLEDnumber){
            digitalWrite(yellowLEDpin, HIGH);
            delay(1500);
            digitalWrite(yellowLEDpin, LOW);
            delay(1000);
          }
          else if(sequenceColour == redLEDnumber){
            digitalWrite(redLEDpin, HIGH);
            delay(1500);
            digitalWrite(redLEDpin, LOW);
            delay(1000);
          }
        }
        guessCount = 0; //Reset guess count ready to start the sequence from the beggining.
      }
      
      addColour = false;
      guessColourSequ = true;
      
      //Display first amount of guesses left. Again offset display depending on digits to stay in the middle.
      if(sequenceCount-guessCount < 10){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(60,9);
        display.println(F("LEFT"));
        display.setTextSize(3);
        display.setCursor(22,5);
        display.println(sequenceCount-guessCount);
        display.display();
      }
      else{
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(66,9);
        display.println(F("LEFT"));
        display.setTextSize(3);
        display.setCursor(16,5);
        display.println(sequenceCount-guessCount);
        display.display();
      }
    }
    
    if (guessColourSequ){
      //Take correct number of guesses
      //Then evaluate the guesses against actual
      //CHECKING BUTTON PRESS
      //BLUE
      if (digitalRead(blueSWITCHpin)==HIGH){
        delay(50);
        if(digitalRead(blueSWITCHpin)==HIGH){
          //Set the LED to on when button pressed
          digitalWrite(blueLEDpin, HIGH);
          //ADD GUESS INTO SEQUENCE ARRAY
          guessSequence[guessCount] = blueLEDnumber;
          //INCREMENT GUESS COUNT
          guessCount++;
          //SHOW ON DISPLAY HOW MANY GUESSES LEFT
          if(sequenceCount-guessCount < 10){
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(60,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(22,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          else{
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(66,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(16,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          delay(1000);
          digitalWrite(blueLEDpin, LOW);
        }
      }
      //GREEN
      else if (digitalRead(greenSWITCHpin)==HIGH){
        delay(50);
        if(digitalRead(greenSWITCHpin)==HIGH){
          digitalWrite(greenLEDpin, HIGH);
          guessSequence[guessCount] = greenLEDnumber;
          guessCount++;
          if(sequenceCount-guessCount < 10){
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(60,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(22,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          else{
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(66,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(16,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          delay(1000);
          digitalWrite(greenLEDpin, LOW);
        }
      }
      //YELLOW
      if (digitalRead(yellowSWITCHpin)==HIGH){
        delay(50);
        if(digitalRead(yellowSWITCHpin)==HIGH){
          //Set the LED to on when button pressed
          digitalWrite(yellowLEDpin, HIGH);
          //ADD GUESS INTO SEQUENCE ARRAY
          guessSequence[guessCount] = yellowLEDnumber;
          //INCREMENT GUESS COUNT
          guessCount++;
          //SHOW ON DISPLAY HOW MANY GUESSES LEFT
          if(sequenceCount-guessCount < 10){
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(60,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(22,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          else{
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(66,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(16,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          delay(1000);
          digitalWrite(yellowLEDpin, LOW);
        }
      }
      //RED
      if (digitalRead(redSWITCHpin)==HIGH){
        delay(50);
        if(digitalRead(redSWITCHpin)==HIGH){
          //Set the LED to on when button pressed
          digitalWrite(redLEDpin, HIGH);
          //ADD GUESS INTO SEQUENCE ARRAY
          guessSequence[guessCount] = redLEDnumber;
          //INCREMENT GUESS COUNT
          guessCount++;
          //SHOW ON DISPLAY HOW MANY GUESSES LEFT
          if(sequenceCount-guessCount < 10){
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(60,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(22,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          else{
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(66,9);
            display.println(F("LEFT"));
            display.setTextSize(3);
            display.setCursor(16,5);
            display.println(sequenceCount-guessCount);
            display.display();
          }
          delay(1000);
          digitalWrite(redLEDpin, LOW);
        }
      }
      //If all the gueses have been made move on to evaluate the sequence.
      else if (guessCount == sequenceCount){
        //Evaluate sequence display text
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(6,9);             // Start at top-left corner
        display.println(F("CHECKING.."));
        display.display();
        guessColourSequ = false;
        evaluateSequence = true;
        delay(2000);
      }
    }
    /*
    Checking the two arrays actualSequence and guessSequence against each other to see if the
    player got the correct sequence. Using a for loop to do this.
    */
    if (evaluateSequence){
      bool isSequenceCorrect = true;
      for (int i = 0; i <= sequenceCount-1; i++){
        if(actualSequence[i] != guessSequence[i]){ //If the actualSequence array does not equal the guessSequence then they got it wrong.
          isSequenceCorrect = false;
        }
      }
      if(isSequenceCorrect){
        addColour = true;
        //Got sequence correct text :)
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(19,9);             // Start at top-left corner
        display.println(F("CORRECT!"));
        display.display();
        delay(2000);
      }
      else if(!isSequenceCorrect){
        //Reset the game if the sequence is incorrect
        //Got sequence wrong text :'(
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(47,9);             // Start at top-left corner
        display.println(F("WAH"));
        display.display();
        delay(1000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(35,9);
        display.println(F("WAHHH"));
        display.display();
        delay(1000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(23,9);
        display.println(F("WAHHHHH"));
        display.display();
        delay(1000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(11,9);
        display.println(F("INCORRECT"));
        display.display();
        delay(1000);
        if(sequenceCount < 10){
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(20,9);
          display.println(F("SCORE"));
          display.setTextSize(3);
          display.setCursor(95,5);
          display.println(String(sequenceCount-1));
          display.display();
        }
        else{
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(10,9);
          display.println(F("SCORE"));
          display.setTextSize(3);
          display.setCursor(85,5);
          display.println(String(sequenceCount-1));
          display.display();
        }
        delay(5000);
        
        //Reset bools required to start the game again
        startGameText = false;
        startGameLoop = false;
        plus1Game = false;
        randomSequenceGame = false;
        
        //Reset counts to start game again
        sequenceCount = 0;
        guessCount = 0;
        
      }
      evaluateSequence = false;
    }
  }
  //Reaction game loop.
  while (reactionGameLoop){
    /*
    This is a second timer. It takes the millis (which is a timer from when the program started)
    compares it to a snapshop reactionTimer and checks if its been a second. If it has then
    it decremets the second timer by 1 from 60.
    */
    if(millis()-reactionTimer >= 1000){
      reactionTimer = millis();
      reactionSecondCount--;
      displayText();
      //GAME END - When the timer reaches 0 seconds.
      if(reactionSecondCount <= 0){
        reactionGameLoop = false;
        startGameText = false;
        setColour = false;
        digitalWrite(blueLEDpin, LOW);
        digitalWrite(greenLEDpin, LOW);
        digitalWrite(redLEDpin, LOW);
        digitalWrite(yellowLEDpin, LOW);
        delay(5000);
      }
    }
    
    //Light up a random button > Check button presses > compare button press against random lit up button
    //Sets a colour after a little delay so there is actually a pause inbetween guessing and setting.
    //Can't use the delay function in this game because the second counter always needs to run.
    if(setColour && millis()-setColourDelay>=500){
      //Setting the colour.
      reactionRandNumber = random(1,5);
      if(reactionRandNumber == blueLEDnumber){
        digitalWrite(blueLEDpin, HIGH);
        setColour = false;
        guessColour = true;
      }
      else if(reactionRandNumber == greenLEDnumber){
        digitalWrite(greenLEDpin, HIGH);
        setColour = false;
        guessColour = true;
      }
      else if(reactionRandNumber == yellowLEDnumber){
        digitalWrite(yellowLEDpin, HIGH);
        setColour = false;
        guessColour = true;
      }
      else if(reactionRandNumber == redLEDnumber){
        digitalWrite(redLEDpin, HIGH);
        setColour = false;
        guessColour = true;
      }
      //Set the flags true to check that buttons have been released.
      checkResetBlue = true;
      checkResetGreen = true;
      checkResetYellow = true;
      checkResetRed = true;
    }
    if(guessColour){
      //This check for a 'reset' of the buttons to ensure they have been let go of in between presses.
      //Otherwise multiple presses will be detected.
      //###Could really do with some debounce on this. Although there is debounce on the button presses.
      if(checkResetBlue){
        if(!digitalRead(blueSWITCHpin)){
          blueReset = true;
          checkResetBlue = false;
        }
      }
      if(checkResetGreen){
        if(!digitalRead(greenSWITCHpin)){
          greenReset = true;
          checkResetGreen = false;
        }
      }
      if(checkResetYellow){
        if(!digitalRead(yellowSWITCHpin)){
          yellowReset = true;
          checkResetYellow = false;
        }
      }
      if(checkResetRed){
        if(!digitalRead(redSWITCHpin)){
          redReset = true;
          checkResetRed = false;
        }
      }
      //Reading the switches so long as they have been 'reset' (let go of).
      //BLUE
      if(digitalRead(blueSWITCHpin) && blueReset){
        delay(50);
        if(digitalRead(blueSWITCHpin)){
          //If the button is pressed and its correct then increment the count.
          if(blueLEDnumber == reactionRandNumber){
            //Increase game score.
            reactionGameScore++;
            //Update screen with score
            displayText();
            //Stop looking for button presses and set the next colour.
            guessColour = false;
            setColour = true;
            digitalWrite(blueLEDpin, LOW);
            //Give it enough time to actually show the LED as off.
            setColourDelay = millis();
          }
          else{
            //If the button is pressed and its incorrect then decrement the count.
            /*Turn off lights that werent pressed. We know the one that was pressed was wrong
            and was therefore already off. This really isnt the most effiecent way of doing this.*/
            digitalWrite(greenLEDpin, LOW);
            digitalWrite(yellowLEDpin, LOW);
            digitalWrite(redLEDpin, LOW);
            //Decrement game score
            reactionGameScore--;
            displayText();
            guessColour = false;
            setColour = true;
            setColourDelay = millis();
          }
          //Makes sure the code runs to check for the buttons being let go of in the next loop around.
          blueReset = false;
          checkResetBlue = true;
        }
      }
      //GREEN
      if(digitalRead(greenSWITCHpin) && greenReset){
        delay(50);
        if(digitalRead(greenSWITCHpin)){
          //If the button is pressed and its correct then increment the count.
          if(greenLEDnumber == reactionRandNumber){
            //Increase game score.
            reactionGameScore++;
            //Update screen with score
            displayText();
            //Stop looking for button presses and set the next colour.
            guessColour = false;
            setColour = true;
            digitalWrite(greenLEDpin, LOW);
            //Give it enough time to actually show the LED as off.
            setColourDelay = millis();
          }
          else{
            //If the button is pressed and its incorrect then decrement the count.
            /*Turn off lights that werent pressed. We know the one that was pressed was wrong
            and was therefore already off. This really isnt the most effiecent way of doing this.*/
            digitalWrite(blueLEDpin, LOW);
            digitalWrite(yellowLEDpin, LOW);
            digitalWrite(redLEDpin, LOW);
            //Decrement game score
            reactionGameScore--;
            displayText();
            guessColour = false;
            setColour = true;
            setColourDelay = millis();
          }
          //Makes sure the code runs to check for the buttons being let go of in the next loop around.
          greenReset = false;
          checkResetGreen = true;
        }
      }
      //YELLOW
      if(digitalRead(yellowSWITCHpin) && yellowReset){
        delay(50);
        if(digitalRead(yellowSWITCHpin)){
          //If the button is pressed and its correct then increment the count.
          if(yellowLEDnumber == reactionRandNumber){
            //Increase game score.
            reactionGameScore++;
            //Update screen with score
            displayText();
            //Stop looking for button presses and set the next colour.
            guessColour = false;
            setColour = true;
            digitalWrite(yellowLEDpin, LOW);
            //Give it enough time to actually show the LED as off.
            setColourDelay = millis();
          }
          else{
            //If the button is pressed and its incorrect then decrement the count.
            /*Turn off lights that werent pressed. We know the one that was pressed was wrong
            and was therefore already off. This really isnt the most effiecent way of doing this.*/
            digitalWrite(blueLEDpin, LOW);
            digitalWrite(greenLEDpin, LOW);
            digitalWrite(redLEDpin, LOW);
            //Decrement game score
            reactionGameScore--;
            displayText();
            guessColour = false;
            setColour = true;
            setColourDelay = millis();
          }
          //Makes sure the code runs to check for the buttons being let go of in the next loop around.
          yellowReset = false;
          checkResetYellow = true;
        }
      }
      //RED
      if(digitalRead(redSWITCHpin) && redReset){
        delay(50);
        if(digitalRead(redSWITCHpin)){
          //If the button is pressed and its correct then increment the count.
          if(redLEDnumber == reactionRandNumber){
            //Increase game score.
            reactionGameScore++;
            //Update screen with score
            displayText();
            //Stop looking for button presses and set the next colour.
            guessColour = false;
            setColour = true;
            digitalWrite(redLEDpin, LOW);
            //Give it enough time to actually show the LED as off.
            setColourDelay = millis();
          }
          else{
            //If the button is pressed and its incorrect then decrement the count.
            /*Turn off lights that werent pressed. We know the one that was pressed was wrong
            and was therefore already off. This really isnt the most effiecent way of doing this.*/
            digitalWrite(blueLEDpin, LOW);
            digitalWrite(yellowLEDpin, LOW);
            digitalWrite(greenLEDpin, LOW);
            //Decrement game score
            reactionGameScore--;
            displayText();
            guessColour = false;
            setColour = true;
            setColourDelay = millis();
          }
          //Makes sure the code runs to check for the buttons being let go of in the next loop around.
          redReset = false;
          checkResetRed = true;
        }
      }
    }
  }
}

//Display text function for reaction game. Handles single/double/triple digits.
void displayText(){
  //Display
  display.clearDisplay();
  //Time Text - Shifting to be in the middle for single or double digits.
  display.setCursor(2,1);
  display.println("TIME");
  if(reactionSecondCount >= 10){
    display.setCursor(14,17);
  }
  else{
    display.setCursor(20,17);
  }
  display.println(reactionSecondCount);
  //Score Text - Shifting to be in the middle for single, double or triple digits.
  display.setCursor(68,1);
  display.println("SCORE");
  if(reactionGameScore >= 10 && reactionGameScore < 100){
    display.setCursor(86,17);
  }
  else if(reactionGameScore >= 100){
    display.setCursor(80,17);
  }
  else{
    display.setCursor(92,17);
  }
  display.println(reactionGameScore);
  //Display everything set above
  display.display();
}
