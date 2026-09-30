/**
 * @class ZN_FontBundle
 * @brief Represents a font-based glyph bundle for text rendering.
 *
 * This class encapsulates a font file and provides methods to load it,
 * shape text, generate glyph metrics, and query glyph existence. It
 * inherits from `ZN_GlyphObject` and is specialized for `GLYPH_FONT` types.
 *
 * Internally, it uses FreeType to load and manage font faces, and
 * `ZN_HarfbuzzShaper` for shaping and metrics calculation via HarfBuzz.
 */

template <typename Method, typename Variant>
void call(Variant &v, Method m)
{
    std::visit([&](auto &x)
               { (x.*m)(); }, v);
}

class ZN_FontBundle : public ZN_GlyphObject
{
private:
    std::string family_name;
    std::string selected_tag;
    // std::vector<ZN_Font> fonts;

    FontType font_type = FontType::NONE; ///< Type of font (NORMAL, COLR0, COLR1)

public:
    std::vector<ZN_Font> fonts;

    std::vector<ZN_VariantFont> _fonts;

    bool inited = false; ///< Flag indicating whether the font has been successfully loaded

    // ZN_FontBundle() = default;

    /**
     * @brief Default constructor.
     *
     * Initializes a `ZN_FontBundle` object as a GLYPH_FONT type without loading a font.
     */
    ZN_FontBundle() : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT) {};

    /**
     * @brief Constructor with font path and flags.
     *
     * @param font_path Path to the font file
     * @param flag Font flags to control features
     *
     * Initializes the object but does not load the font immediately.
     */
    ZN_FontBundle(std::string family_name, std::vector<ZN_Font> fonts)
        : family_name(family_name), fonts(fonts), ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT)
    {

        // set_variant_fonts(fonts);
        // for (auto &f : fonts)
        // {
        //     this->fonts.push_back(ZN_HB_Font(f.tag, f.path));
        // }
    }

    ZN_FontBundle(std::vector<ZN_Font> fonts)
        : fonts(fonts), ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT)
    {
    }

    ZN_FontBundle(std::string family_name, ZN_Font font)
        : family_name(family_name), ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT)
    {
        fonts.push_back(font);
    }

    ZN_FontBundle(ZN_Font font)
        : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT)
    {
        fonts.push_back(font);
    }

    void set_variant_fonts(std::vector<ZN_Font> &fonts)
    {
        for (auto &f : fonts)
        {
            this->fonts.push_back(f);
            // this->fonts.push_back(ZN_HB_Font(f.tag, f.path));
        }
    }

    /**
     * @brief Get the type of font.
     *
     * @return FontType enum representing the font type
     */
    FontType get_type()
    {
        return font_type;
    }

    template <typename Func>
    void visit_font(ZN_VariantFont &f, Func func)
    {
        std::visit(func, f);
    }

    /**
     * @brief Load the font into memory and initialize shaping engine.
     *
     * Loads the font using FreeType and sets up the HarfBuzz wrapper. Handles
     * unsupported COLR1 fonts by cleaning up resources and returning an error.
     *
     * @return ZN_ERROR ZN_OK if successful, or appropriate error code
     */
    ZN_ERROR load()
    {

        // _init_ft();

        // ZN_SVG_Font f;
        for (ZN_VariantFont &font : _fonts)
        {

            ZN_ERROR err;
            visit_font(font, [&](auto &font)
                       { err = font.load(); });

            // ZN_ERROR err = font.load();

            if (err)
                return err;
        }

        //   printf("tewiioert %s","fd");

        ZN_Font &font = fonts.front();

        if (selected_tag.empty())
        {
            ZN_ERROR err;
            selected_tag = get_tag(font);
        }

        if (family_name.empty())
        {
            family_name = font.get_family_name();
        }

        inited = true;

        return ZN_OK;
    }

    std::string get_tag(ZN_VariantFont font)
    {

        std::string tag;

        visit_font(font, [&](auto &f)
                   { tag = f.tag; });

        return tag;
    }

    ZN_VariantFont get_font(std::string tag, ZN_RenderFlag type = ZN_RenderFlag::BITMAP)
    {

        for (auto &f : fonts)
        {
            // printf("rewipopwer %s",f.tag.c_str());
            if (f.tag == tag)
            {
                if (type == ZN_RenderFlag::BITMAP)
                {

                    return ZN_HB_Font(f.tag, f.path);
                }
                else if (type == ZN_RenderFlag::SVG)
                {

                    return ZN_SVG_Font(f.tag, f.path);
                }
                // return &f;
            }
        }

        // return nullptr;
    }
    ZN_ERROR use_font(std::string tag)
    {

        for (auto &f : _fonts)
        {
            if (get_tag(f) == tag)
            {
                selected_tag = tag;
                return ZN_OK;
            }
        }

        return ZN_ERR_FONT_TAG_NOT_FOUND;
    }

    // /**
    //  * @brief Shape a UTF-8 text string and generate metrics.
    //  *
    //  * Uses the internal `ZN_HarfbuzzShaper` instance to shape the text and populate
    //  * the provided `ZN_TextMetrics` structure.
    //  *
    //  * @param text UTF-8 text string to shape
    //  * @param metrics Reference to ZN_TextMetrics to fill glyph metrics
    //  * @return ZN_ERROR ZN_OK if successful, or error code from shaping or metrics generation
    //  */

    ZN_ERROR proccess(std::string text, ZN_TextMetrics &metrics) override
    {

        ZN_VariantFont font =
            get_font(selected_tag, metrics.render_flag);

        ZN_ERROR err;

        visit_font(font, [&](auto &font)
                   { err = font.proccess(text.c_str(), metrics); });

        return ZN_OK;
    }

    /**
     * @brief Check whether a glyph index exists.
     *
     * @param glyph_index Index of the glyph
     * @return ZN_BOOL True if index exists, false otherwise
     *
     * @note Simplified check: returns true if index is non-zero
     */
    ZN_BOOL is_index_exist(int glyph_index)
    {
        return glyph_index != 0;
    }

    /**
     * @brief Check whether a specific codepoint exists in this font.
     *
     * @param codepoint Unicode codepoint
     * @return ZN_BOOL True if codepoint exists in the font, false otherwise
     */
    ZN_BOOL is_codepoint_exist(int codepoint)
    {
        ZN_VariantFont font = get_font(selected_tag);

        ZN_BOOL exist = false;

        visit_font(font, [&](auto &font)
                   { exist = font.is_codepoint_exist(codepoint); });
        return exist;
        // return get_font(selected_tag)->is_codepoint_exist(codepoint);

        // return font_harfbuzz.is_codepoint_exist(codepoint);
    }
};
