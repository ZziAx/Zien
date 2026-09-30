class ZN_CacheManager
{
public:
    std::vector<ZN_GlyphData> glyphs;

    static ZN_CacheManager &instance()
    {
        static ZN_CacheManager instance;
        return instance;
    }

    void put_glyph(ZN_GlyphData &glyph)
    {
        if (glyph.codepoint == 0)
            return;

        
        glyphs.push_back(glyph);

    }

    ZN_BOOL get_glyph(int codepoint, ZN_GlyphData &glyph)
    {

        for (ZN_GlyphData &g : glyphs)
        {

            if (g.codepoint == codepoint)
            {
                glyph = g;
                return ZN_CACHE_GLYPH_FOUND;
            }
        }

        return ZN_CACHE_GLYPH_NULL;
    }
};