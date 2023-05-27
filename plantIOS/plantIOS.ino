#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

// network credentials
const char* ssid = "name";
const char* password = "pass";

//Moisture Level Sensor Variables
const int fromLow = 1100;   
const int fromHigh = 2600;  
const int toLow = 0;
const int toHigh = 100;
int intervals = (fromLow - fromHigh)/3;   
int soilMoistureValue = 0;
int soilMoisturePer = 0;

//LED Variables
#define LED_PIN     13
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

//declare the server
AsyncWebServer server(80);


//current color variable
String currentColor = "Green";

void setup() {
  //begin coms with 115200 BAUD
  Serial.begin(115200);

  //declare the FASTLED library
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("WiFi connected");

  Serial.println(WiFi.localIP());

  // Handle root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = R"html(
     <!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Module 12</title>
    <style>
.header{
  background-color: #033607;
  color: white;
  height: 125px;
  display: flex;  /*we shoudl probrably have a flex-direcxtion. */
  align-items: center;
  justify-content: center;
  padding: 30px 10px 20px;
  font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
  letter-spacing: 5px;
  border-radius: 15px;
  

}
/* using flex box the parent is the container and must have display:flex */
.flexcontainer {
display: block;
/* which direction to layout items/children ? 
 set to row if child items are the columns
*/
}
.flexcontainer h2{
flex-direction: row;
    font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
  letter-spacing: 2px;
  font-weight: lighter;
}           
/* children items needs rules if each needs diff widths */
.myleft {
/* items in a flexbox use 2 css properties to control their width */
padding-right:10px;
text-align: center;
padding-top: 20px;
border: 2px  solid #033607;
border-radius: 15px ;
margin: 20px 10px 10px 10px;
}
.mymiddle {

text-align: center;
padding: 20px;
border: 2px  solid #033607;
border-radius: 15px ;
margin: 20px 10px 10px 10px;

}
.myright {

padding-left:10px;
text-align: center;
padding-top: 20px;
border: 2px  solid #033607;
border-radius: 15px ;
margin: 20px 10px 10px 10px;
}
 
  </style>
    
  </head>
  <body>
    <header class="header">  
      <div class="title">
      <h1>Plant-OS</h1>
      </div>
    </header>
    <section class="flexcontainer">
            <aside class="myleft">
                <h2>Summary</h2>
                <p> Muhlenbergia rigens, commonly known as deergrass, is a warm season perennial bunchgrass. It is found in sandy or well-drained soils below 7,000 feet in elevation in the Southwestern United States and parts of Mexico. It is often used as a decorational plant </p>
            </aside>
            <article class="mymiddle">
                <h2>Color</h2>
                <p>Color: <span id="color">0</span>  </p>
                <script>
            setInterval(function() {
              var xhttp = new XMLHttpRequest();
              xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                  document.getElementById("color").innerHTML = this.responseText;
                }
              };
              xhttp.open("GET", "/color", true);
              xhttp.send();
            }, 1000);
          </script>
            </article>
            <aside class="myright">
                <h2>Moisture</h2>
                <p>Moisture: <span id="moisture">0</span> % </p>
                <script>
            setInterval(function() {
              var xhttp = new XMLHttpRequest();
              xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                  document.getElementById("moisture").innerHTML = this.responseText;
                }
              };
              xhttp.open("GET", "/moisture", true);
              xhttp.send();
            }, 1000);
          </script>
            </aside>
        </section>
  </body>
</html>
    )html";
    request->send(200, "text/html", html);
  });

  // Route to handle requests to the /count path
  server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(soilMoisturePer));
});

  server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(currentColor));   
  });
    

  
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {

  soilMoistureValue = analogRead(A6);  

  soilMoisturePer = map(soilMoistureValue, fromLow, fromHigh, toHigh, toLow);

  if(soilMoisturePer < 10)
  {

  currentColor = "Red";
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
  delay(1000);   
  
  }
 
  else if(soilMoisturePer <= 50)
  {
  currentColor = "Yellow";
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();
  delay(1000);   
  
  }
  else if(soilMoisturePer <= 100)
  {
    
  currentColor = "Green";
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
  delay(1000);   
  
  }
  //delay 1 second
  delay(1000);
}
