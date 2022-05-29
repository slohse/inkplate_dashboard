//
// Created by fraggy on 23/12/2021.
//

#include "Config.h"
#include "util.h"

Config::Config() {

}

Config::~Config() {
    toml_free(m_cfg);
}

Config::Error Config::init(std::string const & cfg_path) {
    m_cfg = parse_toml_from_sd(cfg_path);

    if(!m_cfg) {
        return Error::E_NO_CFG;
    }

    m_cfg_general = toml_table_in(m_cfg, "general");
    m_cfg_modules = toml_array_in(m_cfg_general, "modules");

    return Error::SUCCESS;
}

Config::Error Config::get_general(toml_table_t ** general) {
    if(!m_cfg_general) {
        return Error::E_NOT_FOUND;
    }

    *general = m_cfg_general;

    return Error::SUCCESS;
}

Config::Error Config::get_modules(toml_array_t ** modules) {
    if(!m_cfg_modules) {
        return Error::E_NOT_FOUND;
    }

    *modules = m_cfg_modules;

    return Error::SUCCESS;
}

Config::Error Config::get_wifi_ssid(std::string & ssid) {
    if(!m_cfg_general) {
        return Error::E_NOT_FOUND;
    }

    toml_datum_t toml_ssid = toml_string_in(m_cfg_general, "wifi_ssid");

    if(!toml_ssid.ok) {
        SERIAL_LOG("No 'wifi_ssid' configured.");
        return Error::E_NOT_FOUND;
    }
    ssid = toml_ssid.u.s;

    free(toml_ssid.u.s);
    return Error::SUCCESS;
}

Config::Error Config::get_wifi_password(std::string & password) {
    if(!m_cfg_general) {
        return Error::E_NOT_FOUND;
    }

    toml_datum_t toml_password = toml_string_in(m_cfg_general, "wifi_password");

    if(!toml_password.ok) {
        SERIAL_LOG("No 'wifi_password' configured.");
        return Error::E_NOT_FOUND;
    }
    password = toml_password.u.s;

    free(toml_password.u.s);
    return Error::SUCCESS;
}
