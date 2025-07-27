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
#define BUFFER_SIZE 256
#define LED_PIN 9

const char cmdIdn[] = "*IDN?";
const char cmdVolt[] = "MEAS:VOLT:DC?";
const char cmdErr[] = "SYST:ERR?";

typedef enum{
  NO_ERROR = 0,
  COMMAND_ERROR = -100,
  INTERNAL_FIRMWARE_ERROR = -300,
}Error;

void errorBeep(void){
  /// TODO * loud beep noise *
  int i;

  for (i = 0; i < 10; i++) {
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
}

void loop() {
  static char command[BUFFER_SIZE];
  static int16_t last_error = NO_ERROR;
  char *p = command;
  int i;
  
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    while (Serial.available()) {
      *p = Serial.read(); 
      p++;
    }
    *p = 0; // Terminate the string

    if (startswith(cmdErr, command)) {
      Serial.println(last_error);
      last_error = NO_ERROR;
    } else if (startswith(cmdIdn, command)) {
      Serial.println("Arduino,Pro Micro,serial,hwversion-swversion");
    } else if (startswith(cmdVolt, command)) {
      float value = (5.0 / 1024) * analogRead(A0);
      
      if (dtostre(value, command, 8, 0)) {
        Serial.println(command);
      } else {
        last_error = INTERNAL_FIRMWARE_ERROR;
        errorBeep();
      }
    } else {
      last_error = COMMAND_ERROR;
      errorBeep();
    }
  }
}