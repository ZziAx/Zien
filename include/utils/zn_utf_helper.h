#include <sstream>
#include <stdexcept>

/**
 * @brief Naive detection of character direction based on Unicode ranges.
 *
 * This function currently handles basic Hebrew, Arabic, and Latin ranges.
 * @param c Unicode codepoint
 * @return ZN_DIRECTION Returns RTL, LTR, or NEUTRAL
 */
ZN_DIRECTION char_direction(char32_t c)
{
    if ((c >= 0x05BE && c <= 0x10B7F) || // Hebrew, Arabic, etc.
        (c >= 0x0600 && c <= 0x06FF) ||
        (c >= 0x0750 && c <= 0x077F) ||
        (c >= 0x08A0 && c <= 0x08FF))
    {
        return ZN_DIRECTION_RTL;
    }
    if ((c >= 0x0041 && c <= 0x005A) || // Latin uppercase
        (c >= 0x0061 && c <= 0x007A) || // Latin lowercase
        (c >= 0x0030 && c <= 0x0039))   // digits
    {
        return ZN_DIRECTION_LTR;
    }
    return ZN_DIRECTION_NEUTRAL;
}

/**
 * @brief Utility class for UTF-8 encoding/decoding, string manipulation, and text direction.
 */
class ZN_UtfHelper
{
public:
    /**
     * @brief Convert a Unicode codepoint to uppercase hexadecimal string.
     * @param codepoint Unicode codepoint
     * @return std::string Hex representation
     */
    static std::string to_hex(int codepoint)
    {
        std::stringstream ss;
        ss << std::hex << std::uppercase << codepoint;
        return ss.str();
    }

    /**
     * @brief Reverse a C-style string.
     * @param input Null-terminated input string
     * @return char* Newly allocated reversed string (caller must free)
     */
    static char *get_reversed(const char *input)
    {
        size_t len = std::strlen(input);
        char *reversed = new char[len + 1];
        for (size_t i = 0; i < len; ++i)
        {
            reversed[i] = input[len - 1 - i];
        }
        reversed[len] = '\0';
        return reversed;
    }

    /**
     * @brief Convert a UTF-8 encoded string to a single Unicode codepoint.
     * @param s Pointer to UTF-8 string
     * @return char32_t Unicode codepoint
     */
    static char32_t utf8_to_char32(const char *s)
    {
        unsigned char c0 = s[0];
        if (c0 < 0x80)
            return c0; // 1-byte ASCII
        else if ((c0 >> 5) == 0x6)
            return ((s[0] & 0x1F) << 6) | (s[1] & 0x3F); // 2-byte
        else if ((c0 >> 4) == 0xE)
            return ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); // 3-byte
        else if ((c0 >> 3) == 0x1E)
            return ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F); // 4-byte
        return 0;                                                                                        // invalid UTF-8
    }

    /**
     * @brief Determine direction (LTR/RTL) of the first character in a UTF-8 string.
     * @param text UTF-8 string
     * @return int Direction enum value
     */
    static int get_direction(const char *text)
    {
        char *first_char = utf8_substr(text, 0, 1);
        // printf("hhh '%s'",first_char);
        // char *first_char = utf8_substr(text, std::string(text).size(), std::string(text).size()-1);
        int dir = char_direction(utf8_to_char32(first_char));
        free(first_char);
        return dir;
    }

    /**
     * @brief Split a string by spaces into a vector of tokens.
     * @param str Input string
     * @return std::vector<std::string> Tokens
     */
    static std::vector<std::string> split(const std::string &str)
    {
        std::vector<std::string> tokens;
        size_t start = 0, end;
        while ((end = str.find(' ', start)) != std::string::npos)
        {
            if (end != start)
                tokens.push_back(str.substr(start, end - start));
            start = end + 1;
        }
        if (start < str.size())
            tokens.push_back(str.substr(start));
        return tokens;
    }

    /**
     * @brief Convert a Unicode codepoint to a UTF-8 string.
     * @param cp Codepoint
     * @return std::string UTF-8 encoded string
     */
    static std::string codepoint_to_utf8(uint32_t cp)
    {
        std::string out;
        if (cp <= 0x7F)
            out.push_back(static_cast<char>(cp));
        else if (cp <= 0x7FF)
        {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0x10FFFF)
        {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        return out;
    }

    /**
     * @brief Count the number of UTF-8 characters in a string.
     * @param s UTF-8 string
     * @return int Character count
     */
    static int utf8_char_count(const char *s)
    {
        int count = 0;
        const unsigned char *p = (const unsigned char *)s;
        while (*p)
        {
            if ((*p & 0x80) == 0)
                p += 1;
            else if ((*p & 0xE0) == 0xC0)
                p += 2;
            else if ((*p & 0xF0) == 0xE0)
                p += 3;
            else if ((*p & 0xF8) == 0xF0)
                p += 4;
            else
                p++;
            count++;
        }
        return count;
    }

    /**
     * @brief Get the next Unicode codepoint from a UTF-8 string at a given index.
     * @param s UTF-8 string
     * @param i Reference index (updated after reading codepoint)
     * @return int Codepoint
     */
    static int utf8_to_codepoint(const std::string &s, size_t &i)
    {
        unsigned char c = s[i];
        if (c < 0x80)
            return s[i++]; // 1-byte
        else if ((c & 0xE0) == 0xC0)
        {
            int cp = ((c & 0x1F) << 6) | (s[i + 1] & 0x3F);
            i += 2;
            return cp;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            int cp = ((c & 0x0F) << 12) | ((s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F);
            i += 3;
            return cp;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            int cp = ((c & 0x07) << 18) | ((s[i + 1] & 0x3F) << 12) | ((s[i + 2] & 0x3F) << 6) | (s[i + 3] & 0x3F);
            i += 4;
            return cp;
        }
        i++; // invalid
        return -1;
    }

    /**
     * @brief Get the next codepoint from UTF-8 string.
     * @param text UTF-8 string
     * @param bytes_consumed Optional pointer to store number of bytes read
     * @return uint32_t Codepoint
     */
    static uint32_t next_codepoint(const char *text, int *bytes_consumed = nullptr)
    {
        const unsigned char *s = (const unsigned char *)text;
        uint32_t codepoint = 0;
        int bytes = 0;

        if (s[0] < 0x80)
        {
            codepoint = s[0];
            bytes = 1;
        }
        else if ((s[0] & 0xE0) == 0xC0)
        {
            codepoint = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
            bytes = 2;
        }
        else if ((s[0] & 0xF0) == 0xE0)
        {
            codepoint = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
            bytes = 3;
        }
        else if ((s[0] & 0xF8) == 0xF0)
        {
            codepoint = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
            bytes = 4;
        }

        if (bytes_consumed)
            *bytes_consumed = bytes;
        return codepoint;
    }

    /**
     * @brief Extract a single UTF-8 character starting at a byte offset.
     * @param text UTF-8 string
     * @param byte_offset Start offset in bytes
     * @return std::string UTF-8 character
     */
    static std::string get_utf8_char(const char *text, int byte_offset)
    {
        const unsigned char *ptr = (const unsigned char *)(text + byte_offset);
        int len = 1;
        if ((ptr[0] & 0xF8) == 0xF0)
            len = 4;
        else if ((ptr[0] & 0xF0) == 0xE0)
            len = 3;
        else if ((ptr[0] & 0xE0) == 0xC0)
            len = 2;
        return std::string((const char *)ptr, len);
    }

    /**
     * @brief Return a string with only visible text (skipping directional marks).
     * @param s UTF-8 string
     * @return std::string Filtered string
     */

    static std::string visible_text(const std::string &s)
    {
        std::string result;
        auto it = s.begin();
        while (it != s.end())
        {
            char32_t cp = utf8::next(it, s.end()); // skip directional marks
            if (cp != 0x2066 && cp != 0x2067 && cp != 0x2069)
            {
                utf8::append(cp, std::back_inserter(result));
            }
        }
        return result;
    }

    /**
     * @brief Returns the Unicode codepoint of the visible character at the given index.
     *
     * Skips directional formatting marks (like LTR/RTL controls) when counting characters.
     *
     * @param s UTF-8 encoded string
     * @param index Index of the visible character to retrieve
     * @return int Unicode codepoint, or `ZN_ERR_OUT_OF_RANGE` if index is invalid
     */

    static int char_at(const std::string &s, size_t index)
    {

        auto it = s.begin();
        size_t i = 0;
        char32_t cp = 0;
        while (it != s.end())
        {
            cp = utf8::next(it, s.end());
            // skip directional marks if needed
            if (cp != 0x2066 && cp != 0x2067 && cp != 0x2069)
            {
                if (i == index)
                {
                    int codepoint = static_cast<int>(cp);
                    return codepoint;
                }
                i++;
            }
        }

        return ZN_ERR_OUT_OF_RANGE;
    }

    /**
     * @brief Convert a single char32_t codepoint to C-style null-terminated string.
     * @param c Codepoint
     * @return char* Pointer to buffer (stack-allocated, max 5 bytes)
     */
    static char *char32_to_cstr(char32_t c)
    {
        static char buf[5] = {0};
        if (c <= 0x7F)
            buf[0] = c;
        else if (c <= 0x7FF)
        {
            buf[0] = 0xC0 | ((c >> 6) & 0x1F);
            buf[1] = 0x80 | (c & 0x3F);
        }
        else if (c <= 0xFFFF)
        {
            buf[0] = 0xE0 | ((c >> 12) & 0x0F);
            buf[1] = 0x80 | ((c >> 6) & 0x3F);
            buf[2] = 0x80 | (c & 0x3F);
        }
        else if (c <= 0x10FFFF)
        {
            buf[0] = 0xF0 | ((c >> 18) & 0x07);
            buf[1] = 0x80 | ((c >> 12) & 0x3F);
            buf[2] = 0x80 | ((c >> 6) & 0x3F);
            buf[3] = 0x80 | (c & 0x3F);
        }
        return buf;
    }

    /**
     * @brief Get size in bytes of a UTF-8 character.
     * @param c Pointer to UTF-8 char
     * @return int Byte size
     */
    static int utf8_char_size(const unsigned char *c)
    {
        if ((*c & 0x80) == 0)
            return 1;
        else if ((*c & 0xE0) == 0xC0)
            return 2;
        else if ((*c & 0xF0) == 0xE0)
            return 3;
        else if ((*c & 0xF8) == 0xF0)
            return 4;
        return 1; // fallback
    }

    /**
     * @brief Extract a UTF-8 substring from a C-style string.
     * @param src Source string
     * @param start Start index in characters
     * @param length Length in characters
     * @return char* Newly allocated substring (caller must free)
     */
    static char *utf8_substr(const char *src, int start, int length)
    {
        const unsigned char *s = (const unsigned char *)src;
        int char_count = 0;
        const unsigned char *p = s;

        while (*p && char_count < start)
        {
            if ((*p & 0x80) == 0)
                p += 1;
            else if ((*p & 0xE0) == 0xC0)
                p += 2;
            else if ((*p & 0xF0) == 0xE0)
                p += 3;
            else if ((*p & 0xF8) == 0xF0)
                p += 4;
            else
                p++;
            char_count++;
        }
        const unsigned char *start_ptr = p;

        while (*p && char_count < start + length)
        {
            if ((*p & 0x80) == 0)
                p += 1;
            else if ((*p & 0xE0) == 0xC0)
                p += 2;
            else if ((*p & 0xF0) == 0xE0)
                p += 3;
            else if ((*p & 0xF8) == 0xF0)
                p += 4;
            else
                p++;
            char_count++;
        }
        const unsigned char *end_ptr = p;

        int byte_len = end_ptr - start_ptr;
        char *result = (char *)malloc(byte_len + 1);
        memcpy(result, start_ptr, byte_len);
        result[byte_len] = '\0';
        return result;
    }



    
};
