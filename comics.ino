#include <Inkplate.h>
#include <SdFat.h>
#include <string>
#include <vector>

#include "src/util.h"
#include "src/Config.h"
#include "src/lib/toml.h"
#include "src/shared_consts.h"
#include "src/modules/Comics.h"

Inkplate display(INKPLATE_1BIT);
SdFile config;
Comics cmx;

Config cfg;

bool read_config() {
    char errbuf[ERRBUFSIZE];

    std::string config_path("/config.toml");

    if(cfg.init(config_path) != Config::Error::SUCCESS) {
        return false;
    }

    return true;
}

void setup() {
    Serial.begin(921600);
    while (!Serial) {
        ;
    }
    Serial.println("Serial Monitor Initialized");

    display.begin();
    display.clearDisplay(); // clear buffer
    //display.clean(0, 1); // clear screen
    display.display();

    //display.setTextColor(0,7);
    display.setCursor(0, 5);
    display.setTextSize(2);

    display.println("booting...");
    Serial.println("booting...");
    display.partialUpdate();

    // Init SD card. Handle if SD card could not be accessed
    if (!display.sdCardInit())
    {
        display.println("SD Card error.");
        display.partialUpdate();
        while (true) {
            // nop 
        }
    }

    display.println("SD Card OK.");
    display.partialUpdate();

    if(!read_config()) {
        display.println("read_config failed.");
        display.partialUpdate();
        while(true) {
            // nop
        }
    }

    display.println("Config OK.");
    display.partialUpdate();

    // TODO: initialize modules here

    display.println("Comics config OK.");
    display.partialUpdate();

    //display.display();

    delay(1000);

    cmx.resume();


}

void loop() {
    if(display.readTouchpad(PAD1)) {
        Serial.println("Left Button");
        cmx.leftButton();
    }
    if(display.readTouchpad(PAD2)) {
        Serial.println("Center Button");
    }
    if(display.readTouchpad(PAD3)) {
        Serial.println("Right Button");
        cmx.rightButton();
    }

    delay(100);

}
