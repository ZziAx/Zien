

/**
 * @class ZN_TextMetrics
 * @brief Stores glyph layout and rendering metrics for a shaped text string.
 *
 * This class encapsulates all relevant data needed to render or measure text,
 * including individual glyph metrics, bounding boxes, overall width/height,
 * text direction, and the number of glyphs. Typically populated by shaping
 * engines like `ZN_HarfbuzzShaper` and `ZN_MetricsCalculator`.
 */
class ZN_TextMetrics
{
public:
    /**
     * @brief Default constructor. Initializes an empty metrics object.
     */

    ZN_TextStyle *style = nullptr;

    ZN_TextMetrics(ZN_TextStyle *style = nullptr):style(style){};

    std::vector<ZN_GlyphData> glyphs = {}; ///< Collection of glyph data for the shaped text
    FT_Face face;                       ///< FreeType font face associated with the metrics


    ZN_RenderFlag render_flag = ZN_RenderFlag::BITMAP;

    int min_x = 0;       ///< Minimum X coordinate of the text bounding box
    int min_y = 0;       ///< Minimum Y coordinate of the text bounding box
    int max_x = 0;       ///< Maximum X coordinate of the text bounding box
    int max_y = 0;       ///< Maximum Y coordinate of the text bounding box
    int width = 0;       ///< Total width of the text
    int height = 0;      ///< Total height of the text
    int bitmap_top = 0;  ///< Top offset for rendering
    int bitmap_left = 0; ///< Left offset for rendering

    ZN_DIRECTION direction = ZN_DIRECTION_RTL; ///< Text direction (RTL/LTR)

    unsigned int glyph_count = 0; ///< Total number of glyphs

    /**
     * @brief Merge another metrics object into this one.
     *
     * Combines the glyphs of the provided `metrics` object into this object.
     * Can either append at the end or insert at the beginning depending on `push_back`.
     *
     * @param metrics The `ZN_TextMetrics` object to merge from
     * @param push_back If true, append glyphs at the end; otherwise, insert at the beginning
     */
    void merge_metrics(ZN_TextMetrics metrics, bool push_back = true)
    {
        if (push_back)
        {
            glyphs.insert(glyphs.end(), metrics.glyphs.begin(), metrics.glyphs.end());
        }
        else
        {
            glyphs.insert(glyphs.begin(), metrics.glyphs.begin(), metrics.glyphs.end());
        }
    }
};
