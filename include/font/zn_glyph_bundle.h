/**
 * @brief Enum representing the type of glyph object.
 */
enum ZN_GlyphType
{
    GLYPH_FONT, ///< Standard font glyph
    GLYPH_EMOJI ///< Emoji glyph
};

/**
 * @brief Base class for glyph objects.
 *
 * This class is the polymorphic base for all glyph types
 * such as fonts and emojis. It stores the glyph type and provides
 * a virtual interface for processing text and resource cleanup.
 */
class ZN_GlyphObject
{
private: /**
          * @brief Virtual destructor for safe polymorphic deletion.
          */
    // virtual ~ZN_GlyphObject() = default;

    /**
     * @brief Destroy or clean up resources.
     *
     * Override in derived classes if cleanup is required.
     *
     * @return ZN_ERROR Returns ZN_OK by default
     */
    virtual ZN_ERROR destroy()
    {
        return ZN_OK;
    }

public:
ZN_GlyphObject()=default;
    ZN_GlyphType type; ///< Type of the glyph
    /**
     * @brief Constructor
     * @param type Type of glyph
     */
    ZN_GlyphObject(ZN_GlyphType type)
        : type(type)
    {
    }

    /**
     * @brief Process text and populate metrics.
     *
     * Derived classes override this to implement specific text processing logic.
     *
     * @param text UTF-8 text to process
     * @param metrics Reference to metrics object to populate
     * @return ZN_ERROR Returns ZN_OK by default
     */
    virtual ZN_ERROR proccess(std::string text, ZN_TextMetrics &metrics)
    {
        return ZN_OK;
    }

    // virtual void is_codepoint_exist(int codepoint) = 0;
};
