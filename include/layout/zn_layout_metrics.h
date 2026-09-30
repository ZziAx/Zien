

// class ZN_LineData
// {
// public:
//     // std::vector<ZN_GlyphData> glyphs = {};

//     Alignment alignment = Alignment::NONE;
//     int from;
//     int to;
//     float width;
//     float height;
//     float min_y;
//     float max_y;
//     int y_fix;
//     std::string content;
//     // ZN_LineData();
// };

class LayoutMetrics
{

public:
    ZN_RenderFlag render_flag = ZN_RenderFlag::BITMAP;
    Alignment alignment = Alignment::CENTER;

    FT_Face face;
    std::vector<ZN_GlyphData> glyphs;
    std::vector<ZN_LineData> lines;

    int direction = ZN_DIRECTION_NEUTRAL;

    float height;

    // requested width
    float width;

    // layout fit width
    float bound_width;

    // max possible width text fits
    float max_constraint_width;

    float get_line_x_align(ZN_LineData *line)
    {

        float x = 0;
        float w = line->width;
        // int mw = (int)get_width();
        float mw = (int)get_width();

        Alignment alignment = this->alignment;

        if (line->alignment != Alignment::NONE)
        {
            alignment = line->alignment;
        }

        switch (alignment)
        {
        case Alignment::LEFT:
            x = mw - w;
            break;
        case Alignment::CENTER:
            x = (mw - w) / 2;
            break;
        case Alignment::RIGHT:
            x = 0;
            break;

        default:
            break;
        }

        return -x;
    }

    float get_width()
    {
        return width;
    }

    float get_height(){
        return height;
    }
};