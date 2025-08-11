/**
    Basic Inet SCPI command interpreter.
    
 A server that implements a few SCPI commands.
 To use, telnet to your device's IP address and type SCPI commands it understands.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield.

 Uses DHCP to get an IP address

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 11 August 2025
 
 Based on DHCP Chat Server by Tom Igoe.

 */

#include <SPI.h>
#include <Ethernet.h>
#include <avr/boot.h>

//------------------------------------------------------------------------------------
// Defines - SCPI interpreter
//------------------------------------------------------------------------------------

#define SW_VERSION "1.0"

#define BUFFER_SIZE 256
#define LED_PIN 10

#define error(e) last_error = e; errorBeep(); Serial.print("ErrorCode:"); Serial.println(e);

//------------------------------------------------------------------------------------
// Enums - SCPI interpreter
//------------------------------------------------------------------------------------

typedef enum{
  NO_ERROR = 0,
  COMMAND_ERROR = -100,
  INVALID_CHARACTER_ERROR = -101,
  PARAMETER_ERROR = -104,
  INTERNAL_FIRMWARE_ERROR = -300,
}Error;

//------------------------------------------------------------------------------------
// Variables - SCPI interpreter
//------------------------------------------------------------------------------------

const char cmdIdn[] = "*IDN?";
const char cmdVolt[] = "MEAS:VOLT:DC?";
const char cmdErr[] = "SYST:ERR?";
static const char cmdDigOut[] = "DIG:OUT";

uint32_t deviceSignature; // Unique to the type of microcontroller used
uint32_t serialNumberL;
uint32_t serialNumberH;

//------------------------------------------------------------------------------------
// Variables - Network
//------------------------------------------------------------------------------------

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// telnet defaults to port 23
EthernetServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

uint32_t getDeviceSignature(void){
  deviceSignature = boot_signature_byte_get(0x0000);
  deviceSignature <<= 8;
  deviceSignature += boot_signature_byte_get(0x0002);
  deviceSignature <<= 8;
  deviceSignature += boot_signature_byte_get(0x0004);
}

void getSerialNumber(void) {
  serialNumberL = boot_signature_byte_get(0x000F);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x000E);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  serialNumberL += boot_signature_byte_get(0x0010);
  serialNumberL <<= 8;
  
  serialNumberH = boot_signature_byte_get(0x0010);
}

void errorBeep(void){
  /// TODO * loud beep noise *
  int i;

  for (i = 0; i < 2; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  
}

bool startswith(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(LED_PIN, OUTPUT);
  getDeviceSignature();
  getSerialNumber();

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // initialize the Ethernet device not using DHCP:
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // start listening for clients
  server.begin();
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.print("Client ");
      Serial.print(client.remoteIP());
      Serial.println(" connected.");
      
      
      gotAMessage = true;
    }

    // read the bytes incoming from the client:
    char thisChar = client.read();
    
    // send characters to the SCPI interpreter
    UpdateInterpreter(thisChar);
    
    Ethernet.maintain();
  }
}


void UpdateInterpreter(char c) {
  static char command[BUFFER_SIZE];
  static int16_t last_error = NO_ERROR;
  char *p = command;
  static int i = 0;
  
  // put your main code here, to run repeatedly:
  if (i < (BUFFER_SIZE - 1)) {  
    command[i] = c;
    i++;
  }

  // When a newline is encountered, parse and execute the command
  if (c == '\n') {
    Serial.print("Got:");
    Serial.println(command);
    command[i] = 0; // Append NULL character to string
    i = 0; // Reset write head to beginning of string.
    p = command;

    if (startswith(cmdErr, command)) {
      server.print(last_error);
      last_error = NO_ERROR;
    } else if (startswith(cmdIdn, command)) {
      server.print("Arduino,0x");
      server.print(deviceSignature,HEX);
      server.print(",0x");
      server.print(serialNumberH,HEX);
      server.print(serialNumberL,HEX);
      server.print(",");
      server.println(SW_VERSION);
    } else if (startswith(cmdDigOut, command)){
      
      p = p + sizeof(cmdDigOut)-1;
      while (*p == ' ') {
        p ++;
      }

      if ((*p != '(') | (*(p + 1) != '@')) {
        error(COMMAND_ERROR);
        return;
      }

      p+=2;

      bool state;
      int pin = *p - 'A';
      p++;
      
      if (*p != ')') {
        error(COMMAND_ERROR);
        return;
      }
      p++;
        
      if ((pin < 0) || (pin >= 4 )) {
        error(PARAMETER_ERROR);
        return;
      }
        
      while (*p == ' ') {
        p ++;
      }
      
      
      if (*p == '1') {
        state = 1;
      } else if (*p == '0') {
        state = 0;
      } else {
        error(INVALID_CHARACTER_ERROR);
        return;
      }
      
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state);
    } else if (startswith(cmdVolt, command)) {
      float value = (5.0 / 1024) * analogRead(A0);

      // Using the command buffer to print voltage
      // in scientific notation i.e. 1.352e+1
      if (dtostre(value, command, 8, 0)) {
        server.println(command);
      } else {
        error(INTERNAL_FIRMWARE_ERROR);
        return;
      }
    } else {
      error(COMMAND_ERROR);
      return;
    }
  }
}
