
// class ZN_Font : public ZN_GlyphObject
// {
// public:
// ZN_Font() = default;
// ZN_Font(std::string tag, std::string path) : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT), tag(tag), path(path) {};
// ZN_Font(std::string tag, std::vector<unsigned char> bytes) : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT), tag(tag), path(path) {};

// const std::string path;
// const std::vector<unsigned char> bytes;
// const std::string tag;
//     virtual ZN_ERROR load(std::string path) = 0;
//     virtual ZN_ERROR load(std::vector<unsigned char> bytes) = 0;
//     virtual ZN_ERROR load() = 0;
//     virtual std::string get_family_name() = 0;
//     virtual ZN_BOOL is_codepoint_exist(int codepoint) = 0;
//     virtual ZN_ERROR proccess(const char *text, ZN_TextMetrics &metrics) = 0;
// };
#include <variant>

class ZN_Base_Font : public ZN_GlyphObject
{

public:
    bool loaded = false;
    FT_Face face = nullptr;
    virtual ZN_ERROR load() = 0;
    virtual ZN_BOOL is_codepoint_exist(int codepoint) = 0;

private:
    virtual ZN_ERROR load(std::string path) = 0;
    virtual ZN_ERROR load(std::vector<unsigned char> bytes) = 0;

public:
    const std::string path;
    const std::vector<unsigned char> bytes;
    const std::string tag;

    ZN_Base_Font() = default;
    ZN_Base_Font(std::string tag, std::string path) : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT), tag(tag), path(path) {};
    ZN_Base_Font(std::string tag, std::vector<unsigned char> bytes) : ZN_GlyphObject(ZN_GlyphType::GLYPH_FONT), tag(tag), path(path) {};
};

class ZN_Font : public ZN_Base_Font
{

public:
    bool loaded = false;
    FT_Face face = nullptr;
    ZN_ERROR load()
        override
    {

        if (loaded)
            return ZN_OK;

        if (path != NULL_STR)
        {
            return load(path);
        }

        if (bytes.empty() == false)
        {
            return load(bytes);
        }

        return ZN_ERR_INVALID_FONT;
    }

    ZN_BOOL is_codepoint_exist(int codepoint) override
    {
        return false;
    };

    std::string get_family_name()
    // override
    {
        if (!loaded)
        {
            load();
        }
        return std::string(face->family_name);
    }

private:
    ZN_ERROR load(std::string path)
        override
    {
        FT_Library ft = ZN_EngineContext::get_ft();

        FT_Error error = FT_New_Face(ft, path.c_str(), 0, &face);
        return ZN_OK;
    }

    ZN_ERROR load(std::vector<unsigned char> bytes)
        override
    {

        FT_Library ft = ZN_EngineContext::get_ft();
        FT_Error error = FT_New_Memory_Face(
            ft,
            bytes.data(), // pointer to font bytes
            bytes.size(), // size in bytes
            0,            // face index (0 for most fonts)
            &face);

        if (error)
        {
            return ZN_ERR_INVALID_FONT;
        }

        return ZN_OK;
    }

public:
    const std::string path;
    const std::vector<unsigned char> bytes;
    const std::string tag;
    ZN_Font() = default;
    ZN_Font(std::string tag, std::string path) : tag(tag), path(path) {};
    ZN_Font(std::string tag, std::vector<unsigned char> bytes) : tag(tag), path(path) {};
};

template <typename ZN_Shaper>
class ZN_T_Font : public ZN_Font
{
private:
    ZN_Shaper shaper;


    ZN_ERROR load(std::string path)
        override
    {

        FT_Library ft = ZN_EngineContext::get_ft();

        FT_Error error = FT_New_Face(ft, path.c_str(), 0, &face);

        if (error)
        {
            return ZN_ERR_INVALID_FONT;
        }

        ZN_ERROR err = ZN_Shaper::from_face(face, shaper);
        shaper.path = path;

        if (!err)
        {
            loaded = true;
            return ZN_OK;
        }

        return err;
    }

    ZN_ERROR load(std::vector<unsigned char> bytes)
        override
    {

        FT_Library ft = ZN_EngineContext::get_ft();
        FT_Error error = FT_New_Memory_Face(
            ft,
            bytes.data(), // pointer to font bytes
            bytes.size(), // size in bytes
            0,            // face index (0 for most fonts)
            &face);

        if (error)
        {
            return ZN_ERR_INVALID_FONT;
        }

        ZN_ERROR err = ZN_Shaper::from_face(face, shaper);
        if (!err)
        {
            loaded = true;
            return ZN_OK;
        }

        return err;
    }

public:
    const std::string path;
    const std::vector<unsigned char> bytes;
    const std::string tag;
   
    ZN_T_Font() = default;
    ZN_T_Font(const std::string tag, const std::string path) : tag(tag), path(path) {};
    ZN_T_Font(const std::string tag, const std::vector<unsigned char> bytes) : tag(tag), path(path) {};

    ZN_ERROR load()
        override
    {

        if (loaded)
            return ZN_OK;

        if (path != NULL_STR)
        {
            return load(path);
        }

        if (bytes.empty() == false)
        {
            return load(bytes);
        }

        return ZN_ERR_INVALID_FONT;
    }

    /**
     * @brief Shape a UTF-8 text string and generate metrics.
     *
     * Uses the internal `ZN_HarfbuzzShaper` instance to shape the text and populate
     * the provided `ZN_TextMetrics` structure.
     *
     * @param text UTF-8 text string to shape
     * @param metrics Reference to ZN_TextMetrics to fill glyph metrics
     * @return ZN_ERROR ZN_OK if successful, or error code from shaping or metrics generation
     */
    ZN_ERROR proccess(const char *text, ZN_TextMetrics &metrics)
    // override
    {

        if (!loaded)
            load();



        
        shaper.shape(text);

        ZN_ERROR err = shaper.get_metrics(metrics);

        return err;
    }

    ZN_BOOL is_codepoint_exist(int codepoint)
    // override
    {

        if (!loaded)
            load();

        return shaper.is_codepoint_exist(codepoint);
    }
};

// using ZN_Font = ZN_T_Font<ZN_HarfbuzzShaper>;

// struct ZN_Font{

// };

// using ZN_Font = ZN_T_Font<ZN_Fontt>;

using ZN_HB_Font = ZN_T_Font<ZN_HarfbuzzShaper>;

using ZN_SVG_Font = ZN_T_Font<ZN_SvgShaper>;

using ZN_VariantFont = std::variant<ZN_HB_Font,ZN_SVG_Font ,ZN_Font>;