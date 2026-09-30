
// enum class Alignment
// {
//     NONE,
//     RIGHT,
//     LEFT,
//     CENTER
// };

// enum class FontType
// {
//     NONE,
//     NORMAL,
//     COLR0,
//     COLR1,
//     SVGB
// };

// class ClusterRange
// {
// public:
//     int start;
//     int end;

//     ClusterRange() = default;

//     ClusterRange(int start, int end) : start(start), end(end) {}
// };

// struct GlyphMetrics
// {
//     float x_offset;
//     float y_offset;
//     float x_advance;
//     float width;
//     float height;
//     float min_y;
//     float max_y;
//     int left;
//     int top;
// };

// class ZN_GlyphData
// {

// public:
//     int codepoint;
//     std::string character;
//     int num_chars;
//     FontType font_type;
//     GlyphMetrics metrics;
//     ClusterRange cluster_range;
//     FT_Face face;
//     std::shared_ptr<ZN_TextStyle> style;
//     // FT_Bitmap bitmap; // Optional cached bitmap

//     ZN_GlyphData() = default;
//     ZN_GlyphData(FT_Face face, std::string character, int codepoint, GlyphMetrics metrics, ClusterRange cluster_range, ZN_TextStyle *style, FontType font_type) : face(face),
//                                                                                                                                                                character(character),
//                                                                                                                                                                codepoint(codepoint),
//                                                                                                                                                                metrics(metrics),
//                                                                                                                                                                cluster_range(cluster_range),
//                                                                                                                                                                style(style ? std::make_shared<ZN_TextStyle>(*style) : nullptr),
//                                                                                                                                                                font_type(font_type)

//     {
//     }

//     void print_str()
//     {
//         printf("\nglyph char is %s\n", character.c_str());
//     }
//     bool is_en()
//     {
//         bool _is_en = false;
//         for (char c : character)
//         {

//             _is_en = isalpha(static_cast<unsigned char>(c)) && isascii(c);

//             if (_is_en)
//                 break;
//         }

//         return _is_en;
//     }
//     FT_ULong get_charcode()
//     {
//         FT_UInt gindex;
//         FT_ULong charcode = FT_Get_First_Char(face, &gindex);

//         while (gindex != 0)
//         {
//             if (gindex == codepoint)
//                 return charcode;
//             charcode = FT_Get_Next_Char(face, charcode, &gindex);
//         }
//         return 0; // Not found
//     }

//     bool is_white_space()
//     {

//         if (character == " " || character == "\n" || character == "\t" || character == "\r")
//         {
//             return true;
//         }

//         return false;
//     }
//     void load(FT_Face face, int codepoint, ZN_TextStyle *style = nullptr)
//     {
//         if (font_type == FontType::SVGB)
//             return;

//         ZN_LOAD_GLYPH(face, codepoint, style);
//     }

//     void load()
//     {
//         load(face, codepoint, style.get());
//     }

//     int get_x()
//     {

//         if (font_type == FontType::SVGB)
//         {
//             return metrics.x_offset;
//         }

//         load();
//         // FT_Load_Glyph(face, codepoint, FT_LOAD_RENDER);
//         int left = metrics.x_offset + face->glyph->bitmap_left;

//         // int top = y + -glyph.y_offset - face->glyph->bitmap_top - metrics.min_y;

//         // int left = face->glyph->bitmap_left;

//         return left;
//     }

//     int get_y(FT_Face &face)
//     {
//         FT_Load_Glyph(face, codepoint, FT_LOAD_RENDER);
//         int y = -metrics.y_offset - face->glyph->bitmap_top;
//         return y;
//     }

//     // int get_y(){
//     //     return y_offset;
//     // }
//     int get_height()
//     {
//         FT_Load_Glyph(face, codepoint, FT_LOAD_RENDER);

//         return face->glyph->bitmap.rows;
//     }
//     int get_y()
//     {
//         return face->glyph->bitmap_top;

//         // return top;
//     }
// };