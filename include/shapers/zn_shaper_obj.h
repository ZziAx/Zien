

// template <typename T = ZN_HarfbuzzShaper>
class ZN_ShaperObj
{

public:
ZN_ShaperObj()=default;
std::string path;
    virtual void set_text(const char *_text){
        text = _text;
    };
    virtual void dispose() {};
    virtual void shape(const char *text) {};

    FT_Face face = nullptr;
    ZN_TextMetrics metrics;
    ZN_MetricsCalculator size_c; ///< Helper class to compute metrics
    unsigned int glyph_count;    ///< Number of glyphs after shaping
    const char *font_path;       ///< Path to the loaded font file (not owned)
    const char *text;            ///< Text currently being shaped (not owned)

    /**
     * @brief Checks if a given Unicode codepoint exists in the font.
     *
     * @param codepoint Unicode codepoint to check.
     * @return ZN_BOOL Returns ZN_CODEPOINT_FOUND or ZN_CODEPOINT_NOT_FOUND.
     */

    ZN_BOOL is_codepoint_exist(int codepoint)
    {
        FT_ULong charcode;
        FT_UInt gindex;
        // return ZN_OK;
        int _codepoint = FT_Get_First_Char(face, &gindex);

        while (gindex != 0)
        {

            if (_codepoint == codepoint)
            {

                return ZN_CODEPOINT_FOUND;
            }

            _codepoint = FT_Get_Next_Char(face, _codepoint, &gindex);
        }

        //   printf("\nTEST2 %s\n",_codepoint);
        // return ZN_OK;
        return ZN_CODEPOINT_NOT_FOUND;
    }

    /**
     * @brief Returns the current text metrics.
     *
     * @return ZN_TextMetrics Copy of metrics struct.
     */
    virtual ZN_TextMetrics get_metrics()
    {
        return metrics;
    }

    /**
     * @brief Generates text metrics for the current shaped text.
     *
     * @param metric Reference to metrics structure to fill.
     * @return ZN_ERROR Returns `ZN_OK` on success or an appropriate error code
     *                  if metric computation fails.
     */
    virtual ZN_ERROR get_metrics(ZN_TextMetrics &metric) {};
    

private:
    virtual void _shape() {};
    virtual void _set_size(double size) {};
    virtual ZN_ERROR _init(FT_Face _face) {return ZN_OK;};
    virtual void _clear() {};

    FT_ULong _get_codepoint_from_glyph_index(int glyph_index)
    {
        FT_ULong charcode;
        FT_UInt gidx;
        charcode = FT_Get_First_Char(face, &gidx);
        while (gidx != 0)
        {
            if (gidx == glyph_index)
            {
                break;
            }
            charcode = FT_Get_Next_Char(face, charcode, &gidx);
        }
        return charcode;
    }
};