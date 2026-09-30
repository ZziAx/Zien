
#include "freetype/ftoutln.h"

enum ZN_RenderFlag
{
    BITMAP,
    SVG
};
enum class Alignment
{
    NONE,
    RIGHT,
    LEFT,
    CENTER
};

enum class FontType
{
    NONE,
    NORMAL,
    COLR0,
    COLR1,
    SVGB
};

class ClusterRange
{
public:
    int start;
    int end;

    ClusterRange() = default;

    ClusterRange(int start, int end) : start(start), end(end) {}
};

class ZN_GlyphData
{

public:
    ZN_SvgBundle svg;
    ZN_RenderFlag render_flag = ZN_RenderFlag::BITMAP;
    int codepoint;
    int charcode;
    std::string character;
    int num_chars;
    FontType font_type;
    GlyphMetrics metrics;

    ClusterRange cluster_range;
    FT_Face face;

    std::shared_ptr<ZN_TextStyle> style;

    unsigned char *buffer = nullptr;
    int pitch;
    ZN_GlyphData() = default;
    ZN_GlyphData(FT_Face face, std::string character, int codepoint, GlyphMetrics metrics, ClusterRange cluster_range, ZN_TextStyle *style, FontType font_type) : face(face),
                                                                                                                                                                  character(character),
                                                                                                                                                                  codepoint(codepoint),
                                                                                                                                                                  metrics(metrics),
                                                                                                                                                                  cluster_range(cluster_range),
                                                                                                                                                                  style(style ? std::make_shared<ZN_TextStyle>(*style) : nullptr),
                                                                                                                                                                  font_type(font_type)

    {
    }

    float get_x_advance()
    {
        // int stroke_x_advance = style->stroker.strokes[0].metrics.x_advance;

        int stroke_x_advance = 0;

        return fmax((float)stroke_x_advance, metrics.x_advance);
    }

    float get_min_y()
    {
        // int stroke_min_y = style->stroker.strokes[0].metrics.min_y;
        float stroke_min_y = 0;

        // return metrics.min_y;
        return fmin((float)stroke_min_y, metrics.min_y);
    }
    float get_max_y()
    {

        // int stroke_max_y = style->stroker.strokes[0].metrics.max_y;
        float stroke_max_y = 0;

        // printf("ewruoiouiwer %f\n",metrics.height);
        return fmax((float)stroke_max_y, metrics.max_y);
    }

    ZN_TextStyle *get_style()
    {
        return style.get();
    }

    float get_width()
    {
        int stroke_width = 0;

        if(is_white_space()){
            return get_x_advance();
        }
        return fmax((float)stroke_width, metrics.width);
    }
    float get_height()
    {
        int stroke_height = 0;
        return fmax((float)stroke_height, metrics.height);
    }
    float get_x()
    {

        // ifp2
        // None
        // return metrics.width;
        if (font_type == FontType::SVGB)
        {
            return metrics.x_offset;
        }

        load();
        int left = metrics.x_offset + face->glyph->bitmap_left;
        return left;
    }
    void print_str()
    {
        printf("\nglyph char is %s\n", character.c_str());
    }
    bool is_en()
    {
        bool _is_en = false;
        for (char c : character)
        {

            _is_en = isalpha(static_cast<unsigned char>(c)) && isascii(c);

            if (_is_en)
                break;
        }

        return _is_en;
    }
    FT_ULong get_charcode()
    {
        FT_UInt gindex;
        FT_ULong charcode = FT_Get_First_Char(face, &gindex);

        while (gindex != 0)
        {
            if (gindex == codepoint)
                return charcode;
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
        }
        return 0; // Not found
    }

    bool is_white_space()
    {

        if (character == " " || character == "\n" || character == "\t" || character == "\r")
        {
            return true;
        }

        return false;
    }
    void load(FT_Face face, int codepoint, ZN_TextStyle *style = nullptr)
    {
        if (font_type == FontType::SVGB)
            return;

        ZN_Load_Glyph(face, codepoint, style);
    }

    void load()
    {
        load(face, codepoint, style.get());
    }
};

static void ZN_Hr_Begin_Fit_Pad(ZN_GlyphData glyph, float &sh)
{
    sh = 0;
 
    
    sh -= glyph.get_x_advance() - glyph.get_width();
    sh += glyph.get_x();
}

static void ZN_Hr_End_Fit_Pad(ZN_GlyphData glyph, float &sh)
{
    sh = 0;
    sh = glyph.get_x();
}

static void ZN_Hr_Fit_Pad(std::vector<ZN_GlyphData> glyphs, float &sh)
{
    float b, e;
    sh = 0;
    ZN_GlyphData first_glyph = glyphs[0];
    ZN_GlyphData last_glyph = glyphs.back();
    ZN_Hr_Begin_Fit_Pad(first_glyph, b);
    ZN_Hr_End_Fit_Pad(last_glyph, e);
    sh = sh + b - e;
}

static void ZN_Measure_Max_Width(std::vector<ZN_GlyphData> glyphs, float &w, float &adv)
{

    w = 0.0;
    adv = 0;
    if (glyphs.empty())
        return;

    std::vector<ZN_GlyphData> gl = glyphs;

    float pen_x = 0;

    int count = gl.size();

    for (int i = 0; i < count - 1; i++)
    {
        ZN_GlyphData glyph = gl[i];
        pen_x += glyph.get_x_advance();
    }

    ZN_GlyphData last_glyph = gl[count - 1];

    float sh = 0;
    if (last_glyph.render_flag == ZN_RenderFlag::SVG)
    {
        sh = last_glyph.svg.args.x - gl[0].svg.args.x;
        w = (pen_x + last_glyph.get_width()) - sh;
    }
    else
    {
        ZN_Hr_Fit_Pad(gl, sh);
        // w = pen_x+last_glyph.get_x_advance();
        w = (pen_x + last_glyph.get_x_advance()) + sh;
    }

    // gcode

    adv = pen_x + last_glyph.get_x_advance();
}

class WordData
{
public:
    std::vector<ZN_GlyphData> glyphs;
    std::vector<ZN_GlyphData> rev_glyphs;

    float width = 0.0;
    float height = 0.0;

    float hr_advance = 0;

    float min_y = 0;
    float max_y = 0;

    WordData() {};
    WordData(std::vector<ZN_GlyphData> glyphs) : glyphs(glyphs)
    {
        load_info();
    };

    int get_x()
    {
        return glyphs.back().get_x();
    }
    void print_str()
    {

        printf("\nword text is : \"%s\"\n", str().c_str());
        // ANCHOR: printf debug
        // $&
    }

    void push_back(std::vector<ZN_GlyphData> &out)
    {
        std::vector<ZN_GlyphData> word_glyphs = get_glyphs();
        out.insert(out.end(), word_glyphs.begin(), word_glyphs.end());
    }
    std::vector<ZN_GlyphData> get_rev_glyphs()
    {

        return rev_glyphs;
    }

    std::vector<ZN_GlyphData> get_glyphs()
    {

        return glyphs;
    }
    bool is_rtl()
    {

        // ANCHOR: printf debug
        // $&
        return true;
    }

    std::string str()
    {
        std::string s = "";

        for (auto &g : glyphs)
        {
            s = g.character + s;
        }

        return s;
    }
    void insert_first(ZN_GlyphData glyph, bool refresh = false)
    {
        // glyphs.push_back(glyph);
        glyphs.insert(glyphs.begin(), glyph);

        if (refresh)
        {
            load_info();
        }
    }

    void insert_last(ZN_GlyphData glyph, bool refresh = false)
    {
        // glyphs.push_back(glyph);
        glyphs.insert(glyphs.end(), glyph);

        if (refresh)
        {
            load_info();
        }
    }

    void load_info()
    {

        width = 0;
        min_y = 0;
        max_y = 0;

        ZN_Measure_Max_Width(glyphs, width, hr_advance);


        for (auto &glyph : glyphs)
        {
            if (glyph.is_white_space())
                continue;

            if (glyph.get_height() > height)
            {
                height = glyph.get_height();
            }

            min_y = fmin(min_y, glyph.get_min_y());
            max_y = fmax(max_y, glyph.get_max_y());

            // printf("\neuwiouiwre %i\n",max_y);
        }

        height = max_y - min_y;

        std::vector<ZN_GlyphData> reversed(glyphs.rbegin(), glyphs.rend());
        rev_glyphs = reversed;
    }
};

class TextBlob
{

private:
    int direction = ZN_DIRECTION_RTL;
    std::vector<ZN_GlyphData> glyphs;
    TextBlob(std::vector<ZN_GlyphData> glyphs) : glyphs(glyphs) {};

    void push_glyphs(std::vector<ZN_GlyphData> glyphs, std::vector<ZN_GlyphData> &out)
    {

        if (direction == ZN_DIRECTION_RTL)
        {
            push_pack(glyphs, out);
        }

        else
        {

            push_front(glyphs, out);
        }
    }

    void push_pack(std::vector<ZN_GlyphData> glyphs, std::vector<ZN_GlyphData> &out)
    {
        out.insert(out.end(), glyphs.begin(), glyphs.end());
    }

    void push_front(std::vector<ZN_GlyphData> glyphs, std::vector<ZN_GlyphData> &out)
    {
        out.insert(out.begin(), glyphs.begin(), glyphs.end());
    }

public:
    TextBlob() = default;
    static TextBlob from(std::vector<ZN_GlyphData> glyphs)
    {
        return TextBlob(glyphs);
    }

    std::vector<ZN_GlyphData> get_glyphs()
    {
        return glyphs;
    }

    void reverse()
    {
        std::vector<ZN_GlyphData> gl = this->glyphs;

        std::reverse(gl.begin(), gl.end());

        // GlyphHelper::get_reversed(glyphs);
        glyphs = gl;
    }

    std::string to_str()
    {

        std::string s = "";
        for (auto &g : glyphs)
        {

            s += g.character;
        }

        return s;
    }
    void set_direction(int direction)
    {
        this->direction = direction;
    }

    static int get_hr_begin_fit_shift(ZN_GlyphData glyph)
    {

        int sh = 0;
        sh -= glyph.get_x_advance() - glyph.get_width();
        sh += glyph.get_x();

        // int sh;
        return sh;
    }

    static int get_hr_end_fit_shift(ZN_GlyphData glyph)
    {
        int sh = glyph.get_x();

        return sh;
    }

    ZN_GlyphData first_glyph()
    {
        return glyphs.front();
    }

    ZN_GlyphData last_glyph()
    {
        return glyphs.back();
    }

    int get_hr_fit_shift()
    {

        int sh = 0;

        ZN_GlyphData first_glyph = this->first_glyph();
        ZN_GlyphData last_glyph = this->last_glyph();

        sh += get_hr_begin_fit_shift(first_glyph);
        sh -= get_hr_end_fit_shift(last_glyph);

        return sh;
    }

    float get_text_max_width(WordData word)
    {

        float _w, _adv;

        std::vector<ZN_GlyphData> _gl = glyphs;
        fake_push_glyphs(word.get_glyphs(), _gl);
        ZN_Measure_Max_Width(_gl, _w, _adv);

        return _w;
    }
    float get_text_max_width()
    {

        float w, adv;

        std::vector<ZN_GlyphData> _gl = glyphs;
        ZN_Measure_Max_Width(_gl, w, adv);

        return w;
    }

    void push_glyphs(std::vector<ZN_GlyphData> glyphs)
    {
        push_glyphs(glyphs, this->glyphs);
    }

    void fake_push_glyphs(std::vector<ZN_GlyphData> glyphs, std::vector<ZN_GlyphData> &out)
    {

        push_glyphs(glyphs, out);
    }
};