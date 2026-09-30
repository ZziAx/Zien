#include <freetype/ftcolor.h>
#include <freetype/tttables.h>
#include "zn_glyph_data_builder.h"
// #include <chrono>
#include <ctime>

/**
 * @brief ZN_MetricsCalculator calculates glyph metrics and positions for a given text using FreeType and HarfBuzz.
 *
 * This class is responsible for:
 *  - Determining clusters of glyphs
 *  - Calculating sizes and positions for glyphs
 *  - Building glyph metrics (including emoji support)
 *
 * Typically used internally in a text rendering pipeline.
 */
class ZN_MetricsCalculator
{
private:
    hb_glyph_position_t *glyph_pos; ///< HarfBuzz array of glyph positions
    hb_glyph_info_t *glyph_info;    ///< HarfBuzz array of glyph info
    unsigned int glyph_count;       ///< Number of glyphs in the buffer

    /**
     * @brief Extract a single UTF-8 character from original text given a cluster index.
     * @param original_text Original UTF-8 text
     * @param cluster Cluster index
     * @return UTF-8 string of one character
     */
    std::string _get_string(const char *original_text, uint32_t cluster)
    {
        return ZN_UtfHelper::get_utf8_char(original_text, cluster);
    }

    /**
     * @brief Extract substring of text for cluster range.
     * @param original_text Original UTF-8 text
     * @param c0 Start cluster
     * @param c1 End cluster
     * @return Substring in UTF-8
     */
    std::string _get_string(const char *original_text, int c0, int c1)
    {
        std::string ns = "";
        for (int n = c0; n < c1; n++)
        {
            std::string ch = ZN_MetricsCalculator::_get_string(original_text, n);
            if (ZN_UtfHelper::next_codepoint(ch.c_str()) == 0)
                continue;
            ns += ch;
        }
        return ns;
    }

    /**
     * @brief Get string corresponding to the i-th glyph in the HarfBuzz buffer.
     * @param i Glyph index
     * @param original_text Original text
     * @param range Output: cluster range for this glyph
     * @return UTF-8 substring for the glyph
     */
    std::string _get_string(int i, const char *original_text, ClusterRange &range)
    {
        range = _get_cluster_range(i);
        return _get_string(original_text, range.start, range.end);
    }

    /**
     * @brief Check if a font face uses COLR (color) tables and which version.
     * @param face FT_Face pointer
     * @return FontType: NORMAL, COLR0, COLR1
     */
    FontType _check_colr_version(FT_Face face)
    {
        FT_Bytes colr_table;
        FT_ULong length = 0;

        if (FT_Load_Sfnt_Table(face, FT_MAKE_TAG('C', 'O', 'L', 'R'), 0, nullptr, &length) == 0)
        {
            std::vector<unsigned char> buffer(length);
            if (FT_Load_Sfnt_Table(face, FT_MAKE_TAG('C', 'O', 'L', 'R'), 0, buffer.data(), &length) == 0)
            {
                uint16_t version = (buffer[0] << 8) | buffer[1];

                switch (version)
                {
                case 0:
                    return FontType::COLR0;
                case 1:
                    return FontType::COLR1;
                }
            }
        }

        return FontType::NORMAL;
    }

    /**
     * @brief Initialize internal glyph info and positions arrays from HarfBuzz buffer.
     * @param hb_buffer HarfBuzz buffer containing shaped text
     */
    void _init(hb_buffer_t *hb_buffer)
    {
        glyph_info = hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
        glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, &glyph_count);
    }

    /**
     * @brief Get cluster index of the last glyph in the buffer.
     * @return Last cluster index
     */
    int _get_last_cluster()
    {
        return glyph_info[glyph_count - 1].cluster;
    }

    /**
     * @brief Get the start and end cluster for a glyph at given index.
     * @param index Glyph index
     * @return ClusterRange for the glyph
     */
    ClusterRange _get_cluster_range(int index)
    {
        int c0 = glyph_info[index].cluster;
        int c1 = (index == glyph_count - 1) ? _get_last_cluster() + 1 : glyph_info[index + 1].cluster;
        return ClusterRange(c0, c1);
    }

public:
#if ZN_TINYXML_IMPLEMENTATION

    /**
     * @brief Special calculation for emoji glyphs.
     * @param text UTF-8 text
     * @param metric Output metrics object to populate
     * @return ZN_ERROR code
     */
    static int compute_svgb_metrics(const char *text, ZN_TextMetrics &metric)
    {

        std::string t = std::string(text);
        ZN_SvgbParser emoji_parser = EmojiManager::instance().get_parser_ref();
        int str_count = ZN_UtfHelper::utf8_char_count(text);

        metric.glyph_count += str_count;

        for (unsigned int i = 0; i < str_count; ++i)
        {
            ZN_GlyphData glyph;
            std::string hex = ZN_UtfHelper::to_hex(ZN_UtfHelper::char_at(t, i)).c_str();
            ZN_GlyphDataBuilder::build_emoji(metric, hex.c_str(), glyph);
            metric.glyphs.push_back(glyph);
        }
        return ZN_OK;
    }

#endif

    bool _is_marked(int codepoint)
    {

        return codepoint == 8294 || codepoint == 8297 || codepoint == 8295;

        // // Check for format/invisible characters
        // if ((codepoint >= 0x200B && codepoint <= 0x200F) || // Zero Width Space, LRM, RLM
        //     (codepoint >= 0x202A && codepoint <= 0x202E) || // LRE, RLE, PDF, LRO, RLO
        //     (codepoint >= 0x2060 && codepoint <= 0x2064) || // Word Joiner, etc.
        //     (codepoint >= 0xFE00 && codepoint <= 0xFE0F) || // Variation Selectors
        //     (codepoint >= 0xFFF0 && codepoint <= 0xFFF8))   // Specials
        // {
        //     return true;
        // }
        // return false;
    }

    /**
     * @brief Computes glyph metrics and layout information for a given UTF-8 text string.
     *
     * This method iterates over the HarfBuzz-shaped glyphs in the provided buffer,
     * calculates positions, clusters, and other relevant layout data, and populates
     * the provided `ZN_TextMetrics` structure. It handles special cases like emojis
     * and color fonts where applicable.
     *
     * @param hb_buffer Pointer to a HarfBuzz buffer containing shaped text glyphs.
     * @param face FreeType face object representing the font used.
     * @param original_text Pointer to the original UTF-8 encoded text string.
     * @param hb_font Pointer to the HarfBuzz font associated with the `face`.
     * @param metric Reference to a `ZN_TextMetrics` object where glyph metrics and layout
     *               information will be stored.
     * @return ZN_ERROR Returns `ZN_OK` on success, or an appropriate error code if
     *                  a failure occurs during metric calculation.
     *
     * @note This method assumes that `hb_buffer` has already been shaped for the
     *       input text using HarfBuzz. It does not modify the font face itself.
     * @note Emoji and special glyphs are handled via `ZN_GlyphDataBuilder`.
     */
    int idx = 0;
    clock_t start = clock();

    // auto start = std::chrono::high_resolution_clock::now();

    ZN_ERROR compute_font_metrics(FT_Face face,std::vector<ZN_SvgBundle> svg_glyphs, ZN_TextMetrics &metric)
    {

        for (int i = 0; i < svg_glyphs.size(); i++)
        {
            ZN_GlyphData glyph;
            ZN_SvgBundle & b = svg_glyphs[i];
            ZN_GlyphDataBuilder::build_svg(face, metric,"",FontType::NORMAL,b,glyph);
            metric.glyphs.push_back(glyph);
        }

        // svg.path
    }

    ZN_ERROR compute_font_metrics(hb_buffer_t *hb_buffer, FT_Face face, const char *original_text, hb_font_t *hb_font, ZN_TextMetrics &metric)
    {
        _init(hb_buffer);

        metric.glyph_count += glyph_count;
        metric.face = face;

        int str_count = ZN_UtfHelper::utf8_char_count(original_text);

        for (unsigned int i = 0; i < glyph_count; ++i)
        {

            ZN_GlyphData glyph;

// #ifdef ZN_CACHE_ENABLE
//             ZN_CacheManager cache_manager = ZN_CacheManager::instance();
//             ZN_BOOL found = cache_manager.get_glyph(glyph_info[i].codepoint, glyph);

//             if (found)
//             {
//                 metric.glyphs.push_back(glyph);
//                 continue;
//             }

// #endif

            GlyphHelper::load_glyph(face, glyph_info[i].codepoint, metric.style);

            ClusterRange range;

            std::string content = _get_string(i, original_text, range);

           
            switch (metric.render_flag)
            {
            case ZN_RenderFlag::BITMAP:
            {
                ZN_GlyphDataBuilder::build_glyph(face, i, metric, content, _check_colr_version(face), glyph_pos, glyph_info, glyph);
                break;
            }

            case ZN_RenderFlag::SVG:
            {
                printf("\nsvg type\n");
                ZN_GlyphDataBuilder::build_svg(face, i, metric, content, _check_colr_version(face), glyph_pos, glyph_info, glyph);
            }
            }

            int cd = ZN_UtfHelper::next_codepoint(glyph.character.c_str());

            glyph.charcode = cd;

            // skip invisible/formatting characters
            if (_is_marked(cd))
                continue;

            glyph.cluster_range = range;

            metric.glyphs.push_back(glyph);
        }

        return ZN_OK;
    }
};
