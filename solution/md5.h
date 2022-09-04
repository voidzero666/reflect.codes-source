/* Деректива препроцессора #pragma once следит за тем,
чтобы конкретный исходный файл при компиляции
подключался только один раз*/
#pragma once
#include <string>
/*«определяем» тип unsigned int как uint. Теперь везде в коде, где
будет встречаться слово uint, компилятор будет вставлять unsigned int*/
typedef unsigned int uint;

std::string to_hex(uint value);
uint F1(uint X, uint Y, uint Z);
uint G1(uint X, uint Y, uint Z);
uint H1(uint X, uint Y, uint Z);
uint I1(uint X, uint Y, uint Z);
uint rotate_left(uint value, int shift);
std::string get_md5(std::string in);