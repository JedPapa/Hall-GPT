#include <FirebaseArduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "https://hall-gpt-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "8GcYq2UoNOIxPP8GU683qlJXICG3ygHEP2JGToRI"


// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);

TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 

const char* ssid = "Reno4";
const char* password = "123456789";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;


WiFiServer server(80);
void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    unsigned long last_millis = millis();
}

void loop()
{
  feed_gps();
    // set bool value
    Firebase.setBool("truth", false);
    // handle error
    if (Firebase.failed())
    {
        Serial.print("setting /truth failed:");
        Serial.println(Firebase.error());
        return;
    }

    char location_str[25] = {0};
    // store string of lat,lng
    sprintf(location_str, "%f, %f", lat, lng);

    String name = Firebase.push("location", location_str);
    // handle error
    if (Firebase.failed())
    {
        Serial.print("pushing /location failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print("pushed: /location/");
    Serial.println(name);
}

  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 5; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }

    }
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td>  </tr> </table> ";
 
  
  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

}