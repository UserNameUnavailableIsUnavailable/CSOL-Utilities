#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

///@brief Retrieves the base address and size of the .text section of a module.
///@param module_base The base address of the module.
///@param text_section_base Pointer to receive the base address of the .text section.
///@param text_section_size Pointer to receive the size of the .text section.
///@return ERROR_SUCCESS if successful, otherwise a Win32 error code.
uint32_t GetTextSection(uintptr_t module_base, const uint8_t **text_section_base, size_t *text_section_size);

///@brief Searches for specified byte pattern in the given string.
///@param string The string to search within.
///@param string_size The size of the string.
///@param pattern The byte pattern to search for. If byte is negative, it will be treated as a wildcard.
///@param pattern_size The size of the pattern.
///@return Pointer to the start of the pattern if found, otherwise NULL.
const uint8_t *SearchPattern(
    const uint8_t *string,
    size_t string_size,
    const int16_t *pattern,
    size_t pattern_size
);

#ifdef __cplusplus
}
#endif