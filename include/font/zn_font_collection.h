
/*
 * @class ZN_FontCollection
 * @brief Manages all font and emoji bundles used for text shaping.
 *
 * This class handles font registration, glyph selection, and run generation
 * for text shaping. If `ZN_TINYXML_IMPLEMENTATION` is enabled, it also
 * manages emoji SVG bundles.
 *
 * Responsibilities:
 * - Load and organize multiple fonts (default + alternates)
 * - Select the proper font or emoji bundle for each codepoint
 * - Generate shaping runs for a given text
 * - Aggregate shaping metrics
 */
class ZN_FontCollection
{
private:
    std::vector<ZN_ShaperRun> shaper_runs;     ///< Holds all shaped runs for the current text.
    std::vector<ZN_FontBundle *> font_bundles; ///< Alternate fonts available for fallback.

    
    // ZN_FontBundle get_font()
    // {
    //     return default_font_bundle;
    // }

#if ZN_TINYXML_IMPLEMENTATION
    ZN_EmojiBundle emoji_bundle = ZN_EmojiBundle(0); ///< Used only if TinyXML (emoji support) is enabled.
#endif

    /**
     * @brief Adds a font to the collection.
     *
     * @param font Path to font file.
     * @param flag Whether it's a default or alternate font.
     * @return ZN_ERROR Error code if load failed.
     */

    ZN_ERROR add_font(std::string path)
    {

        ZN_Font font = ZN_Font(std::string("vazir"), path);

        std::vector<ZN_Font> fonts;
        fonts.push_back(font);
        ZN_FontBundle *f_bundle = new ZN_FontBundle("vazir", fonts);

        add_font(f_bundle);

        return ZN_OK;
    }

    ZN_ERROR add_font(ZN_FontBundle *f_bundle)
    {

        ZN_ERROR err = f_bundle->load();

        if (err)
            return err;

        if (f_bundle->inited)
        {
            font_bundles.push_back(f_bundle);
        }

        return ZN_OK;
    }

public:
    ZN_FontCollection() = default;

    /// @return Path of the current default font.
    // char *get_font_path()
    // {
    //     return default_font_bundle.font_path;
    // }

    /**
     * @brief Determines which glyph bundle should render the given character.
     *
     * Prioritizes emoji bundles (if enabled), then the default font,
     * and finally any alternate fonts.
     *
     * @param c UTF-8 encoded character.
     * @return ZN_GlyphObject* Pointer to the appropriate glyph bundle.
     */
    ZN_GlyphObject *resolve_glyph_bundle(char *c)
    {

        int codepoint = ZN_UtfHelper::next_codepoint(c);
        ZN_GlyphObject *g_obj = nullptr;
#if ZN_TINYXML_IMPLEMENTATION

        ZN_SvgbParser &emoji_parser = EmojiManager::instance().get_parser_ref();

        if (EmojiManager::instance().loaded && emoji_parser.is_exist(codepoint))
        {

            emoji_bundle = ZN_EmojiBundle();
            g_obj = &emoji_bundle;
        }
#endif

        // Check default font and fallback fonts if emoji not found
        if (!g_obj)
        {

            

            // return g_obj;

            for (auto &bundle : font_bundles)
            {

                if (bundle->is_codepoint_exist(codepoint))
                {

                    g_obj = bundle;
                    break;
                }
            }
        }

        return g_obj;
    }

#if ZN_TINYXML_IMPLEMENTATION
    /**
     * @brief Loads an emoji SVG set from file.
     *
     * @param path Path to emoji SVG bundle directory or file.
     */
    void add_emoji_set(const char *path)
    {
        EmojiManager::instance().load((char *)path);
    }
#endif

    /**
     * @brief Splits text into shaping runs based on font availability.
     *
     * Each run corresponds to a consecutive sequence of characters
     * from the same glyph bundle.
     *
     * @param text UTF-8 input text.
     * @return std::vector<ZN_ShaperRun> All detected shaping runs.
     */
    std::vector<ZN_ShaperRun> get_runs(const char *text)
    {
        std::string t = ZN_UtfHelper::visible_text(std::string(text));
        std::vector<ZN_ShaperRun> runs;
        ZN_ShaperRun shaper_run;
        int char_count = ZN_UtfHelper::utf8_char_count(t.c_str());
        ZN_GlyphObject *g_obj = nullptr;
        bool need_add = false;

        for (int i = 0; i < char_count; i++)
        {
            char *c = ZN_UtfHelper::utf8_substr(t.c_str(), i, 1);

            if (g_obj == nullptr)
            {

                g_obj = resolve_glyph_bundle(c);

                shaper_run = ZN_ShaperRun(g_obj);

                if (g_obj == nullptr)
                    continue;

                need_add = true;
            }

            bool done = shaper_run.push_back(c);

            if (!done)
            {
                runs.insert(runs.begin(), shaper_run);
                g_obj = nullptr;
                i -= 1;
                need_add = false;
            }
        }

        if (need_add)
            runs.insert(runs.begin(), shaper_run);


        return runs;
    }

    /**
     * @brief Shapes the entire text and merges metrics from all runs.
     *
     * @param text Input UTF-8 text.
     * @param metrics Output metrics structure.
     */
    ZN_ERROR shape_text(const char *text, ZN_TextMetrics &metrics, ZN_TextStyle *style = nullptr)
    {

        shaper_runs.clear();
        shaper_runs = get_runs(text);

        for (int i = 0; i < shaper_runs.size(); i++)
        {

            ZN_ShaperRun &shape_run = shaper_runs[i];

            ZN_TextMetrics m = shape_run.proccess(style,metrics.render_flag);

            metrics.merge_metrics(m, false);

        }


        return ZN_OK;
    }

    /// Adds a new alternate font.
    bool add_font(char *font)
    {
        return add_font(font);
    }

    ZN_ERROR add_font(ZN_Font &font)
    {
        ZN_FontBundle *b = new ZN_FontBundle(font);
        return add_font(b);
    }

    ZN_ERROR add_bundle(ZN_FontBundle *bundle)
    {
        return add_font(bundle);
    }

    // ZN_ERROR add_bundle(ZN_SvgbBundle * bundle){

    // }
};
