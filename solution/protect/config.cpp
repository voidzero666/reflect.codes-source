#include "config.h"
#include "protect.hpp"
#include <stdio.h>

static uint8_t g_hwid_raw[sizeof(DWORD) + 4 * sizeof(int) + MAX_COMPUTERNAME_LENGTH + 1] = {};
extern size_t hwid_raw_calc();

static const uint8_t const_key[] = { 166, 202, 168, 206, 15, 104, 94, 161, 97, 36, 201, 243, 243, 102, 223, 133, 193, 161, 51, 210, 229, 210, 100, 81, 80, 202, 30, 171, 208, 44, 57, 167, 108, 32, 194, 157, 209, 147, 169, 213, 37, 102, 253, 43, 129, 172, 153, 125, 198, 16, 213, 131, 84, 39, 192, 224, 212, 107, 99, 90, 255, 9, 105, 177, 142, 57, 72, 191, 205, 232, 103, 113, 43, 55, 218, 84, 246, 150, 9, 232, 30, 40, 209, 215, 199, 146, 112, 35, 238, 226, 150, 242, 30, 97, 181, 145, 105, 208, 217, 202, 10, 221, 157, 122, 172, 170, 214, 91, 120, 136, 167, 25, 41, 77, 80, 49, 147, 132, 149, 99, 60, 149, 155, 92, 132, 1, 130, 76, 74, 67, 64, 47, 73, 126, 67, 151, 125, 152, 68, 38, 120, 221, 142, 214, 6, 139, 18, 223, 211, 215, 195, 230, 253, 195, 169, 192, 86, 122, 51, 83, 31, 91, 189, 4, 189, 228, 46, 6, 55, 70, 20, 116, 152, 178, 227, 220, 118, 220, 171, 17, 111, 198, 133, 134, 101, 182, 29, 119, 26, 254, 14, 215, 81, 17, 67, 54, 232, 39, 190, 52, 159, 96, 50, 223, 216, 231, 253, 130, 224, 83, 16, 37, 69, 142, 90, 255, 22, 114, 112, 158, 44, 244, 196, 79, 154, 173, 242, 127, 240, 233, 52, 87, 232, 26, 36, 103, 12, 137, 166, 121, 75, 61, 84, 30, 20, 36, 14, 14, 177, 163, 90, 140, 43, 40, 124, 241, 85, 86, 158, 218, 132, 113, 194, 91, 172, 155, 78, 184, 130, 176, 8, 109, 192, 33, 184, 181, 193, 36, 82, 157, 27, 235, 113, 127, 218, 79, 42, 239, 129, 139, 73, 91, 218, 33, 31, 99, 247, 3, 126, 253, 110, 237, 81, 210, 211, 252, 85, 85, 219, 207, 69, 136, 94, 1, 124, 146, 231, 35, 35, 128, 36, 221, 6, 87, 207, 212, 234, 99, 142, 97, 56, 164, 157, 34, 108, 127, 179, 32, 72, 58, 28, 148, 12, 28, 115, 101, 102, 241, 93, 58, 210, 198, 89, 223, 44, 61, 244, 239, 232, 73, 141, 78, 20, 61, 226, 29, 45, 251, 72, 101, 146, 70, 240, 35, 217, 35, 84, 27, 55, 211, 187, 8, 63, 214, 200, 143, 29, 194, 102, 192, 1, 213, 127, 123, 103, 37, 186, 234, 216, 171, 73, 215, 85, 87, 124, 133, 133, 155, 43, 109, 230, 169, 94, 53, 42, 122, 173, 44, 87, 83, 161, 169, 54, 11, 142, 22, 198, 207, 49, 189, 94, 74, 251, 9, 10, 24, 186, 219, 72, 130, 76, 195, 137, 41, 180, 161, 5, 119, 33, 74, 129, 95, 71, 18, 54, 74, 104, 241, 121, 230, 231, 9, 240, 204, 21, 213, 235, 243, 80, 168, 64, 166, 200, 102, 220, 141, 112, 134, 21, 253, 111, 7, 182, 181, 107, 158, 97, 241, 244, 41, 118, 28, 196, 109, 233, 123, 236, 113, 111, 206, 190, 115, 131, 19, 176, 60, 201, 159, 113, 75, 89, 21, 111, 210, 233, 9, 47, 214, 251, 243, 179, 29, 183, 114, 17, 192, 84, 236, 80, 24, 116, 218, 176, 114, 120, 212, 142, 145, 249, 16, 17, 232, 33, 87, 45, 223, 19, 203, 79, 153, 77, 15, 23, 215, 9, 202, 117, 20, 92, 37, 20, 208, 75, 173, 63, 44, 132, 200, 3, 196, 211, 35, 116, 200, 199, 240, 11, 1, 74, 16, 19, 80, 83, 181, 75, 139, 23, 215, 97, 168, 167, 241, 102, 112, 74, 10, 100, 13, 120, 22, 133, 241, 52, 86, 120, 76, 249, 57, 155, 89, 158, 169, 96, 128, 241, 74, 27, 1, 195, 115, 200, 162, 141, 71, 120, 86, 215, 94, 191, 53, 80, 152, 59, 242, 176, 179, 4, 159, 127, 161, 235, 86, 70, 207, 211, 205, 169, 234, 123, 187, 61, 249, 77, 22, 136, 248, 84, 97, 98, 144, 239, 170, 212, 37, 168, 255, 75, 173, 133, 230, 55, 113, 108, 121, 2, 141, 96, 162, 221, 37, 239, 56, 232, 133, 197, 173, 62, 230, 222, 169, 126, 235, 188, 62, 51, 65, 106, 228, 143, 227, 71, 50, 122, 164, 60, 174, 154, 32, 139, 190, 63, 154, 123, 41, 244, 232, 13, 226, 188, 245, 219, 133, 69, 52, 43, 98, 121, 158, 173, 66, 240, 245, 173, 235, 249, 154, 228, 156, 202, 107, 19, 148, 59, 82, 82, 61, 172, 49, 102, 162, 41, 192, 45, 75, 199, 215, 11, 204, 41, 237, 114, 218, 159, 28, 168, 22, 103, 7, 184, 96, 40, 77, 53, 33, 32, 207, 123, 34, 236, 11, 208, 127, 101, 100, 116, 223, 48, 248, 83, 251, 95, 83, 171, 131, 74, 124, 255, 240, 105, 83, 50, 48, 250, 189, 60, 230, 152, 184, 144, 202, 158, 146, 253, 138, 64, 98, 45, 116, 59, 67, 254, 199, 219, 220, 193, 46, 169, 118, 179, 62, 219, 39, 10, 142, 47, 240, 184, 169, 119, 49, 18, 186, 41, 200, 235, 188, 89, 158, 98, 164, 150, 250, 35, 100, 252, 33, 235, 196, 11, 29, 235, 92, 217, 132, 167, 192, 79, 190, 88, 94, 63, 44, 107, 202, 117, 242, 169, 89, 187, 59, 234, 174, 34, 55, 231, 103, 189, 46, 2, 199, 59, 14, 96, 95, 108, 201, 168, 124, 159, 45, 206, 124, 23, 8, 133, 137, 91, 134, 173, 31, 136, 216, 146, 222, 170, 227, 248, 216, 11, 189, 98, 250, 67, 98, 35, 78, 172, 149, 156, 171, 74, 184, 84, 227, 74, 125, 183, 60, 31, 135, 153, 175, 209, 221, 128, 217, 96, 79, 225, 217, 191, 147, 128, 184, 245, 215, 157, 237, 51, 56, 55, 254, 78, 2, 18, 152, 5, 169, 79, 16, 42, 237, 181, 188, 52, 5, 113, 17, 62, 64, 94, 165, 66, 225, 255, 150, 5, 205, 210, 10, 142, 167, 178, 243, 109, 121, 58, 70, 71, 113, 112, 192, 186, 20, 63, 65, 236, 44, 240, 139, 67, 8, 118, 65, 107, 14, 81, 21, 235, 96, 70, 161, 98, 60 };
/*
size_t hwid_raw_calc()
{
    static size_t offset = 0;
    static const size_t hwid_raw_size = sizeof(g_hwid_raw) - 1;
    if (offset == 0)
    {
        // DWORD serial_num = 0;
        GetVolumeInformationA(crypt_str("C:\\"), NULL, 0, (PDWORD)&g_hwid_raw[offset], NULL, NULL, NULL, 0);
        offset += sizeof(DWORD);

        __cpuid((int*)&g_hwid_raw[offset], 0);
        offset += 4 * sizeof(int);

        DWORD computer_name_size = MAX_COMPUTERNAME_LENGTH + 1;
        GetComputerNameA((LPSTR)&g_hwid_raw[offset], &computer_name_size);

        for (size_t i = 0; i < hwid_raw_size; ++i)
            if (g_hwid_raw[i] == 0)
                g_hwid_raw[i] = 0xff;
    }
    return hwid_raw_size;
}
*/

static __forceinline void delta_xor_enc(uint8_t* buffer, SSIZE_T length)
{
    uint8_t last = 0;
    for (intptr_t i = 0; i < length; ++i)
    {
        uint8_t current = buffer[i];
        buffer[i] = (current - last) ^ static_cast<uint8_t>(length - i) ^ const_key[i % sizeof(const_key)];
        last = current;
    }
}

static __forceinline void delta_xor_dec(uint8_t* buffer, SSIZE_T length)
{
    uint8_t last = 0;
    for (intptr_t i = 0; i < length; ++i)
    {
        uint8_t delta = buffer[i] ^ static_cast<uint8_t>(length - i) ^ const_key[i % sizeof(const_key)];
        last = buffer[i] = delta + last;
    }
}

std::wstring c_config::SH_APPDATA;

c_config::c_config()
{
}

c_config::c_config(std::wstring dir_path, std::wstring config_name)
{
    CreateDirectoryW(dir_path.c_str(), NULL);
    this->full_path = dir_path + L"\\" + config_name;
    this->offset = 0;
    FILE* fp = _wfopen(this->full_path.c_str(), nnx::encoding::utf8to16(crypt_str("rb")).c_str());
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        SSIZE_T raw_size = ftell(fp);
        this->raw_stack.resize(raw_size);
        fseek(fp, 0, SEEK_SET);
        fread(&this->raw_stack[0], sizeof(this->raw_stack[0]), raw_size, fp);
        delta_xor_dec(&this->raw_stack[0], raw_size);
        fclose(fp);
        if (*reinterpret_cast<uint32_t*>(&this->raw_stack[raw_size - sizeof(uint32_t)]) == C_CONFIG_MAGIC)
            this->offset = raw_size - sizeof(uint32_t);
        this->raw_stack.resize(this->offset);
    }
}

void c_config::save()
{
    FILE* fp = _wfopen(this->full_path.c_str(), nnx::encoding::utf8to16(crypt_str("wb")).c_str());
    auto raw_stack_clone = this->raw_stack;
    size_t raw_stack_clone_size = raw_stack_clone.size();
    raw_stack_clone.resize(raw_stack_clone_size + sizeof(uint32_t));
    *reinterpret_cast<uint32_t*>(&raw_stack_clone[raw_stack_clone_size]) = C_CONFIG_MAGIC;
    raw_stack_clone_size = raw_stack_clone.size();
    delta_xor_enc(&raw_stack_clone[0], raw_stack_clone_size);
    fwrite(&raw_stack_clone[0], sizeof(raw_stack_clone[0]), raw_stack_clone_size, fp);
    fclose(fp);
}

int c_config::rm()
{
    return _wremove(this->full_path.c_str());
}

void c_config::push(int32_t value)
{
    uintptr_t new_offset = this->offset + sizeof(value);
    this->raw_stack.resize(new_offset);
    *reinterpret_cast<int32_t*>(&this->raw_stack[this->offset]) = value;
    this->offset = new_offset;
}

void c_config::push(bool value)
{
    uintptr_t new_offset = this->offset + sizeof(value);
    this->raw_stack.resize(new_offset);
    *reinterpret_cast<bool*>(&this->raw_stack[this->offset]) = value;
    this->offset = new_offset;
}

void c_config::push(size_t value)
{
    uintptr_t new_offset = this->offset + sizeof(value);
    this->raw_stack.resize(new_offset);
    *reinterpret_cast<size_t*>(&this->raw_stack[this->offset]) = value;
    this->offset = new_offset;
}

void c_config::push(const std::string& value)
{
    size_t value_len = value.length();
    this->raw_stack.insert(this->raw_stack.begin() + this->offset, value.begin(), value.end());
    this->offset += value_len;
    this->push(value_len);
}

int32_t c_config::pop_int32_t()
{
    this->offset -= sizeof(int32_t);
    if (this->offset < 0)
        return false;
    return *reinterpret_cast<int32_t*>(&this->raw_stack[this->offset]);
}

bool c_config::pop_bool()
{
    this->offset -= sizeof(bool);
    if (this->offset < 0)
        return false;
    return *reinterpret_cast<bool*>(&this->raw_stack[this->offset]);
}

size_t c_config::pop_size_t()
{
    this->offset -= sizeof(size_t);
    if (this->offset < 0)
        return 0;
    return *reinterpret_cast<size_t*>(&this->raw_stack[this->offset]);
}

std::string c_config::pop_string()
{
    size_t str_len = this->pop_size_t();
    this->offset -= str_len;
    if (this->offset < 0)
        return 0;
    const char* ret = reinterpret_cast<const char*>(&this->raw_stack[this->offset]);
    return { ret, &ret[str_len] };
}

void c_config::reset_offset()
{
    this->offset = this->raw_stack.size();
}