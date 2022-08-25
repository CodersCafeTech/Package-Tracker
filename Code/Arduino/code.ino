
void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(String Data);
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000);

const String APN  = "airtelgprs.com";
const String USER = "";
const String PASS = "";

const String FIREBASE_HOST  = "https://counter-a2b53-default-rtdb.firebaseio.com/";
const String FIREBASE_SECRET  = "va6SGLXk01DPOICa8afZkiQh8kWhx3z1usS1s9Qb";

String Label;

/* Includes ---------------------------------------------------------------- */
#include <Package_Tracker_inferencing.h>
#include <Arduino_LSM9DS1.h>

/* Constant defines -------------------------------------------------------- */
#define CONVERT_G_TO_MS2    9.80665f
#define MAX_ACCEPTED_RANGE  2.0f        // starting 03/2022, models are generated setting range to +-2, but this example use Arudino library which set range to +-4g. If you are using an older model, ignore this value and use 4.0f instead
#define USE_SSL true
#define DELAY_MS 500


/* Private variables ------------------------------------------------------- */
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600); // For GSM module
    Serial.println("Edge Impulse Inferencing Demo");
    if (!IMU.begin()) 
    {
        ei_printf("Failed to initialize IMU!\r\n");
    }
    else 
    {
        ei_printf("IMU initialized\r\n");
    }

    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) 
    {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
        return;
    }
}

float ei_get_sign(float number) 
{
    return (number >= 0.0) ? 1.0 : -1.0;
}

void loop()
{
    ei_printf("\nStarting inferencing in 2 seconds...\n");

    delay(500);

    ei_printf("Sampling...\n");

    // Allocate a buffer here for the values we'll read from the IMU
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        IMU.readAcceleration(buffer[ix], buffer[ix + 1], buffer[ix + 2]);

        for (int i = 0; i < 3; i++) {
            if (fabs(buffer[ix + i]) > MAX_ACCEPTED_RANGE) {
                buffer[ix + i] = ei_get_sign(buffer[ix + i]) * MAX_ACCEPTED_RANGE;
            }
        }

        buffer[ix + 0] *= CONVERT_G_TO_MS2;
        buffer[ix + 1] *= CONVERT_G_TO_MS2;
        buffer[ix + 2] *= CONVERT_G_TO_MS2;

        delayMicroseconds(next_tick - micros());
    }

    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0) {
        ei_printf("Failed to create signal from buffer (%d)\n", err);
        return;
    }

    // Run the classifier
    ei_impulse_result_t result = { 0 };

    err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", err);
        return;
    }

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
    result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) 
    {
        //ei_printf("%s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        if(result.classification[ix].value > 0.95 )
        {
          Label = String(result.classification[ix].label);
          Serial.println(Label);
           if (Label == "Idle")
             {
                 Serial.println("Idle state");
             }
             else if (Label == "Normal_carrying")
             {
                 Serial.println("Normal_carrying");
             }
             else if (Label == "Hard_Fall")
             {
                 Serial.println("Hard Fall");
                 alert(Label);
             }
             else if (Label == "Throwing")
             {
                 Serial.println("Throwing");
                 alert(Label);
             }
             else if (Label == "Vigorous_shaking")
             {
                 Serial.println("Vigorous_shaking");
                 alert(Label);
             }
        }
    }
// Here we didn't care about anomalies
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
void alert(String lab)
{
  Serial.println("Initializing Serial1...");
  init_gsm();
  if(!is_gprs_connected())
  {
    gprs_connect();
  }
  String Data = "{\"Val\":"+get_time()+"\","+"\"Label\":"+"\""+String(lab)+"\"""}";
  Serial.println(Data);
  post_to_firebase(Data);
  delay(1000);
}

String get_time()
{
  Serial1.println("AT+CCLK?"); 
  waitResponse("+CCLK:");
  delay(DELAY_MS);
  String res = Serial1.readStringUntil('+');
  Serial.println(res);
  waitResponse("OK");
  return(res);
}

void init_gsm()
 {
      //Testing AT Command
      Serial1.println("AT");
      waitResponse();
      delay(DELAY_MS);
      //Checks if the SIM is ready
      Serial1.println("AT+CPIN?");
      waitResponse("+CPIN: READY");
      delay(DELAY_MS);
      //Turning ON full functionality
      Serial1.println("AT+CFUN=1");
      waitResponse();
      delay(DELAY_MS);
      //Turn ON verbose error codes
      Serial1.println("AT+CMEE=2");
      waitResponse();
      delay(DELAY_MS);
      //Enable battery checks
      Serial1.println("AT+CBATCHK=1");
      waitResponse();
      delay(DELAY_MS);
      //Register Network (+CREG: 0,1 or +CREG: 0,5 for valid network)
      //+CREG: 0,1 or +CREG: 0,5 for valid network connection
      Serial1.println("AT+CREG?");
      waitResponse("+CREG: 0,");
      delay(DELAY_MS);
      //setting SMS text mode
      Serial1.print("AT+CMGF=1\r");
      waitResponse("OK");
      delay(DELAY_MS);
 }
void post_to_firebase(String Data)
{
  
  //Start HTTP connection
  Serial1.println("AT+HTTPINIT");
  waitResponse();
  delay(DELAY_MS);
  //Enabling SSL 1.0
  if(USE_SSL == true)
  {
    Serial1.println("AT+HTTPSSL=1");
    waitResponse();
    delay(DELAY_MS);
  }
  //Setting up parameters for HTTP session
  Serial1.println("AT+HTTPPARA=\"CID\",1");
  waitResponse();
  delay(DELAY_MS);
  //Set the HTTP URL - Firebase URL and FIREBASE SECRET
  Serial1.println("AT+HTTPPARA=\"URL\","+FIREBASE_HOST+"Alert/"+".json?auth="+FIREBASE_SECRET);
  waitResponse();
  delay(DELAY_MS);
  //Setting up re direct
  Serial1.println("AT+HTTPPARA=\"REDIR\",1");
  waitResponse();
  delay(DELAY_MS);
  //Setting up content type
  Serial1.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  waitResponse();
  delay(DELAY_MS);
  //Setting up Data Size
  //+HTTPACTION: 1,601,0 - error occurs if Data length is not correct
  Serial1.println("AT+HTTPData=" + String(Data.length()) + ",10000");
  waitResponse("DOWNLOAD");
  delay(DELAY_MS);
  //Sending Data
  Serial1.println(Data);
  waitResponse();
  delay(DELAY_MS);
  //Sending HTTP POST request
  Serial1.println("AT+HTTPACTION=1");
  
  for (uint32_t start = millis(); millis() - start < 20000;){
    while(!Serial1.available());
    String response = Serial1.readString();
    if(response.indexOf("+HTTPACTION:") > 0)
    {
      Serial.println(response);
      break;
    }
  }
    
  delay(DELAY_MS);
  //+HTTPACTION: 1,603,0 (POST to Firebase failed)
  //+HTTPACTION: 0,200,0 (POST to Firebase successfull)
  //Read the response
  Serial1.println("AT+HTTPREAD");
  waitResponse("OK");
  delay(DELAY_MS);

  //Stop HTTP connection
  Serial1.println("AT+HTTPTERM");
  waitResponse("OK",1000);
  delay(DELAY_MS);
}

//Connect to the internet

void gprs_connect()
{
  //DISABLE GPRS
  Serial1.println("AT+SAPBR=0,1");
  waitResponse("OK",60000);
  delay(DELAY_MS);
  //Connecting to GPRS: GPRS - bearer profile 1
  Serial1.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  //sets the APN settings for your sim card network provider.
  Serial1.println("AT+SAPBR=3,1,\"APN\","+APN);
  waitResponse();
  delay(DELAY_MS);
  //sets the user name settings for your sim card network provider.
  if(USER != ""){
    Serial1.println("AT+SAPBR=3,1,\"USER\","+USER);
    waitResponse();
    delay(DELAY_MS);
  }
  //sets the password settings for your sim card network provider.
  if(PASS != ""){
    Serial1.println("AT+SAPBR=3,1,\"PASS\","+PASS);
    waitResponse();
    delay(DELAY_MS);
  }
  //after executing the following command. the LED light of 
  //Serial1l blinks very fast (twice a second) 
  //enable the GPRS: enable bearer 1
  Serial1.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
  //Get IP Address - Query the GPRS bearer context status
  Serial1.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
}



/* Function: gprs_disconnect()
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network */

boolean gprs_disconnect()
{
  //Disconnect GPRS
  Serial1.println("AT+CGATT=0");
  waitResponse("OK",60000);
  delay(DELAY_MS);
  //DISABLE GPRS
  //Serial1.println("AT+SAPBR=0,1");
  //waitResponse("OK",60000);
  //delay(DELAY_MS);

  return true;
}

/*
* Function: gprs_disconnect()
* checks if the gprs connected.
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network */

boolean is_gprs_connected()
{
  Serial1.println("AT+CGATT?");
  if(waitResponse("+CGATT: 1",6000) == 1) { return false; }

  return true;
}

//Handling AT COMMANDS
boolean waitResponse(String expected_answer, unsigned int timeout) //uncomment if syntax error (esp8266)
{
  uint8_t x=0, answer=0;
  String response;
  unsigned long previous;
    
  //Clean the input buffer
  while( Serial1.available() > 0) Serial1.read();
  previous = millis();
  do{
    if(Serial1.available() != 0){
        char c = Serial1.read();
        response.concat(c);
        x++;
        //checks if the (response == expected_answer)
        if(response.indexOf(expected_answer) > 0){
            answer = 1;
        }
    }
  }while((answer == 0) && ((millis() - previous) < timeout));
  Serial.println(response);
  return answer;
}
