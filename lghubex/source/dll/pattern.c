#include <lghubex/dll/pattern.h>

#include <Windows.h>

// IMPORTANT: no std allowed in this file
// code are executed in the injected context where CRT is not initialized.

uint32_t GetTextSection(uintptr_t module_base, const uint8_t **text_section_base, size_t *text_section_size)
{
    if (module_base == 0 || text_section_base == NULL || text_section_size == NULL) {
        return ERROR_INVALID_PARAMETER;
    }
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)module_base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return ERROR_INVALID_EXE_SIGNATURE;
    }
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(module_base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return ERROR_INVALID_EXE_SIGNATURE;
    }

    IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    if (section == NULL) {
        return ERROR_BAD_EXE_FORMAT;
    }

    for (uint16_t i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (memcmp(section->Name, ".text", sizeof(".text")) == 0) {
            *text_section_base = (uint8_t*)(module_base + section->VirtualAddress);
            *text_section_size = section->Misc.VirtualSize;
            return ERROR_SUCCESS;
        }
        ++section;
    }
    return ERROR_BAD_EXE_FORMAT;
}

const uint8_t *SearchPattern(const uint8_t *string, size_t string_size, const int16_t *pattern, size_t pattern_size)
{
    if (string == NULL || pattern == NULL || string_size == 0 || pattern_size == 0) {
        return NULL;
    }
    const uint8_t *address = NULL;
    // X X X X X
    // ^i      ^j
    size_t i = 0; // base
    size_t j = 0; // lookahead
    while (j < string_size && j - i < pattern_size) {
        size_t t = j - i;
        int16_t p = pattern[t];
        if (p < 0 || string[j] == p) {
            ++j;
        } else {
            ++i;
            j = i;
        }
        if (j - i == pattern_size) {
            address = string + i;
            break;
        }
    }
    return address;
}