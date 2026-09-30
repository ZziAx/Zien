// #include "glyph_helper.h"

// class

class LineHandler
{
private:
    ZN_LineData line;

    TextBlob blob;

    int direction = ZN_DIRECTION_RTL;

    bool exceed_max_width(float width)
    {
        return width > this->max_width;
    }

    float next_width(ZN_GlyphData glyph)
    {

        return adv + glyph.get_width();
    }

    void shift_adv(ZN_GlyphData glyph)
    {
        adv += glyph.get_x_advance();
    }

    void shift_width(ZN_GlyphData glyph)
    {
        width = adv + glyph.get_width();
    }

    void shift_info(ZN_GlyphData glyph)
    {
        shift_width(glyph);
        shift_adv(glyph);
    }

    float next_width(WordData word)
    {


        std::vector<ZN_GlyphData> gl = glyphs();

        word.push_back(gl);

        float w;
       

       w = blob.get_text_max_width(word);

    
        return w;
        // return adv + word.width;
    }

    void shift_width(WordData word)
    {
        width = blob.get_text_max_width();
    }

    // word
    void shift_adv(WordData word)
    {
        adv += word.hr_advance;
    }

    void shift_adv(int sh)
    {
       
        adv += sh;


    }
    void shift_info(WordData word)
    {

        shift_width(word);
        shift_adv(word);

        line.min_y = fmin(line.min_y, word.min_y);
        line.max_y = fmax(line.max_y, word.max_y);
    }

public:
    LineHandler() = default;

    float width = 0;
    float height = 0;
    float adv = 0; // pen x

    float max_width;

    // std::vector<ZN_GlyphData> glyphs;

    std::vector<ZN_GlyphData> glyphs()
    {
        return blob.get_glyphs();
    }

    void set_direction(int direction)
    {
        this->direction = direction;
        blob.set_direction(direction);
    }
    ZN_LineData get_line()
    {

        std::vector<ZN_GlyphData> _glyphs = glyphs();

        //  ifp2
        // float height = round(line.max_y - line.min_y);
        float height = line.max_y - line.min_y;

        // printf("\nerwouuoiuoiewr %f\n",line.min_y);
        for (int i = 0; i < _glyphs.size(); i++)
        {

            FontType t = _glyphs[i].font_type;

            if (t == FontType::SVGB)
            {
                float _h = _glyphs[i].metrics.height;

                height = fmax(height, (float)_h);
            }
        }

        line.height = height;

        line.glyphs = glyphs();

        return line;
        // line.width = width;

        // line.
        // line.
        // line.content =
    }
    int push_glyph(ZN_GlyphData glyph)
    {
        int _width = next_width(glyph);

        if (exceed_max_width(_width))
        {
            return 0;
        }

        shift_info(glyph);

        glyphs().push_back(glyph);

        return 1;
    }

    bool push_word(WordData word)
    {
        float width = next_width(word);

        if (!glyphs().empty() && exceed_max_width(width))
        {
            return true;
        }

        blob.push_glyphs(word.get_glyphs());
        
        shift_info(word);

        return false;
    }

    void pop_last()
    {
    }
};

class LayoutIterator
{

private:
    int max_width = 0;
    int direction = ZN_DIRECTION_RTL;

public:
    bool done = false;
    std::vector<ZN_GlyphData> glyphs;

    std::vector<WordData> words;

    LayoutIterator() = default;
    LayoutIterator(std::vector<ZN_GlyphData> glyphs) : glyphs(glyphs) {};
    LayoutIterator(std::vector<WordData> words) : words(words) {};

    void set_max_width(int max_width)
    {
        this->max_width = max_width;
    }

    void set_direction(int direction)
    {
        this->direction = direction;
    }

    void get_handler(LineHandler &line_handler)
    {
        line_handler = LineHandler();
        line_handler.max_width = max_width;
        line_handler.set_direction(direction);
        // line_handler.
    }

    void iter_words(std::function<bool(WordData word)> next_word)
    {

        for (int i = 0; i < words.size(); i++)
        {
            bool success = next_word(words[i]);

            if (!success)
            {
                i -= 1;
            }
        }
    }
};