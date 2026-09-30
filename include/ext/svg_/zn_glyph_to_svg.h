#include "extra/font_to_svg.h"

class ZN_GlyphToSvg
{

public:
    // static std::string to_svg(FT_Face &face, int gidx)
    // {
    //     LatexDrawGraphics::CFreeType f = LatexDrawGraphics::CFreeType(face);
    //     auto g = LatexDrawGraphics::CFreeGlypth(f, gidx);
    //     return g.outline();
    // }

    // static void to_svg(FT_Face &face, int gidx, int &x_advance, int &width, int &height, ZN_SvgBundle &svg)
    // {
    //     LatexDrawGraphics::CFreeType f = LatexDrawGraphics::CFreeType(face);
    //     LatexDrawGraphics::CFreeGlypth g = LatexDrawGraphics::CFreeGlypth(f, gidx);

    //     // printf("\nheader %i\n\noutlines %s\n\n",g._gm.height,g.outline().c_str());
    //     x_advance = g._gm.horiAdvance;
    //     height = g._gm.height;
    //     width = g._gm.width;
    //     svg.path = g.outline();
    // }

    static char *to_svg(const std::string font, const std::string text)
    {

        const ZN_BoundingBox *bbox;
        size_t len;

      
        const char *_text = text.c_str();
        const char *_font = font.c_str();

        char *out_svg;
        // zn_text_to_svg(_text, _font,  &out_svg);
        return out_svg;
    }
};