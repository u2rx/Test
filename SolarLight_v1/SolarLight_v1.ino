/*
 * SOLAR/BATTERY POWERED LIGHT SYSTEM
 */

#define DEBUG 1

#ifdef DEBUG
 #define DEBUG_PRINT  Serial.print("Usolar: "); Serial.print (Usolar); Serial.print(" UBattery: "); Serial.print (Ubat); \
                      Serial.print(" DAY: "); Serial.print (dayTime); Serial.print(" Battery LOW: "); Serial.print (batLow); \
                      Serial.print ("  LEDON time: "); Serial.println(ledONtimer);                    
#else
 #define DEBUG_PRINT
#endif


//HW define
const int ledPin =  12;                     // the number of the LED pin
const int solPin = 1;                       //Analogue pins
const int batPin = 2;

//Variables define
const int ledONmaxTime = 20;               // Lighting time in secs (3-4hours)
const int UsolarMin = 3.5 * 1024 / 5;        // Minimal solar voltage, then start Dark time
const int UbatMin = 3.7 * 1024 / 5;        // Minimal (critical) battery level V * 1024/5
const int UbatRestore = 3.9 * 1024 / 5;     // After discharge need to reach this value to be charged again


boolean dayTime = true;
boolean batLow = false;
boolean ledDone = true;                     //Flag Job- finished (timer expired or day starts before timer expire
 
int ledState = LOW;             // ledState used to set the LED
int ledONtimer = 0;             // timer actual LEDs are ON
int Usolar = 0;
int Ubat = 0;

unsigned long previousMillis = 0;        // will store last time LED was updated
const int interval = 1000;           // interval at which to check all status and make actions (milliseconds)

 
void setup() {
  // set the digital pin as output:
  #ifdef DEBUG
    Serial.begin( 9600);
  #endif
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);      
}

void loop() {
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    previousMillis += interval;   

    // Do 1 per Second
    voltmeter();
    #ifdef DEBUG
      DEBUG_PRINT
    #endif
        
    if (dayTime){    //Day time. charging

       if (ledState == HIGH){  //LEDs on, mean LED still ON, but already not dark

         ledOFF();
         ledONtimer = 0;
                 
       }else {                //Leds OFF. Check if finished normally?

         if(  batLow && Ubat >= UbatRestore ) { 
            batLow = false;                       //Если батарея была пустая, но потом зарядилась
         }
        
       }
       ledDone = false;       //Job finished before timer expire couse a)day come or b) battery was empty





    } else {  //Night time. Lights or off if battery empty

        //Check Battery state than action
      if ( batLow){
          if( ledState == HIGH)    
               ledOFF();
          
      } else {  //Battery not empty

        if ( ledDone ==false){
          
          //Lighting Cycle not finished 1st time (no Lights - init) than count timer
          if ( ledState ==LOW){
              // 1st time, swithc ON the lights
              ledON();
              //ledONtimer = 0;
            
          } else {      //ledState == HIGH, already light
              
              ++ledONtimer;
              if (ledONtimer >= ledONmaxTime){
                  //Finished for tonight until next day
                  ledOFF();
                  ledONtimer = 0;
                  ledDone = true;
              }
            
          }
        }  //end ledDone==false
      
      }

    }


    //END Do 1 per Second
  }
  sleep(200);
}


/* 
 *  Put controller in sleep mode 
 */
void sleep( int sleeptime){
  delay( sleeptime);      //DEBUG change for real sleep mode. depends on HW
  
}

/*
 * Read voltage on deviders Solar and Battery and set Flags
 * 
 */
 void voltmeter(){
    Usolar = analogRead(solPin);
    if (Usolar <= UsolarMin) dayTime = false; else dayTime = true;

    Ubat = analogRead( batPin);
    if ( Ubat <= UbatMin){
      batLow = true;
    } else{
      if (Ubat >= UbatRestore && batLow == true) batLow = false;
    }
 
 }

 void ledON(){
  ledState = HIGH;
  digitalWrite( ledPin, HIGH);
 }
 void ledOFF(){
  ledState = LOW;
  digitalWrite( ledPin, LOW);
 }
