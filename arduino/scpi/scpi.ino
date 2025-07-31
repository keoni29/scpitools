/**
    Basic SCPI command interpreter. Tested using Arduino Pro Micro.

    MIT License

    Copyright (c) 2025 Koen van Vliet

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */
#include <avr/boot.h>

#define SW_VERSION "1.0"

#define BUFFER_SIZE 256
#define LED_PIN 10

#define error(e) last_error = e; errorBeep(); Serial.println(e);

typedef enum{
  NO_ERROR = 0,
  COMMAND_ERROR = -100,
  INVALID_CHARACTER_ERROR = -101,
  PARAMETER_ERROR = -104,
  INTERNAL_FIRMWARE_ERROR = -300,
}Error;

const char cmdIdn[] = "*IDN?";
const char cmdVolt[] = "MEAS:VOLT:DC?";
const char cmdErr[] = "SYST:ERR?";
static const char cmdDigOut[] = "DIG:OUT";

uint32_t deviceSignature; // Unique to the type of microcontroller used
uint32_t serialNumberL;
uint32_t serialNumberH;

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
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  getDeviceSignature();
  getSerialNumber();
}

void loop() {
  static char command[BUFFER_SIZE];
  static int16_t last_error = NO_ERROR;
  char *p = command;
  char c;
  int i;
  
  // put your main code here, to run repeatedly:
  while (Serial.available()){
    if (i < (BUFFER_SIZE - 1)) {
      c = Serial.read();
      command[i] = c;
      i++;
    }

    // When a newline is encountered, parse and execute the command
    if (c == '\n') {
      command[i] = 0; // Append NULL character to string
      i = 0; // Reset write head to beginning of string.
      p = command;

      if (startswith(cmdErr, command)) {
        Serial.println(last_error);
        last_error = NO_ERROR;
      } else if (startswith(cmdIdn, command)) {
        Serial.print("Arduino,0x");
        Serial.print(deviceSignature,HEX);
        Serial.print(",0x");
        Serial.print(serialNumberH,HEX);
        Serial.print(serialNumberL,HEX);
        Serial.print(",");
        Serial.println(SW_VERSION);
      } else if (startswith(cmdDigOut, command)){
        
        p = p + sizeof(cmdDigOut)-1;
        while (*p == ' ') {
          p ++;
        }

        if ((*p != '(') | (*(p + 1) != '@')) {
          Serial.println("no @ or ()");
          error(COMMAND_ERROR);
          break;
        }

        p+=2;

        bool state;
        int pin = *p - 'A';
        p++;
        
        if (*p != ')') {
          Serial.println("no )");
          error(COMMAND_ERROR);
          break;
        }
        p++;
          
        if ((pin < 0) || (pin >= 4 )) {
          Serial.println("pin no not ok");
          error(PARAMETER_ERROR);
          break;
        }
          
        while (*p == ' ') {
          p ++;
        }
        
        
        if (*p == '1') {
          state = 1;
        } else if (*p == '0') {
          state = 0;
        } else {
          Serial.println("invalid state");
          error(INVALID_CHARACTER_ERROR);
          break;
        }
        
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
      } else if (startswith(cmdVolt, command)) {
        float value = (5.0 / 1024) * analogRead(A0);
        
        if (dtostre(value, command, 8, 0)) {
          Serial.println(command);
        } else {
          error(INTERNAL_FIRMWARE_ERROR);
          break;
        }
      } else {
        error(COMMAND_ERROR);
        break;
      }
    }
  }
}