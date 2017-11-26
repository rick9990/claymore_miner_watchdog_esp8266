#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ArduinoJson.h>
#include <Arduino.h>

const char* ssid     = "A_das_WiFi";
const char* password = "rick8620";

const char host[] = "192.168.0.103";
const char* remote_host = "www.google.com";

String data = "";   // String with json data
float temp = 0.0;
int pin = D2;
int restcnt = 1;
int check = 1;
WiFiClient client;


unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

void setup() {
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  
 }

void loop() {
  delay(150000);
  Serial.print("\n\nstarting new loop\n");
  Serial.print("Pinging host ");
  Serial.println(remote_host);

  if(Ping.ping(remote_host)) {
    Serial.println("ping Success!!");
    
  } else { 
    if(Ping.ping(remote_host)) {
    Serial.println("ping Success!!");}
    else {      
            if(restcnt <=3){
            Serial.println("ping Error :(");
            restart();
            restcnt++;
            return;
            } else{ 
                delay(600000);
                restart();
                return;
              } 
     }
  }
  // if ten seconds have passed since your last connection,then connect again and send data:
 // if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();

    char buffer[data.length() + 1];
    data.toCharArray(buffer, sizeof(buffer));     // Copy json String to char array
    buffer[data.length() + 1] = '\0';

    parseJSON(buffer);
 // }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    data = client.readStringUntil('\r');
   // Serial.print(data);
  }
}

void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  delay(1000);

  Serial.print("connecting to miner ...\n");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  const int httpPort = 3333;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection to miner failed !");
    restart();
    return;
  }

  Serial.print("Requesting miner: ");
  // This will send the request to the server
  client.print(String {"{\"id\":0,\"jsonrpc\":\"2.0\",\"method\":\"miner_getstat1\"}"} + " HTTP/1.1\r\n");
  client.print("Host: 192.168.0.103\r\n");
  client.print("Connection: close\r\n\r\n");
  // note the time that the connection was made:

  //lastConnectionTime = millis();
  delay(1000);
}


/*void parseJSON(char json[1000])
{
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success())
  {
    Serial.print("ParseObject() failed");
    //return;
  }
*/
    void parseJSON(char json[])  {
      StaticJsonBuffer<200> jsonBuffer;
    Serial.print(json);
    Serial.println();
      JsonObject& root = jsonBuffer.parseObject(json);
    
     /* if (!root.success()) {
      //Serial.println("parseObject() failed");
        return;
      }
    */

  //double temp = root["temp"];
  //Serial.print(data);
  if(data[0]!= NULL){
     int i,j,n=0;
     String mystrarry[20];
     for(i=0;data[i]!='\0';){
      if(data[i]=='"'){
          for(j=i+1;data[j]!='"';j++){
         mystrarry[n]+= data[j];
         //Serial.print(data[j]);
           } 
           n++;
           i=j++;
           }
           if(i!=j++)i++;
    }
     
   /*  for(i=0;mystrarry[i]!='\0';i++){
      Serial.println(mystrarry[i]);
      }
*/
     char index3[20];
     String algo ;
      (mystrarry[3]).toCharArray(index3,20);
       //Serial.println(index3[0]);
     for(i=0;index3[i]!= NULL;i++){
      if(isAlpha(index3[i])){
        algo+=index3[i];
      }
     }

     char index5[20];
     char  hsrt[2] ;
     (mystrarry[5]).toCharArray(index5,20);
     for(i=0;index5[i]!= ';';i++){
      hsrt[i]=index5[i];
      }
     //Serial.println(algo);
     //Serial.println(hsrt);
     int hsrtint = atoi(hsrt);
     
     if(algo=="ETH"){ 
          if(hsrtint<=123000 ){
             if(check ==3){
                Serial.println("low hash rate restarting...");
                check=1;
                restart();
                return;
              }
            Serial.println("WARNING:low hash rate...");
            check++;
            return;
        }else{
          Serial.println("Ethereum Hashrate ok..");
          check=1;
        }
     }
       
        if(algo=="XMR"){ 
                  if(hsrtint<=2800){
                     if(check ==3){
                        Serial.println("low hash rate restarting...");
                        check=1;
                        restart();
                        return;
                      }
                    Serial.println("WARNING:low hash rate...");
                    check++;
                    return;
                }else{
                  Serial.println("Monero Hashrate ok..");
                  check =1;
                }
             }

               if(algo=="ZEC"){ 
                      if(hsrtint<=1200){
                         if(check ==3){
                            Serial.println("low hash rate restarting...");
                            check=1;
                            restart();
                            return;
                          }
                        Serial.println("WARNING:low hash rate...");
                        check++;
                        return;
                    }else{
                      Serial.println("Zcash Hashrate ok..");
                      check=1;
                    }
                 }  

     
    }
  } 

  void restart(){
      digitalWrite(pin, LOW);
      Serial.println("relay on!");
      delay(2000);
      digitalWrite(pin, HIGH);
      Serial.println("relay off!");
      delay(90000);
      return;
    }
