#include "freetype/ftoutln.h"
#include "freetype/ftglyph.h";
#include "freetype/ftstroke.h";

class ZN_Stroker : public ZN_StyleObj
{

public:

    std::vector<ZN_Stroke> strokes;

    float width;

    ZN_Stroker() = default;

    ZN_Stroker(float width, bool enabled) : width(width), ZN_StyleObj(enabled) {}

    void set_width(float _width)
    {
        width = _width;
    }

    void add_stroke(ZN_Stroke stroke)
    {
        strokes.push_back(stroke);
    }

    void add_stroke(float width, bool enabled = true)
    {
        ZN_Stroke stroke = ZN_Stroke(width, enabled);
        strokes.push_back(stroke);
    }
    void apply()
        override
    {
        // return;
        // FT_Library ft = ZN_EngineContext::get_ft();

        // FT_Stroker stroker;
        // FT_Stroker_New(ft, &stroker);
        // FT_Stroker_Set(stroker, 64 * width, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

        // FT_Glyph glyph;
        // FT_Get_Glyph(face->glyph, &glyph);

        // FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
    }
};