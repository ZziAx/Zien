#include <ft2build.h>
#include FT_FREETYPE_H

class ZN_EngineContext
{
public:
    static void init()
    {
        get_instance(); // ensures the static instance is created
    }

    static FT_Library &get_ft()
    {
        return get_instance().ft;
    }

private:
    FT_Library ft;

    ZN_EngineContext()
    {
        if (FT_Init_FreeType(&ft))
        {
            throw std::runtime_error("Failed to initialize FreeType");
        }
    }

    ~ZN_EngineContext()
    {
        FT_Done_FreeType(ft);
    }

    static ZN_EngineContext &get_instance()
    {
        static ZN_EngineContext instance;
        return instance;
    }
};
