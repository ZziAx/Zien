class ZN_ShaperRun
{
public:
    ZN_GlyphObject *obj;

    ZN_ShaperRun() = default;

    ZN_ShaperRun(ZN_GlyphObject *obj) : obj(obj) {};

    std::string text;

    ZN_TextMetrics proccess(ZN_TextStyle *style = nullptr, ZN_RenderFlag render_flag = ZN_RenderFlag::BITMAP)
    {

        ZN_TextMetrics metrics;
        metrics.style = style;
        metrics.render_flag = render_flag;

        if (obj->type == ZN_GlyphType::GLYPH_FONT)
        {
            ZN_FontBundle *font_bundle = static_cast<ZN_FontBundle *>(obj);

            font_bundle->proccess(text, metrics);
        }

#if ZN_TINYXML_IMPLEMENTATION

        else if (obj->type == ZN_GlyphType::GLYPH_EMOJI)
        {

            ZN_EmojiBundle *emoji_bundle = static_cast<ZN_EmojiBundle *>(obj);
            emoji_bundle->proccess(text, metrics);
        }
#endif

        return metrics;
        // return ZN_TextMetrics();
    }

    void push_front(std::string c)
    {
        text = c + text;
    }

    bool push_back(char *c)
    {

        int codepoint = ZN_UtfHelper::next_codepoint(c);

        switch (obj->type)
        {
        case ZN_GlyphType::GLYPH_FONT:
        {
            ZN_FontBundle *font_bundle = static_cast<ZN_FontBundle *>(obj);

            if (!font_bundle->is_codepoint_exist(codepoint))
            {
                return false;
            }
            break;
        }
        case ZN_GlyphType::GLYPH_EMOJI:
        {

#if ZN_TINYXML_IMPLEMENTATION

            ZN_SvgbParser emoji_parser = EmojiManager::instance().get_parser_ref();

            int codepoint = ZN_UtfHelper::next_codepoint(c);

            if (emoji_parser.is_exist(codepoint))
            {
                break;
            }
            return false;

#endif

            break;
        }
        }

        text += c;

        return true;
    }
};