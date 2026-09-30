/**
 * @brief The ZN_HarfbuzzShaper class handles text shaping using FreeType and HarfBuzz.
 *
 * It loads a font, shapes text, and calculates glyph metrics.
 * This is a lower-level class responsible for handling glyph buffer management,
 * text direction, script, and language setup for proper shaping.
 */
class ZN_HarfbuzzShaper : public ZN_ShaperObj
{
private:
    // -------------------------------
    // Internal State and Resources
    // -------------------------------

    // ZN_TextMetrics metrics; ///< Holds calculated text metrics (width, height, etc.)
    // FT_Face face;           ///< The loaded FreeType font face

    hb_buffer_t *hb_buffer;      ///< HarfBuzz buffer used for shaping
    hb_font_t *hb_font;          ///< HarfBuzz font created from FreeType face
    hb_glyph_info_t *glyph_info; ///< Pointer to glyph info array after shaping

    // -------------------------------
    // Private Methods
    // -------------------------------

    /**
     * @brief Runs HarfBuzz shaping process.
     *
     * Uses the current hb_font and hb_buffer to produce glyph positions
     * and glyph info, which can be used for further layout and rendering.
     */
    void _shape()
        override
    {


        hb_shape(hb_font, hb_buffer, nullptr, 0);
        glyph_info = hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
    }

    /**
     * @brief Sets the font pixel size.
     *
     * @param size Desired font size in pixels.
     */
    void _set_size(double size)
        override
    {

        FT_Set_Pixel_Sizes(face, 0, fnts); // 'fnts' should be your desired size variable
    }

    ZN_ERROR _init(FT_Face _face)
        override
    {
        face = _face;

        _set_size(fnts);

        hb_font = hb_ft_font_create_referenced(face);

        hb_buffer = hb_buffer_create();
        hb_buffer_set_unicode_funcs(hb_buffer, hb_unicode_funcs_get_default());

        return ZN_OK;
    }

    /**
     * @brief Clears and reinitializes the shaping environment.
     *
     * Useful when changing text or resetting state between shaping runs.
     */
    void _clear()
        override
    {
        dispose();
    }

    /**
     * @brief Sets text direction for HarfBuzz buffer.
     *
     * Defaults to Left-To-Right (HB_DIRECTION_LTR).
     */
    void _set_direction()
    {
        hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
    }

    /**
     * @brief Sets script for HarfBuzz shaping.
     *
     * Currently hardcoded to Arabic; should be dynamic for multilingual support.
     */
    void _set_script()
    {
        hb_buffer_set_script(hb_buffer, HB_SCRIPT_ARABIC);
    }

    /**
     * @brief Sets the language for HarfBuzz shaping.
     *
     * Defaults to English ("en").
     */
    void _set_lang()
    {
        hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
    }

public:
    ZN_HarfbuzzShaper() = default;
    // -------------------------------
    // Public API
    // -------------------------------

    static ZN_ERROR from_face(FT_Face _face, ZN_HarfbuzzShaper &zn_font_harfbuzz)
    {
        ZN_ERROR err = zn_font_harfbuzz._init(_face);
        return err;
    }

    /**
     * @brief Sets the text to shape and adds it to the HarfBuzz buffer.
     *
     * @param text UTF-8 encoded string.
     */
    void set_text(const char *text)
        override
    {
        this->text = text;
        hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);
    }

    /**
     * @brief Disposes HarfBuzz resources (buffer and font).
     *
     * Should be called before destruction or reinitialization.
     */
    void dispose()
        override
    {
        hb_buffer_destroy(hb_buffer);
        hb_font_destroy(hb_font);
        hb_buffer = nullptr;
        hb_font = nullptr;
    }

    /**
     * @brief Performs full shaping process on the provided text.
     *
     * Clears buffer, reinitializes state, sets direction, script, and language,
     * then calls HarfBuzz to shape the text.
     *
     * @param _text Text to shape.
     */
    void shape(const char *_text)
        override
    {

        _clear();
        _init(face);

        text = _text;

        size_c = ZN_MetricsCalculator();

        set_text(text);
        _set_direction();
        _set_script();
        _set_lang();
        _shape();
    }

    ZN_ERROR get_metrics(ZN_TextMetrics &metric)
        override
    {
        // printf("ueirwoioewr %s", face->family_name);

        ZN_ERROR err = size_c.compute_font_metrics(hb_buffer, face, text, hb_font, metric);

        return err;
    }
};
