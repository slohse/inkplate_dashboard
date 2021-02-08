#include <Inkplate.h>
#include <SdFat.h>
#include <string>
#include <vector>

#include "src/lib/toml.h"
#include "src/shared_consts.h"
#include "src/modules/Comics.h"

Inkplate display(INKPLATE_1BIT);
SdFile config;
Comics cmx;

toml_table_t * cfg;

bool read_config() {
    char errbuf[ERRBUFSIZE];

    if (!config.open("/config.toml")) {
        display.println("Could not open config.toml.");
        display.partialUpdate();
        return false;
    }

    char * config_buf = new char[config.fileSize() + 1];
    config_buf[config.fileSize()] = '\0';

    if(!config_buf) {
        display.println("Failed to allocate memory for config.");
        display.partialUpdate();
        return false;
    }

    int16_t bytes_read = config.read(config_buf, config.fileSize());

    if(bytes_read <= 0) {
        display.println("Failed to read config.");
        display.partialUpdate();
        return false;
    }

    // TODO: call toml_free at an appropriate time
    cfg = toml_parse(config_buf, errbuf, ERRBUFSIZE);

    if(!cfg) {
        display.println("Failed to parse config.");
        display.partialUpdate();
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
    display.clean(); // clear screen
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

    if(!cmx.setup(display, cfg)) {
        display.println("open_comics_dir failed.");
        display.partialUpdate();
        while(true) {
            // nop
        }
    }

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
