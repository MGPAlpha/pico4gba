#include "cartridge.hpp"
#include <cstdio>
#include <cstring>
#include "picodata.hpp"
#include "print.h"

CartSection::CartSection(const char* name, const char* start, const char* end) {
    this->name = name;
    this->start = start;
    this->end = end;
    this->length = end-start;
}

CartSection::CartSection() : CartSection("", nullptr, nullptr) {}

CartSection CartSection::findSection(const char* name) const {
    CartReader reader = CartReader(*this);
    
    char sectionHeader[20];
    snprintf(sectionHeader, 19, "__%s__", name);
    int headerLen = strlen(sectionHeader);

    char buffer[256];
    while (reader.readLine(buffer, 255)) {
        mgba_printf(buffer);
    }

    return CartSection();
    
}

CartReader::CartReader(const CartSection s) {
    this->s = s;
    this->next = s.start;
    this->curr = nullptr;
}

int CartReader::readLine(char* buff, int maxLen) {
    if (!next) return 0;
    const char* source = next;
    char* dest = buff;

    for (; source < s.end && dest - buff < maxLen && *source != '\n'; source++, dest++) {
        *dest = *source;
    }

    *dest = 0;


    curr = next;
    if (source < s.end - 1) next = source + 1;
    else next = nullptr;

    return 1;
}

