#include <Inkplate.h>
#include <SdFat.h>

#include "toml.h"

const int ERRBUFSIZE = 50;

Inkplate display(INKPLATE_1BIT);
SdFile config;

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

bool open_comics_dir() {
    char errbuf[ERRBUFSIZE];
    bool success = true;

    toml_table_t * table = toml_table_in(cfg, "comics");
    if(!table) {
        display.println("No 'comics' section in config.");
        display.partialUpdate();
        return false;
    }

    toml_datum_t dir = toml_string_in(table, "path");
    if(!dir.ok) {
        display.println("No 'path' configured for comics.");
        display.partialUpdate();
        success = false;
    }

    if(success) {
        SdFile comics_dir;

        if(!comics_dir.open(dir.u.s)) {
            sprintf(errbuf, "Could not open %s", dir.u.s);
            display.println(errbuf);
            display.partialUpdate();
            success = false;
        }
    }

    free(dir.u.s);

    return true;
}

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

    if(!open_comics_dir()) {
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


}

void loop() {
  // put your main code here, to run repeatedly:

}
