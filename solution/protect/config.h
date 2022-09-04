#pragma once

#include <shlobj.h>
#include <stdio.h>

#include <fstream>
#include <string>
#include <vector>

#define C_CONFIG_MAGIC 0x43464700

class c_config
{
protected:
    std::wstring full_path;
    std::vector<uint8_t> raw_stack;
public:
    intptr_t offset;
    static std::wstring SH_APPDATA;
    c_config();
    c_config(std::wstring dir_path, std::wstring config_name);
    void save();
    int rm();
    void push(int32_t value);
    void push(bool value);
    void push(size_t value);
    void push(const std::string& value);
    int32_t pop_int32_t();
    bool pop_bool();
    size_t pop_size_t();
    std::string pop_string();
    void reset_offset();
};
