
#define SPLIT_BY_WORD 0

#define SPLIT_BY_CHARACTER 1

float MAXWIDTH = -1;
float height = 0;

class ZN_TextLayout
{
private:
    int split_mode = SPLIT_BY_CHARACTER;

    std::vector<ZN_GlyphData> glyphs;

    float width = MAXWIDTH;

    GlyphHelper glyph_helper = GlyphHelper();

    void _generate_lines_by_word(std::vector<ZN_LineData> &lines)
    {

        ZN_DIRECTION direction = metrics.direction;

        std::vector<WordData> words;

        bool is_rtl = direction == ZN_DIRECTION_RTL;

        GlyphHelper::split_words(glyphs, words, true, is_rtl);

        int word_count = words.size();

        float max_bound_width = 0.;
        bool need_add_line = false;

        float max_width;

        int glyph_count = glyphs.size();

        get_width(max_width);

        LayoutIterator layout_iterator = LayoutIterator(words);
        LineHandler line_handler;
        layout_iterator.set_max_width(max_width);
        layout_iterator.set_direction(direction);
        layout_iterator.get_handler(line_handler);

        ZN_RenderFlag render_flag = metrics.render_flag;
        layout_iterator.iter_words([&line_handler, &layout_iterator, &max_bound_width, &render_flag, &lines](WordData word)
                                   {
                                       word.load_info();

                                       bool next_line = line_handler.push_word(word);

                                       if (next_line)
                                       {
                                           ZN_LineData line = line_handler.get_line();

                                           lines.push_back(line);

                                           layout_iterator.get_handler(line_handler);

                                           return false;
                                       }

                                       max_bound_width = fmax(line_handler.width, max_bound_width);

                                       return true; });

        if (!line_handler.glyphs().empty())
        {

            ZN_LineData line = line_handler.get_line();

            lines.push_back(line);
        }

        measure_height();

        this->lines = lines;
        metrics.bound_width = max_bound_width;
        metrics.lines = lines;
        metrics.width = max_bound_width;
        width = metrics.width;

    }

public:
    float line_space = 5.0;

    std::vector<ZN_LineData> lines;

    LayoutMetrics metrics;

    ZN_TextLayout() = default;

    ZN_TextLayout(ZN_TextShaper shaper)
    {
        this->metrics.glyphs = shaper.get_metrics().glyphs;
        this->metrics.face = shaper.get_metrics().face;
        this->metrics.direction = shaper.get_metrics().direction;
        this->metrics.render_flag = shaper.get_metrics().render_flag;
        this->glyphs = metrics.glyphs;
        glyph_helper.load_glyph_set(this->glyphs);

        shaper.handler->set_layout(this);
    };

    ZN_ERROR get_width(float &w)
    {
        // glyphs.clear();

        if (glyphs.empty())
        {
            return ZN_ERR_NO_GLYPHS;
        }

        if (width == MAXWIDTH)
        {
            TextBlob blob = TextBlob::from(glyphs);
            blob.reverse();
            w = blob.get_text_max_width();
            width = w;
        }
        else if (width > 0)
        {
            w = width;
        }

        else
        {
            return ZN_ERR_NEGATIVE_WIDTH;
        }

        return ZN_OK;
    }

    void get_size(int &w, int &h)
    {
        w = metrics.get_width();
        h = metrics.height;
    }

    float padding()
    {
        return 0.0;
    }

    void get_lines(std::vector<ZN_LineData> &lines)
    {
        _generate_lines_by_word(lines);
    }

    void measure_height()
    {
        height = 0;
        for (int i = 0; i < lines.size(); i++)
        {

            ZN_LineData line = lines[i];

            float _line_space = line_space;


            height += line.height;

            if (i != 0)
            {
                height += line_space;
            }
        }

        this->metrics.height = height;
    }

    void set_metrics(ZN_TextMetrics metrics)
    {
        this->metrics.glyphs = metrics.glyphs;
        this->metrics.face = metrics.face;
        // face
        // this->metrics = metrics;
    }

    void generate_lines()
    {
        lines.clear();
        get_lines(lines);
    }

    void set_split_mode(int split_mode = SPLIT_BY_CHARACTER)
    {

        this->split_mode = split_mode;
    }

    void set_line_space(float _line_space)
    {
        line_space = _line_space;
    }
    ZN_ERROR set_width(float _width)
    {
        width = _width;
        float w;
        ZN_ERROR err = get_width(w);
        if (err)
            return err;
        width = w;
        metrics.width = w;

        return ZN_OK;
    }

    void set_glyph_set(std::vector<ZN_GlyphData> _glyphs)
    {
        glyph_helper.load_glyph_set(_glyphs);
        glyphs = _glyphs;
    }
};
