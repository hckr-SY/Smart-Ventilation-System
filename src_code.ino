#include <ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<Servo.h>

Servo myservo;

const int gasPin=A0;
int gasVal=0;
bool activate=false;
bool trigger=false;
String activeStr="deActive";


const int httpPort=80;
String url="/trigger/gas_leak/json/with/key/krk3j4BYSBZzxSs0tJHth7NwavugJLGLKZplx1OAi63";
HTTPClient http; 


// Enter your wifi network name and Wifi Password
const char* ssid = "Test101";
const char* password = "psd90365";
const char* host = "maker.ifttt.com";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// These variables store current output state of LED
String windowState = "closed";
//String outputGreenState = "off";
String buzzState = "off";

// Assign output variables to GPIO pins
const int servoPin = 14; //d5
const int buzzPin = 12; //d6
//const int greenLED = 4;
//const int yellowLED = 5;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void servoOf()
{
  myservo.write(150);
}

void servoOn()
{
  myservo.write(10);
}

void sendEmail()
{
  WiFiClient client;
Serial.println("you are in: ");
http.begin(client,host,httpPort,url);
int httpCode= http.GET();
Serial.println(httpCode);
delay(2000);
                          
}


bool isSafe(int val)
{
    
  if(val>=300)
  {
    return false;
  }

  return true;
  
}



void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  pinMode(gasPin,INPUT);
  pinMode(servoPin, OUTPUT); 
  pinMode(buzzPin,OUTPUT);
  myservo.attach(servoPin);
 myservo.attach(servoPin,500,2400);
servoOf();


 //pinMode(greenLED, OUTPUT);
  //pinMode(yellowLED,OUTPUT);
  // Set outputs to LOW
//  digitalWrite(servoPin, LOW);
  //digitalWrite(greenLED, LOW);
  //digitalWrite(yellowLED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;



    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /14/open") >= 0) {
              Serial.println("Window is open");
              windowState = "open";
              //digitalWrite(servoPin, HIGH);
              servoOn();
            } else if (header.indexOf("GET /14/closed") >= 0) {
              Serial.println("window is closed");
              windowState = "closed";
              // digitalWrite(servoPin, LOW);
              servoOf();
            } 
            else if (header.indexOf("GET /66/activate") >= 0) {
//              Serial.println("Green LED is on");
//              outputGreenState = "on";

                activeStr="active";
                
                activate=true;
                trigger=false;
                
              
//              digitalWrite(greenLED, HIGH);
            } else if (header.indexOf("GET /66/deActivate") >= 0) {
//              Serial.println("Green LED is off");
//              outputGreenState = "off";
//              digitalWrite(greenLED, LOW);


                activeStr="deActive";
                activate=false;
                

                
            }
               
               
//               else if (header.indexOf("GET /5/buzzOn") >= 0) {
//              //Serial.println("Yellow LED is on");
//              buzzState = "on";
////              digitalWrite(yellowLED, HIGH);
//            } 
            
            else if (header.indexOf("GET /5/buzzOff") >= 0) {
              //Serial.println("Yellow LED is off");
              buzzState = "off";
              trigger=false;
              digitalWrite(buzzPin, LOW);
              
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonRed { background-color: #ff0000; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonGreen { background-color: #00ff00; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
//            client.println(".buttonYellow { background-color: #feeb36; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
//            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonOff { background-color: #77878A; border: none; color: white; padding: 16px 40px; border-radius: 70%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
//              client.println("</style></head>");

            // Web Page Heading
            client.println("<body><h1>My Window Controller</h1>");

            // Display current state, and ON/OFF buttons for GPIO 2 Red LED
            client.println("<p>Window is " + windowState + "</p>");
            // If the windowState is off, it displays the OFF button
            if (windowState == "closed") {
              client.println("<p><a href=\"/14/open\"><button class=\"button buttonOff\">CLICK TO OPEN</button></a></p>");
            } else {
              client.println("<p><a href=\"/14/closed\"><button class=\"button buttonRed\">CLICK TO CLOSE</button></a></p>");
            }

//            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for GPIO 4 Green LED
            client.println("<p>Gas Sensor is  " + activeStr + "</p>");
            // If the outputGreenState is off, it displays the OFF button
            if (activeStr == "deActive") {
              client.println("<p><a href=\"/66/activate\"><button class=\"button buttonOff\">CLICK TO ACTIVATE</button></a></p>");
            } else {
              client.println("<p><a href=\"/66/deActivate\"><button class=\"button buttonGreen\">CLICK TO DEACTIVATE</button></a></p>");
            }


           //   client.println("</body></html>");
            

            // Display current state, and ON/OFF buttons for GPIO 5 Yellow LED

            if(trigger)

            {
            client.println("<p>Buzzer is on </p>");
            // If the buzzState is off, it displays the OFF button
//            if (buzzState == "on") {
              client.println("<p><a href=\"/5/buzzOff\"><button class=\"button buttonOff\">Click to turn off buzzer</button></a></p>");
//            } 
            
            
//            else {
//              client.println("<p><a href=\"/5/off\"><button class=\"button buttonYellow\">ON</button></a></p>");
//            }

            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

if(activate)
{
  gasVal=analogRead(gasPin);
  Serial.println(gasVal);
  if(!isSafe(gasVal))
  {
    servoOn();        
    sendEmail();
    windowState="open";    
    activate=false;
    activeStr="deActive";
    trigger=true;
  }

  
}

if(trigger)
{
  digitalWrite(buzzPin,HIGH);
  delay(300);
  digitalWrite(buzzPin,LOW);
  delay(300);
}





}
