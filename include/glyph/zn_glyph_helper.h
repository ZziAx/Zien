
class GlyphHelper
{
public:
    GlyphHelper() {};

    std::vector<ZN_GlyphData> glyphs;
    std::vector<ZN_GlyphData> rev_glyphs;

    void load_glyph_set(std::vector<ZN_GlyphData> glyphs)
    {
        this->glyphs = glyphs;

        get_reversed(this->rev_glyphs);
    }
  
    static void load_glyph(FT_Face face, int codepoint,ZN_TextStyle * style = nullptr)
    {
        
        ZN_Load_Glyph(face,codepoint,style);
    }

  

    GlyphHelper range(int from, int to, bool reversed = false)
    {

        std::vector<ZN_GlyphData> src_glyphs;
        std::vector<ZN_GlyphData> sub_glyphs;

        if (reversed)
        {
            get_reversed(src_glyphs);
        }
        else
        {
            src_glyphs = glyphs;
        }

        if (to == -1)
        {
            to = src_glyphs.size();
        }

        for (int i = from; i < to; i++)
        {

            sub_glyphs.push_back(src_glyphs[i]);
        }

        GlyphHelper h = GlyphHelper();

        h.load_glyph_set(sub_glyphs);

        return h;
    }

    static void split_words(std::vector<ZN_GlyphData> glyphs, std::vector<WordData> &words, bool rev = false, bool rtl = true)
    {

        WordData word;

        bool need_add_new_word = false;

        if (rtl)
        {
            GlyphHelper::get_reversed_vector(glyphs);
        }
       

        int from, to;

        if (rtl)
        {
            from = 0;
            to = glyphs.size();
        }

        else
        {
            from = 0;
            to = glyphs.size();
        }

        bool rec = from < to;

        for (int i = rec ? from : to; rec ? i < to : i > from; rec ? i++ : i++)
        {
            need_add_new_word = true;
            ZN_GlyphData glyph = glyphs[i];
            // if (glyph.is_white_space())
            // {
            //     continue;
            // }

            bool is_white_space = glyph.is_white_space();

            if (rtl)
            {
                word.insert_last(glyph, false);
            }
            else
            {
                word.insert_first(glyph, false);
            }

            if (is_white_space)
            {
                need_add_new_word = false;

                if (word.glyphs.size() > 0)
                {

                    if (rev)
                    {
                        words.push_back(word);
                    }
                    else
                    {
                        words.insert(words.begin(), word);
                    }

                    // words.push_f(word);
                }

                word.load_info();

                word = WordData();

                // continue;
            }
        }


        if (need_add_new_word)
        {

            word.load_info();

            if (rev)
            {
                words.push_back(word);
            }
            else
            {
                words.insert(words.begin(), word);
            }
            // words.push_back(word);
        }
    }
    void get_reversed(std::vector<ZN_GlyphData> &glyphs)
    {

        std::vector<ZN_GlyphData> gl = this->glyphs;

        std::reverse(gl.begin(), gl.end());

        glyphs = gl;
    }

    static void get_reversed_vector(std::vector<ZN_GlyphData> &glyphs)
    {

        std::reverse(glyphs.begin(), glyphs.end());
    }
  

 

    static void get_paddings(FT_Face face, int &l, int &t, int &r, int &b, bool needL = true, bool needR = true, bool needT = true, bool needB = true)
    {

        FT_Bitmap &bitmap = face->glyph->bitmap;

        int rows = bitmap.rows;
        int cols = bitmap.width;

        l = 0;
        r = 0;
        t = 0;
        b = 0;

        return;
        unsigned char lalpha = 0;
        unsigned char ralpha = 0;
        unsigned char talpha = 0;
        unsigned char balpha = 0;

        bool is_valid_lcol = false;
        bool is_valid_rcol = false;
        bool is_valid_tcol = false;
        bool is_valid_bcol = false;

        needB = false;
        int threshold = 16;

        while ((!is_valid_lcol && needL) || (!is_valid_rcol && needR) || (!is_valid_tcol && needT) || (!is_valid_bcol && needB))
        {

            for (int i = 0; i < rows; i++)
            {

                if (needT || needB)
                {
                    // if (!is_valid_tcol && needT)
                    // {
                    for (int j = 0; j < cols; j++)
                    {

                        if (!is_valid_tcol && needT)
                        {
                            int trow = i * cols + j;

                            talpha = bitmap.buffer[trow];

                            if (talpha <= threshold)
                            {
                                is_valid_tcol = true;
                            }
                        }

                        if (!is_valid_bcol && needB)
                        {
                            int brow = (cols * (rows - i - 1)) + j;

                            balpha = bitmap.buffer[brow];
                            if (balpha <= threshold)
                            {
                                is_valid_bcol = true;
                            }
                        }

                        // }
                    }
                }

                if (!is_valid_rcol && needR)
                {

                    int rrow = (i * cols) + cols - (1 + r);

                    ralpha = bitmap.buffer[rrow];

                    if (ralpha > threshold)
                    {
                        // printf("ewroioiewr %i",ralpha);
                        is_valid_rcol = true;
                    }
                }

                if (!is_valid_lcol && needL)
                {
                    int lrow = i * cols + l;

                    lalpha = bitmap.buffer[lrow];

                    if (lalpha <= threshold)
                    {
                        is_valid_lcol = true;
                    }
                }
            }

            if (!is_valid_lcol)
            {
                l += 1;
            }

            if (!is_valid_rcol)
            {

                r += 1;
            }
            if (!is_valid_tcol)
            {
                t += 1;
            }

            if (!is_valid_bcol)
            {
                b += 1;
            }
        }

    };

    static void get_exact_bound(ZN_GlyphData glyph, int &w, int &h, bool left = true, bool right = true)
    {
        // left = false;
        // right = false;

        if (glyph.font_type == FontType::SVGB)
        {
            w = glyph.get_width();
            h = glyph.get_height();
            return;
        }

        glyph.load();
        FT_Face face = glyph.face;
        int l, t, r, b;
        GlyphHelper::get_paddings(face, l, t, r, b);

        FT_Bitmap &bitmap = face->glyph->bitmap;

        if (!left)
        {
            l = 0;
        }
        if (!right)
        {
            r = 0;
        }
        w = bitmap.width - l - r;
    }
};