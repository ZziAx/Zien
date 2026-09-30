#include <cstdint>

class ZN_GlyphDataBuilder
{

public:
    static void _build_stroke(FT_Face face, int i, ZN_TextMetrics &metric, std::string content, FontType font_type, hb_glyph_position_t *glyph_pos, hb_glyph_info_t *glyph_info, ZN_GlyphData &glyph)
    {

        std::vector<ZN_Stroke> &strokes = metric.style->stroker.strokes;

        FT_Library ft = ZN_EngineContext::get_ft();

        for (ZN_Stroke &stroke : strokes)
        {
            if (stroke.enabled)
            {

                // FT_Load_Char(face, codepoint, FT_LOAD_DEFAULT); doesnt work for connected glyph like لا، so i switched to FT_load_glyph

                // int codepoint = ZN_UtfHelper::next_codepoint(content.c_str());

                int codepoint = glyph_info[i].codepoint;
                // printf("ferwuiouiower %s\n", content.c_str());

                FT_Load_Char(face, codepoint, FT_LOAD_DEFAULT);
                FT_Load_Glyph(face, codepoint, FT_LOAD_DEFAULT);
                // ZN_Load_Glyph(face, codepoint, metric.style);
                //         const char * t = "A";
                // ZN_UtfHelper::next_codepoint(t);

                FT_Stroker ft_stroker;
                FT_Stroker_New(ft, &ft_stroker);
                FT_Stroker_Set(ft_stroker,
                               64 * (stroke.width), // width in 26.6 fixed-point
                               FT_STROKER_LINECAP_SQUARE,
                               FT_STROKER_LINEJOIN_ROUND,
                               0);

                // FT_Glyph stroked_glyph;
                // FT_Get_Glyph(face->glyph, &stroked_glyph);

                // // Apply stroke (0 = outside, 1 = inside, false for keeping border)
                // FT_Glyph_StrokeBorder(&stroked_glyph, ft_stroker, 1, 1);

                // // 🔹 Convert outline glyph to bitmap
                // FT_Glyph_To_Bitmap(&stroked_glyph, FT_RENDER_MODE_NORMAL, nullptr, 1);

                // // Now safe to cast
                // FT_BitmapGlyph bmp_glyph = (FT_BitmapGlyph)stroked_glyph;

                ZN_Load_Glyph(face, glyph_info[i].codepoint, metric.style, false);
                // FT_Load_Glyph(face, glyph_info[i].codepoint, FT_LOAD_DEFAULT);
                FT_Glyph glyph;
                FT_Get_Glyph(face->glyph, &glyph);

                // Apply stroke
                FT_Glyph_Stroke(&glyph, ft_stroker, 1);

                FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 1);

                // Access the bitmap
                FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
                FT_Bitmap bitmap = bitmap_glyph->bitmap;

                // FT_Bitmap &bitmap = bmp_glyph->bitmap;

                int top = face->glyph->bitmap_top; // pixels above baseline
                int bottom = top - bitmap.rows;    // pixels below baseline

                int left = (0 + (glyph_pos[i].x_offset / 64) + face->glyph->bitmap_left);

                int w = (int)bitmap.width;
                int h = (int)bitmap.rows;
                int num_chars = ZN_UtfHelper::utf8_char_count(content.c_str());

                GlyphMetrics gm;
                gm.height = float(h);
                gm.width = float(w);
                gm.x_advance = float(glyph_pos[i].x_advance / 64);
                gm.x_offset = float(glyph_pos[i].x_offset / 64);
                gm.y_offset = float(glyph_pos[i].y_offset / 64);
                gm.min_y = float(bottom);
                gm.max_y = float(top);
                gm.left = left;
                gm.top = top;
                // printf("erwuiouoiuwer %i",left);

                stroke.metrics = gm;
                stroke.buffer = bitmap.buffer;
                stroke.bitmap = bitmap;

                // printf("roweoui %i\n",h);

                // Cleanup
                // FT_Done_Glyph(stroked_glyph);
                // FT_Stroker_Done(ft_stroker);
            }
        }
    }

    static void build_svg(FT_Face face, ZN_TextMetrics &metric, std::string content, FontType font_type, ZN_SvgBundle &b, ZN_GlyphData &glyph)
    {
        ZN_SvgArgs args = b.args;
        GlyphMetrics gm;
        gm.height = args.height;
        gm.width = args.width;

        // printf("ewruiuioewr %f %f\n",args.height, args.top);

        gm.x_advance = args.x_advance;
        gm.x_offset = args.x_offset;
        gm.y_offset = args.y_offset;
        gm.min_y =  args.bottom;
        gm.max_y = args.top;
        gm.left = 0;
        gm.top = 0;

        // printf("ewruouoiwer %f %f\n",gm.min_y,gm.max_y);
        glyph = ZN_GlyphData(
            face,
            content,
            0,
            gm,
            ClusterRange(0, 0),
            metric.style,
            font_type);

        glyph.svg = b;
        glyph.render_flag = ZN_RenderFlag::SVG;
    }

    static void build_svg(FT_Face face, int i, ZN_TextMetrics &metric, std::string content, FontType font_type, hb_glyph_position_t *glyph_pos, hb_glyph_info_t *glyph_info, ZN_GlyphData &glyph)
    {
        int w, h, x_advance;
        ZN_SvgBundle svg;
        // ZN_GlyphToSvg::to_svg(face, glyph_info[i].codepoint, x_advance, w, h, svg);

        // printf("width %i height %i x_adv %i %s\n", w, h, x_advance, content.c_str());

        int num_chars = ZN_UtfHelper::utf8_char_count(content.c_str());

        GlyphMetrics gm;
        gm.height = float(h);
        gm.width = float(w);
        gm.x_advance = x_advance;
        gm.x_offset = float(glyph_pos[i].x_offset / 64);
        gm.y_offset = float(glyph_pos[i].y_offset / 64);
        gm.min_y = float(0);
        gm.max_y = float(0);
        gm.left = 0;
        gm.top = 0;

        glyph = ZN_GlyphData(
            face,
            content,
            glyph_info[i].codepoint,
            gm,
            ClusterRange(0, num_chars),
            metric.style,
            font_type);

        glyph.svg = svg;

        // glyph.render_flag = ZN_RenderFlag::SVG;
    }

    static void build_glyph(FT_Face face, int i, ZN_TextMetrics &metric, std::string character, FontType font_type, hb_glyph_position_t *glyph_pos, hb_glyph_info_t *glyph_info, ZN_GlyphData &glyph)
    {

        int x = 0;
        FT_Bitmap &bitmap = face->glyph->bitmap;
        int top = face->glyph->bitmap_top;
        int bottom = top - bitmap.rows;
        int left = ((glyph_pos[i].x_offset / 64) + face->glyph->bitmap_left);

        int w = (int)bitmap.width;
        int h = (int)bitmap.rows;
        int num_chars = ZN_UtfHelper::utf8_char_count(character.c_str());

        GlyphMetrics gm;
        gm.height = float(h);
        gm.width = float(w);
        gm.x_advance = float(glyph_pos[i].x_advance / 64);
        gm.x_offset = float(glyph_pos[i].x_offset / 64);
        gm.y_offset = float(glyph_pos[i].y_offset / 64);
        gm.min_y = float(bottom);
        gm.max_y = float(top);
        gm.left = left;
        gm.top = top;
// printf("ewiporioperw '%f'\n",gm.x_advance);
        // _build_stroke(face, i, metric, content, font_type, glyph_pos, glyph_info, glyph);

        glyph = ZN_GlyphData(
            face,
            character,
            glyph_info[i].codepoint,
            gm,
            ClusterRange(0, num_chars),
            metric.style,
            font_type);

        int size = w * h;
        glyph.buffer = new unsigned char[size]; // allocate
        memcpy(glyph.buffer, bitmap.buffer, size);
        // glyph.buffer = bitmap.buffer;
        glyph.pitch = bitmap.pitch;

#ifdef ZN_CACHE_ENABLE
        ZN_CacheManager &cache_manager = ZN_CacheManager::instance();
        cache_manager.put_glyph(glyph);
#endif

   
    }

    static void build_emoji(ZN_TextMetrics metric, const char *hex, ZN_GlyphData &glyph)
    {
#if ZN_TINYXML_IMPLEMENTATION

        // return;

        ZN_SvgbParser emoji_parser = EmojiManager::instance().get_parser_ref();

        bool loaded = false;
        ZN_SvgbEmojiSlot *slot = emoji_parser.get_slot(hex, loaded);

        if (loaded && slot != nullptr)
        {
            slot->load_buffer();
        }

        int width;
        int height;

        // slot->get_size(width, height);

        int target_width = fnts;

        // printf("Target width %i\n", target_width);

        GlyphMetrics gm;
        gm.width = target_width;
        gm.height = target_width;
        gm.x_advance = target_width;
        // gm.content = x_advance;
        glyph = ZN_GlyphData(
            FT_Face(),
            std::string(hex),
            0,
            gm,
            ClusterRange(0, 1),
            nullptr,
            FontType::SVGB);

        // glyph = ZN_GlyphData(
        //     0,            // codepoint
        //     0,            // x_offset
        //     0,            // y_offset
        //     target_width, // x_advance
        //     0,            // min_y
        //     0,            // max_y
        //     target_width, // width
        //     target_width, // height
        //     // 0,                 // height (unused for emoji)
        //     std::string(hex), // content
        //     0,                // left
        //     0,                // top
        //     // target_width,      // min_x (or other metric)
        //     // 0,                 // min_y
        //     // 0,                 // h_shrink
        //     1,                 // num_chars
        //     FontType::SVGB,    // font_type
        //     FT_Face(),         // FT_Face
        //     ClusterRange(0, 1) // cluster_range
        // );

#endif
    }
};