

// enum class Font

class ZN_LineData
{
public:
    // std::vector<ZN_GlyphData> glyphs = {};

    Alignment alignment = Alignment::NONE;
    // int from;
    // int to;

    std::vector<ZN_GlyphData> glyphs;

    float width;
    float height;
    float min_y;
    float max_y;
    int y_fix;
    std::string content;
    // ZN_LineData();

    static void build_line(ZN_LineData &line, int line_width, int &start, int &to, int min_y, int max_y, int height, bool is_rtl = true)
    {

        // if (!is_rtl)
        // {
        //     int p_start = start;
        //     start = to;

        //     to = p_start + 1;
        // }

        line.width = line_width;
        // line.from = start;
        // line.to = to;
        // line.get_emojies();
        // line.height = round(max_y - min_y);

        line.max_y = max_y;
        line.min_y = min_y;
        // glyph_helper.get_reversed();

        line.height = height;
    }
};


