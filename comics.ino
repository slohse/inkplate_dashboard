#include <Inkplate.h>
#include <SdFat.h>

Inkplate display(INKPLATE_1BIT);
SdFile config;

void setup() {
    display.begin();
    display.clearDisplay(); // clear buffer
    display.clean(); // clear screen
    display.display();

    //display.setTextColor(0,7);
    display.setCursor(0, 5);
    display.setTextSize(2);

    display.println("booting...");
    display.partialUpdate();

    // Init SD card. Handle if SD card could not be accessed
    if (display.sdCardInit())
    {
        display.println("SD Card OK.");
    }
    else
    {
        display.println("SD Card error.");
        display.partialUpdate();
        while (true) {
            // nop 
        }
    }
    display.partialUpdate();

    if (!config.open("/config.toml")) {
        display.println("Could not open config.toml.");
        display.partialUpdate();
        while (true) {
            // nop
        }
    }
    //display.display();

    delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

}
