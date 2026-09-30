#include "freetype/ftoutln.h"

class ZN_Bold : public ZN_StyleObj
{

public:
    float strength;

    ZN_Bold() = default;

    ZN_Bold(float strength, bool enabled) : strength(strength), ZN_StyleObj(enabled) {}

    void set_strength(float _strength)
    {
        strength = _strength;
    }

    void apply()
        override
    {
        FT_Outline_Embolden(&face->glyph->outline, strength); // e.g. 64 = 1px in 26.6 fixed
    }
};