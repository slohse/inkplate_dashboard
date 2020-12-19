#include <Inkplate.h>
#include <SdFat.h>

Inkplate display(INKPLATE_3BIT);
SdFile file;

void setup() {
    display.begin();
    display.clearDisplay(); // clear buffer
    display.clean(); // clear screen
    display.display();

    display.setTextColor(0,7);
    display.setCursor(0, 5);
    display.setTextSize(2);

    display.println("booting...");

    // Init SD card. Handle if SD card could not be accessed
    if (display.sdCardInit())
    {
        display.println("SD Card OK.");
    }
    else
    {
        display.println("SD Card error.");
        display.display();
        while (true) {
            // nop 
        }
    }
    display.display();
    delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

}
