#include <Inkplate.h>
#include <SdFat.h>
#include <string>
#include <vector>

#include "src/util.h"
#include "src/Config.h"
#include "src/lib/toml.h"
#include "src/shared_consts.h"
#include "src/modules/ViewManager.h"

Inkplate display(INKPLATE_1BIT);
SdFile config;
ViewManager viewMan;

Config cfg;

const unsigned long WIFI_CONNECT_TIMEOUT_MS = 30000;

bool read_config() {
    char errbuf[ERRBUFSIZE];

    std::string config_path("/config.toml");

    if(cfg.init(config_path) != Config::Error::SUCCESS) {
        return false;
    }

    return true;
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    SERIAL_LOG("Serial Monitor Initialized");

    display.begin();
    display.clearDisplay(); // clear buffer
    //display.clean(0, 1); // clear screen
    display.display();

    //display.setTextColor(0,7);
    display.setCursor(0, 5);
    display.setTextSize(2);

    display.println("booting...");
    SERIAL_LOG("booting...");
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

    toml_table_t * cfg_general = nullptr;
    toml_array_t * cfg_modules = nullptr;
    if (Config::Error::SUCCESS != cfg.get_general(&cfg_general)) {
        display.println("Config has no 'general' section.");
        display.partialUpdate();
        while(true) {
            // nop
        }
    }

    if (Config::Error::SUCCESS != cfg.get_modules(&cfg_modules)) {
        display.println("Config's 'general' section has no 'modules'.");
        display.partialUpdate();
        while(true) {
            // nop
        }
    }

    std::string wifi_ssid;
    std::string wifi_password;
    if (Config::Error::SUCCESS != cfg.get_wifi_ssid(wifi_ssid)
        || Config::Error::SUCCESS != cfg.get_wifi_password(wifi_password)) {
        display.println("No WiFi config, skipping connecting to WiFi.");
        display.partialUpdate();
    } else {
        unsigned long wifi_connect_start_time = millis();
        while(!display.connectWiFi(wifi_ssid.c_str(), wifi_password.c_str()))
        {
            Serial.println("Connecting to WiFi...");
            if(millis() - wifi_connect_start_time > WIFI_CONNECT_TIMEOUT_MS) {
                Serial.println("WiFi connect timeout reached, aborting...");
                break;
            }
        }
        delay(1000);
        if(display.isConnected())
        {
            Serial.println("Connected to WiFi.");
            display.println("Connected to WiFi.");
        } else {
            Serial.println("Could not connect to WiFi.");
            display.println("Could not connect to WiFi.");
        }
    }

    viewMan.init(display, cfg_modules);

    display.println("ViewManager initialized.");
    display.partialUpdate();

    //display.display();

    delay(1000);

    viewMan.draw();


}

void loop() {
    if(display.readTouchpad(PAD1)) {
        SERIAL_LOG("Left Button");
        viewMan.leftButton();
    }
    if(display.readTouchpad(PAD2)) {
        SERIAL_LOG("Center Button");
        viewMan.centerButton();
    }
    if(display.readTouchpad(PAD3)) {
        SERIAL_LOG("Right Button");
        viewMan.rightButton();
    }

    delay(100);

}
