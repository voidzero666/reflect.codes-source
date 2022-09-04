#include "KeyValues.h"
#include "Hooks.h"

KeyValues* KeyValues::fromString(const char* name, const char* value) noexcept
{
    const auto keyValuesFromString = csgo->keyValuesFromString;
    KeyValues* keyValues;
    __asm {
        push 0
        mov edx, value
        mov ecx, name
        call keyValuesFromString
        add esp, 4
        mov keyValues, eax
    }
    return keyValues;
}

KeyValues* KeyValues::findKey(const char* keyName, bool create) noexcept
{
    return csgo->keyValuesFindKey(this, keyName, create);
}

void KeyValues::setString(const char* keyName, const char* value) noexcept
{
    if (const auto key = findKey(keyName, true))
        csgo->keyValuesSetString(key, value);
}
