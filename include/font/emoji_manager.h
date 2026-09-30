

#if ZN_TINYXML_IMPLEMENTATION
class EmojiManager
{

private:
    std::unique_ptr<ZN_SvgbParser> parser;

    EmojiManager() = default; // private ctor
public:
    bool loaded = false;

    static EmojiManager &instance()
    {
        static EmojiManager inst; // created once, thread-safe since C++11
        return inst;
    }

    ZN_ERROR load(const char *path)
    {
        if (loaded)
            return ZN_OK;

        ZN_ERROR err = ZN_SvgbParser::load(path, parser);

        if (!err)
        {
            loaded = true;
        }
        return err;
    }

    std::unique_ptr<ZN_SvgbParser> get_ptr()
    {
        return std::move(parser); // raw pointer for external use
    }

    ZN_SvgbParser &get_parser_ref()
    {
        return *parser;
    }
};


#endif