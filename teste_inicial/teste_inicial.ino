#include <ESP8266WiFi.h> //biblioteca esp8266
#include <WiFiClient.h> //biblioteca para conectar no Wi-Fi
#include <ESP8266WiFiMulti.h>  //biblioteca pra conectar a mais de uma rede Wi-Fi
#include <ESP8266mDNS.h> //biblioteca pra nao ter que colocar o ip do esp8266 
#include <ESP8266WebServer.h> //biblioteca pra criar o webserver com o esp

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

const int led = 2;

void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();



/*   SETUP   */
void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  pinMode(led, OUTPUT);

  const char* ssid_AP_1="Wi-Fly";
  const char* password_AP_1="2#Ferriso#2";
  //const char* ssid_AP_2="";
  //const char* password_AP_2="";
  //const char* ssid_AP_3="";
  //const char* password_AP_3="";

    
  wifiMulti.addAP(ssid_AP_1, password_AP_1); // add Wi-Fi networks you want to connect to
  //wifiMulti.addAP("ssid_AP_2", "password_AP_2");
  //wifiMulti.addAP("ssid_AP_3", "password_AP_3");
  Serial.println("Connecting ...");
  
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  //SERVIDOR WEB
  //1. Faz um GET pra pegar o html do form com botão quando o cliente pede o URI "/"
  //2. Faz um POST pra mudar o estado do botão quanto o URI "/LED" é requisitado pelo cliente
  //3. Se não achar a URI devolve o código 404

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/LED", HTTP_POST, handleLED);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}


/*   LOOP (só fica ouvindo na porta 80 infinito)   */
void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients
}






/*   FUNÇÕES de handle do HTML   */

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Alterar estado do LED\"></form>");
}

void handleLED() {                          // If a POST request is made to URI /LED
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
