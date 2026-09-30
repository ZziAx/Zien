#include "freetype/ftoutln.h"

class ZN_Italic : public ZN_StyleObj
{

public:
    float angle;

    ZN_Italic() = default;

    ZN_Italic(float angle, bool enabled) : angle(angle), ZN_StyleObj(enabled) {}

    void set_angle(float _angle)
    {
        angle = _angle;
    }

    void apply()
        override
    {

        // return;

        double radians = angle * 3.141592653589793 / 180.0;

        // Create a shear (italic) matrix
        FT_Matrix matrix = {
            (FT_Fixed)(1 << 16),                  // x scale = 1.0
            (FT_Fixed)(tan(radians) * (1 << 16)), // x skew (italic slant)
            0,                                    // y skew = 0
            (FT_Fixed)(1 << 16)                   // y scale = 1.0
        };

        // Apply transformation to this face
        FT_Set_Transform(face, &matrix, nullptr);

        // printf("\neoriwiouwer %i\n", 0);
        // printf("applied ital %i\n",face->glyph->bitmap.width);
    }
};