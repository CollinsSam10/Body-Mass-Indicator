 
/*Body Mass Indicator Project Code 
 * Written by Collins Sam Ayipeh and Beatrice Kpornyo
 
          
This sketch read ping reflected from a patient head using an ultrasonic sensor and compute the height of the patient
with the range obtained by the ping reflected and a fixed distance in-between the ultrasonic sensor and 
the weight measuring scale(load cell). 

It also reads the output of the load cell amplified by a HX711 amplifier, computes it and returns the weight of the 
patient on the weight measuring scale(load cell).

Finally it calculates the body mass index of the patient(kg/m^2) using the height and weight acquired and displays the 
weight, height, body mass index and recommendations corresponding to the body mass index on a 16*2 Liquid 
Crystal Display. 
It also blinks a particular LED which also corresponds to one of the four BMI ranges. 
 
*/

#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
//Serial clock = pin A5
//Serial data = pin A4

const int DOUT = 6;//digital pin for load cell data output
const int CLK = 5; //digital pin for load cell serial clock  
HX711 scale(DOUT, CLK); //HX711 scale(6, 5);
float calibration_factor = -101; //calibration factor for load cell NB:This is unique to every weight measuring load cell
float grams;

 
const float fullLength = 85;  //Fixed Length between ultrasonic sensor and weighing scale in inches
const int trigPin = 4;  //Trigger pin of ultrasonic sensor 
const int echoPin = 3;  //Echo pin of ultrasonic sensor

const int red1 = 10;
const int green = 11; 
const int yellow = 12;
const int red2 = 13;



void setup() {
 Serial.begin(9600);//Setting the baud rate

//Setting digital pins for LEDs in series with resistors
 pinMode(red1,OUTPUT); //Red LED for Underweight BMI
 pinMode(green,OUTPUT); //Green LED for Normal BMI
 pinMode(yellow,OUTPUT); //Yellow LED for Overweight BMI
 pinMode(red2,OUTPUT); //Different red LED for Obese BMI

 scale.set_scale(calibration_factor);// setting up scale for load cell
 scale.tare();
 
 pinMode(trigPin,OUTPUT);
 pinMode(echoPin,INPUT);
 
 lcd.init();//initializing lCD screen 
 lcd.backlight();//turning on LCD backlight
}

void loop() {
  
  delay(5000);
  digitalWrite(trigPin,LOW); //Setting trig pin to low 
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);//setting trig pin to high, thus producing ultrasound
  delayMicroseconds(5);
  digitalWrite(trigPin,LOW);// Setting it back to low after 10microseconds delay
  
  float reflectionTime = pulseIn(echoPin,HIGH);  //Saving the duration of the reflected ultrasounds in units of microseconds 
  float gapLengthInInches = microsecondsToInches(reflectionTime);//function call for changing reflection time into distance(units). Returned value is stored in gapLengthInInches
  float patientHeightInInches = calculateHeight(gapLengthInInches);//Function call for calculating patient's height(inches). Value returned is stored in patientHeightInInches
  float patientHeightInMetres = InchesToMetres(patientHeightInInches);

  grams = scale.get_units(),10; //Getting output value of the load cell,thus the weight of the patient in grams  
  if (grams < 0) // if when value is 0 or negative, grams should be set to zero
  {
    grams = 0.00;
  }
  
  float patientWeightInKg = grams/1000; //Changing grams to Kilograms
  
  float patientBMI = patientWeightInKg/pow(patientHeightInMetres,2); //calculating BMI in kg/m^2

  //Setting cursor position to column 0,row 0 on the 16*2 LCD screen and displaying weight value in Kilogram
  lcd.setCursor(0,0);
  lcd.print("Weight:");
  lcd.print(patientWeightInKg);
  lcd.print("kg");

  
  //Setting cursor position to column 0, row 1 on the 16*2 LCD screen and displaying Height value in Inches 
  lcd.setCursor(0,1);
  lcd.print("Height:");
  lcd.print(patientHeightInInches);
  lcd.print("inc");

  delay(5000);//Delay for 5 seconds and initialize screen
  lcd.init();

  //Setting cursor position to column 0, row 0 on the 16*2 LCD screen and displaying BMI value in kilogram per meters square 
  lcd.setCursor(0,0);
  lcd.print("BMI:");
  lcd.print(patientBMI);
  lcd.print("kg/m^2");
  
  lcd.setCursor(0,1);

// If-else loop to determine the LED to blink, the BMI status and corresponding recommendation to display on the LCD screen. 
    if(patientBMI<18.5){
      lcd.print("UNDERWEIGHT");
      delay(5000);
      lcd.init();
      lcd.print("Eat Nutritiously");
      digitalWrite(10,HIGH);
      delay(7000);
      digitalWrite(10,LOW);
      lcd.clear();
  }
    else if(patientBMI>=18.5 && patientBMI<=24.9){
      lcd.print("NORMAL");
      delay(5000);
      lcd.init();
      lcd.print("Keep Your");
      lcd.setCursor(0,1);
      lcd.print("Physique Up");
      digitalWrite(11,HIGH);
      delay(7000);
      digitalWrite(11,LOW);
      lcd.clear();
  }
    else if(patientBMI>=25 && patientBMI<=29.9){
      lcd.print("OVERWEIGHT");
      delay(5000);
      lcd.init();
      lcd.print("Regular Exercise");
      lcd.setCursor(0,1);
      lcd.print("And Eat Healthy");
      digitalWrite(12,HIGH);
      delay(7000);
      digitalWrite(12,LOW);
      lcd.init();
  }

    else{
      lcd.print("OBESE");
      delay(5000);
      lcd.init();
      lcd.print("Seek Medical");
      lcd.setCursor(0,1);
      lcd.print(" Attention");
      digitalWrite(13,HIGH);
      delay(3000);
      digitalWrite(13,LOW);
      lcd.clear();  
      delay(2000);
 }
}

float microsecondsToInches(float microseconds){   //function for changing the time taken to reflect ping(microseconds) into inches 
  
  return (microseconds/2)/74.1;
}
float InchesToMetres(float inches){ //function for changing inches into meters
  return inches/39.3701;
}

float calculateHeight(float gapLength){   //function for calculating the actual height of the patient
   float pat_Height = fullLength-gapLength;
   if (pat_Height<0){
    pat_Height=0;
   }
   return (pat_Height);
}
