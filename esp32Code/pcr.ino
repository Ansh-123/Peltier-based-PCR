#include <math.h>

int analogPin = 12;

// follows reverese logic ie low is power on
int coolPin = 13;
int heatPin = 4;

// temps for each stage
int denature = 95;
int anneal = 55;
int extend = 77;

// time for each stage
int dTime = 60;
int aTime = 60;
int eTime = 60;

int cycleCount = 2;

void stop(int coolPin, int heatPin){
  digitalWrite(heatPin, HIGH);
  digitalWrite(coolPin, HIGH);
}

void coolDown(int coolPin, int heatPin){
  digitalWrite(heatPin, HIGH);
  digitalWrite(coolPin, LOW);
}

void heatUp(int coolPin, int heatPin){
  digitalWrite(coolPin, HIGH);
  digitalWrite(heatPin, LOW);
}

// reach temp, then holds it for set time
void holdTemp(int temp, int time, int analogPin, int coolPin, int heatPin){
  // get to temp requested
  float curTemp = getTemp(analogPin);
  while (curTemp < temp - 4 || curTemp > temp + 4 ){
    curTemp = getTemp(analogPin);
    Serial.println(curTemp);
    if (curTemp < temp){
      heatUp(coolPin, heatPin);
    }
    else {
      coolDown(coolPin, heatPin);
    }
    delay(1000);
  }

  Serial.println("Temp reached");

  // hold temp for certain time
  for(int j = 0; j < time; j++){
    curTemp = getTemp(analogPin);
    Serial.println(curTemp);
    if (curTemp < temp){
      heatUp(coolPin, heatPin);
    }
    else if(curTemp < temp + 3) {
      stop(coolPin, heatPin);
    }
    else {
      coolDown(coolPin, heatPin);
    }
    delay(1000);
  }
}

float getTemp(int analogPin){
  float T_approx;
  int measure_count = 0;

  float V_0 = 3.3; // voltage reference

  // first resistance value for voltage divider
  float R_1 = 100000.0;
  // fit coefficients
  float a = 0.0007685549004322312;
  float b = 0.00020812709575069568;
  float c = 1.243349218405405e-7;

  int avg_size = 50; // averaging size

  // loop over several values to lower noise
  float T_sum = 0.0;

  float temps[avg_size];

  // average temps
  for (int i = 0;i<avg_size;i++){
    // read the input on analog pin 0:
    int sensorValue = analogRead(analogPin);
    // Convert the analog reading (which goes from 0 - 1023) to voltage reference (3.3V or 5V or other):
    float voltage = V_0 - ((sensorValue/4095.0)*V_0);
    // calc resistance here
    float resist = (voltage * R_1)/(V_0 - voltage);

    // this is where the thermistor conversion happens based on parameters from fit
    float measure = (1/(a + b * log(resist)+ c * pow(log(resist), 3))) - 273.15;
    temps[i] = measure;

    if (measure > 0 && measure < 150){
      T_sum+= measure;
      measure_count++;
    }
  }
  // for soting temps to get median
  /*
  for(int i = 0; i < avg_size - 1; i++){
    for(int j = 0; j < avg_size - 2; j++){
      if (temps[j] > temps[j + 1]){
        float g = temps[j+1];
        temps[j] = temps[j+1];
        temps[j+1] = g;
      }
    }
  }
  */
  // averaging values from loop
  T_approx = T_sum/float(measure_count);
  //T_approx = temps[int(avg_size/2)];
  return T_approx;
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(analogPin, INPUT);
  pinMode(coolPin, OUTPUT);
  pinMode(heatPin, OUTPUT);

  for(int i = 0; i < cycleCount; i++){
    Serial.println("Prepping to denature.");
    holdTemp(denature, dTime, analogPin, coolPin, heatPin);
    Serial.println("Prepping to anneal.");
    holdTemp(anneal, aTime, analogPin, coolPin, heatPin);
    Serial.println("Prepping to extend.");
    holdTemp(extend, eTime, analogPin, coolPin, heatPin);
  }
  stop(coolPin, heatPin);
  Serial.println("Done with PCR!");
}

void loop() {

  
}
