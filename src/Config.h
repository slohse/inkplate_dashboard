//
// Created by fraggy on 23/12/2021.
//

#ifndef INKPLATE_COMICS_CONFIG_H
#define INKPLATE_COMICS_CONFIG_H

#include <string>
#include "lib/toml.h"

class Config {
public:
    enum class Error {
        SUCCESS,
        E_NO_CFG,
        E_NO_GENERAL_SECTION,
        E_NO_MODULES
    };

    Config();
    virtual ~Config();

    Error init(std::string const & cfg_path);
    Error get_general(toml_table_t ** general);
    Error get_modules(toml_array_t ** modules);

private:
    toml_table_t * m_cfg;
    toml_table_t * m_cfg_general;
    toml_array_t * m_cfg_modules;
};




#endif //INKPLATE_COMICS_CONFIG_H
