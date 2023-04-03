
// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>


// Callback to be actioned upon rec. of subscribed topic and instance of MQTT client
void callback(char* topic, byte* payload, unsigned int length);
MQTT client("broker.emqx.io", 1883, callback);

// led, trigger and echo pin assignment
const int led = D2;
const int trigPin = D7;
const int echoPin = D6;

// Constants for calculating wave and pat distances, and the expected strings
const long wave_distance = 35;
const long pat_distance = 10;

const String bevan_wave = "Bevan_wave";
const String bevan_pat = "Bevan_pat";

// timer for limiting publish requests
long timer;

void setup() {
    
	Serial.begin(9600);
	
	timer = Time.now();
	
	pinMode(led, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    // 	Connect to broker, with unique connection string
	client.connect("bevanclient_" + String(Time.now()));
	
    // Subscribe to topic
    if (client.isConnected()) {
        client.subscribe("SIT210/wave");
    }
}

void loop() {
    
    // Only bother with recording distance, IF the timer has elapsed (5secs)
    if (Time.now() - 5 > timer) {

        // Quick toggle of trigger
        digitalWrite(trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        // wait for pulse return, and record time as duration.  Use duration to calculate distance in CM
        long duration = pulseIn(echoPin, HIGH);
        long distance = duration * 0.034 / 2;
        
        
        // If distance is < pat_distance, we have pat, else if the distance is < wave_distance, we have wave.

        if (distance < wave_distance) {

            if(distance < pat_distance) {
                if (client.isConnected()) {
                    client.publish("SIT210/wave",bevan_pat);
                }
            }
            else {
                if (client.isConnected()) {
                    client.publish("SIT210/wave",bevan_wave);
                }
            }
            
            // we've made a detection, so reset timer
            timer = Time.now();
        }
        
    }

    if (client.isConnected()) {
        client.loop();
    }
    // short delay to reset the sensor
    delay(100);
}


void callback(char* topic, byte* payload, unsigned int length) {
    
    // Load the payload and convert to a string
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String sp = String(p);
    
    // Check the string against the expected string values
    
    if (sp == bevan_wave) {
        Serial.printlnf(p);
            // Flash led 3 times
        for (int i = 0; i < 3; i++) {
            digitalWrite(led, HIGH);
            delay(300);
            digitalWrite(led, LOW);
            delay(300);
        }
    }
    else if (sp == bevan_pat){
        Serial.printlnf(p);
        // fast flash led 5 times
        for (int i = 0; i < 5; i++) {
            digitalWrite(led, HIGH);
            delay(100);
            digitalWrite(led, LOW);
            delay(100);
        }
    }

}


