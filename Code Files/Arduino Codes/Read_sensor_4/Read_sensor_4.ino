#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemperature(&oneWire);
float water_temperature = 0;


#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


#define EC_on 6

//PH
float m_calibration_value = 4.556;
float c_calibration_value = -3.0818
;
int phval = 0;
unsigned long int avgval;
int buffer_arr[30], temp;
float ph_act;
//PH


//EC
#define TdsSensorPin A2
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 31.0, EC = 0;
//EC

//Ultrasonic Sensor
const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;
float waterLevel;
float groundLevel= 13;
//



void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(EC_on, OUTPUT);

  dht.begin();
  dallasTemperature.begin();

  pinMode(TdsSensorPin,INPUT);
  pinMode(A3,INPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  delay(1000);

}
void loop() {

  //This section measures PH of the system START
  for (int i = 0; i < 30; i++)
  {
    buffer_arr[i] = analogRead(A3);
    delay(30);
  }

  for (int i = 0; i < 29; i++)
  {
    for (int j = i + 1; j < 30; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;

  for (int i = 10; i < 20; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 10;
  ph_act = m_calibration_value * volt + c_calibration_value;
  // This sections measures PH of the section end


  //This Section measures air temperature and humidity start
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float air_temperature = dht.readTemperature();
  //This section measures air temperature and humidity end



  // Waterproof Temperature Sensor START
  dallasTemperature.requestTemperatures();
  water_temperature = dallasTemperature.getTempCByIndex(0);
  //Waterproof Temperature Sensor END


  // Ultrasonic Sensor START
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  waterLevel = groundLevel - distance;
  
  //Ultrasonic Sensor END


  //This section measures EC start
  digitalWrite(EC_on, HIGH);
  delay(50);

  for (int z = 0; z < 30; z++)
  {
     
     analogBuffer[z] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer 
     delay(40);
  }

  delay(50);
    
  for(copyIndex=0;copyIndex<SCOUNT;copyIndex++){
    analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
  }
    averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
//    EC=(794.36*compensationVolatge*compensationVolatge*compensationVolatge - 55.924*compensationVolatge*compensationVolatge - 215.82*compensationVolatge + 170.3); //convert voltage value to tds value
//    EC = tdsValue * 2;
      
  
  delay(50);
  digitalWrite(EC_on, LOW);
  //This section measures EC end


  // Create a byte array to hold the data
  byte data[sizeof(float) * 6];
  
  // Copy the float values to the byte array
  memcpy(data, &compensationVolatge, sizeof(float));
  memcpy(data + sizeof(float), &ph_act, sizeof(float));
  memcpy(data + (sizeof(float) * 2), &humidity, sizeof(float));
  memcpy(data + (sizeof(float) * 3), &air_temperature, sizeof(float));
  memcpy(data + (sizeof(float) * 4), &water_temperature, sizeof(float));
  memcpy(data + (sizeof(float) * 5), &waterLevel, sizeof(float));

  // Send the byte array
  Serial.write(data, sizeof(float) * 6);

  // Delay before sending the values again (adjust as needed)
  delay(20);

  
//  Serial.print("EC:");
//  Serial.print(EC,0);
//  Serial.println("  micro-siemens/cm");
//  Serial.print("pH Val: ");
//  Serial.println(ph_act);
//  Serial.print(F("Humidity: "));
//  Serial.print(humidity);
//  Serial.println("%");
//  Serial.print(F("Air Temperature: "));
//  Serial.print(air_temperature);
//  Serial.println(F("°C "));
//  Serial.print("Water Temperature: ");
//  Serial.print(water_temperature);
//  Serial.println("°C");
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
//  Serial.println();
//  Serial.println();

}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
