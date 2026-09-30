#if ZN_TINYXML_IMPLEMENTATION

/**
 * @brief Represents an emoji glyph as a specialized glyph object.
 *
 * This class inherits from ZN_GlyphObject and is specifically designed
 * to handle emoji characters. It stores the emoji codepoint and provides
 * a method to process emoji text and populate text metrics.
 *
 * Typically used in a text rendering pipeline to handle emoji layout
 * and metrics alongside normal glyphs.
 */
class ZN_EmojiBundle : public ZN_GlyphObject
{
public:
    int codepoint = 0; ///< Unicode codepoint of the emoji

    /**
     * @brief Default constructor.
     *
     * Initializes the object as a GLYPH_EMOJI type glyph.
     */
    ZN_EmojiBundle()
        : ZN_GlyphObject(ZN_GlyphType::GLYPH_EMOJI) {};

    /**
     * @brief Constructs an emoji bundle for a specific codepoint.
     *
     * @param codepoint Unicode codepoint of the emoji.
     */
    ZN_EmojiBundle(int codepoint)
        : codepoint(codepoint), ZN_GlyphObject(ZN_GlyphType::GLYPH_EMOJI) {};

    /**
     * @brief Processes emoji text and updates metrics.
     *
     * This method computes metrics for the given UTF-8 text using the
     * `ZN_MetricsCalculator::compute_svgb_metrics` utility, which handles layout
     * and sizing of emojis. Results are stored in the provided `ZN_TextMetrics`.
     *
     * @param text UTF-8 string containing emoji characters.
     * @param metrics Reference to a ZN_TextMetrics object to populate.
     * @return ZN_ERROR Returns `ZN_OK` on success or an error code from
     *                  `ZN_MetricsCalculator::compute_svgb_metrics`.
     */
    ZN_ERROR proccess(std::string text, ZN_TextMetrics &metrics) override
    {

        ZN_ERROR err = ZN_MetricsCalculator::compute_svgb_metrics(text.c_str(), metrics);
        return err;
    }
};

#endif