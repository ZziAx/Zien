

/**
 * @class ZN_TextShaper
 * @brief Handles text shaping, direction detection, and metric calculation.
 *
 * The `ZN_TextShaper` class is responsible for converting UTF-8 text into shaped
 * glyphs using font data from `ZN_FontCollection`. It automatically detects
 * text direction (LTR or RTL), applies Unicode directional isolation marks,
 * and uses a BiDi parser to ensure proper visual ordering of mixed-direction text.
 */
class ZN_TextShaper
{
    friend class ZN_TextLayout;

private:
    ZN_TextStyle *style;

    /**
     * @brief HarfBuzz-based text shaper helper.
     * Used internally for low-level glyph shaping.
     */
    // ZN_HarfbuzzShaper harfbuzz_shaper;

    /**
     * @brief Reference to a font collection used for shaping and fallback.
     */
    ZN_FontCollection &font_collection;

    /**
     * @brief Total number of glyphs produced during shaping.
     */
    unsigned int glyph_count;

    ZN_RenderFlag render_flag = ZN_RenderFlag::BITMAP;

public:
    ZN_TextShaper();

    /**
     * @brief Holds metrics of the most recently shaped text.
     */
    ZN_TextMetrics text_metrics = ZN_TextMetrics();
    ZN_HandlerObj *handler;

    /**
     * @brief Constructs a text shaper bound to a given font collection.
     *
     * @param font_collection Reference to the font collection that provides
     *                        font and emoji bundles.
     */
    explicit ZN_TextShaper(ZN_FontCollection &font_collection, ZN_TextStyle *style, ZN_HandlerObj *handler)
        : font_collection(font_collection), style(style), handler(handler)
    {
        this->render_flag = handler->type;
        text_metrics.render_flag = this->render_flag;
    }

    /**
     * @brief Returns metrics for the most recently shaped text.
     *
     * @return A `ZN_TextMetrics` structure containing text layout information.
     */
    ZN_TextMetrics get_metrics()
    {
        return text_metrics;
    }

    /**
     * @brief Shapes a UTF-8 text string and calculates layout metrics.
     *
     * This method performs the following steps:
     * - Detects text direction (LTR or RTL).
     * - Wraps the string with Unicode directional isolation markers.
     * - Passes the string through a BiDi parser for correct ordering.
     * - Calls `ZN_FontCollection::shape_text()` to perform shaping.
     *
     * @param text Pointer to a UTF-8 encoded text string.
     * @return `ZN_OK` on success, or an error code on failure.
     */
    ZN_ERROR shape_text(const char *text)
    {

        // style->set_ft_library(font_collection.ft);

        // Detect text direction (e.g., Arabic = RTL, English = LTR)
        int direction = ZN_UtfHelper::get_direction(text);

        // Add directional isolation marks:
        // \u2066 = LTR isolate, \u2067 = RTL isolate, \u2069 = end isolate
        

        const char *out;

        if (render_flag == ZN_RenderFlag::SVG)
        {
            // Reverse the string for SVG rendering

            out = text;;
        }
        else
        {
            std::string s =
            (direction == ZN_DIRECTION_RTL ? "\u2067" : "\u2066") +
                std::string(text) + "\u2069";
                out = BidiParser::parse(s.c_str());

               
        }

        // Apply BiDi algorithm to reorder mixed-direction text

        // Store detected direction in text metrics
        text_metrics.direction = direction;

        // Perform shaping using the font collection
        ZN_ERROR err = font_collection.shape_text(out, text_metrics, style);

        handler->set_shaper(this);

        return err;
    }
};
