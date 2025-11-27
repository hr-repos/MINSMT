// Put external dependencies here:
#include <Arduino.h>

// Put internal dependencies here:
#include "testlib.h"

// put function declarations here:
int myFunction(int, int);

void setup()
{
    Serial.begin(115200);

    // Met deze loop wacht de code tot de seriele monitor verbonden is voordat deze verder gaat, handig voor debuggen.
    while (!Serial)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }

    pinMode(LED_BUILTIN, OUTPUT);
    // put your setup code here, to run once:
    int result = myFunction(2, 3);
}

void loop()
{
    // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y)
{
    return x + y;
}