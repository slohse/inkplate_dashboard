#include <Inkplate.h>
#include <SdFat.h>

Inkplate display(INKPLATE_3BIT);
SdFile file;

void setup() {
    display.begin();
    display.clearDisplay(); // clear buffer
    display.clean(); // clear screen

    // Init SD card. Handle if SD card could not be accessed
    if (display.sdCardInit())
    {
        display.println("SD Card OK!");
        display.partialUpdate();
    }
    else
    {
        display.println("SD Card error!");
        display.partialUpdate();
        while (true) {
            // nop 
        }
    }
    delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

}
