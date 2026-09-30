// #include "emoji_manager.h"
#ifdef ZN_TINYXML_IMPLEMENTATION
using namespace tinyxml2;

class ZN_SvgbParser
{

private:
    ZN_ERROR _parse()
    {

        // ZN_SvgbEmojiSlot::parse();


        XMLElement *root = doc.RootElement(); // <svg>
        XMLElement *defs = root->FirstChildElement("defs");
        for (XMLElement *symbol = defs->FirstChildElement("symbol");
             symbol != nullptr;
             symbol = symbol->NextSiblingElement("symbol"))
        {
            const char *unicode = symbol->Attribute("data-unicode");
            const char *id = symbol->Attribute("id");

            if (unicode && id)
            {
                put_element(symbol);
            }
        }
    }

public:
    // XMLDocument* doc = nullptr;

    XMLDocument &doc;

    std::vector<ZN_SvgbEmojiSlot> emojies;

    ZN_SvgbParser() = default;

    ZN_SvgbParser(XMLDocument &doc) : doc(doc) {
                                      };

    ZN_SvgbEmojiSlot *get_slot(const char *unicode, bool &success)
    {

        success = false;

        for (int i = 0; i < emojies.size(); i++)
        {
            ZN_SvgbEmojiSlot *slot = &emojies[i];

            if (slot->svg_data != "" && is_unicode(unicode, slot->unicode))
            {
                // printf("get slot for %s\n", unicode);

                success = true;
                return slot;
            }
        }

        return nullptr;
    }

    std::string get_svg_data(char *unicode, bool &success)
    {

        success = false;
        ZN_SvgbEmojiSlot *slot = get_slot(unicode, success);
        if (success)
        {
            return slot->svg_data;
        }
        return "";
    }

    bool is_exist(int codepoint)
    {

        std::string hex = ZN_UtfHelper::to_hex(codepoint);
        const char *unicode = hex.c_str();

        bool exist = false;

        // return false;

        size_t length = emojies.size();

        for (int i = 0; i < length; i++)
        {
            ZN_SvgbEmojiSlot e = emojies[i];

            if (is_unicode(unicode, e.unicode))
            {
                exist = true;
                break;
            }
        }

        return exist;
    }

    bool is_exist(const char *unicode)
    {

        bool exist = false;

        size_t length = emojies.size();

        for (int i = 0; i < length; i++)
        {
            ZN_SvgbEmojiSlot e = emojies[i];

            if (is_unicode(unicode, e.unicode))
            {
                exist = true;
                break;
            }
        }

        return exist;
    }

    void put_element(XMLElement *element)
    {
        ZN_SvgbEmojiSlot slot = ZN_SvgbEmojiSlot(*element);
        emojies.push_back(slot);
    }

    static ZN_ERROR load(const char *path, std::unique_ptr<ZN_SvgbParser> &parser)
    {
        auto doc = std::make_unique<XMLDocument>();
        doc->LoadFile(path);
        parser = std::make_unique<ZN_SvgbParser>(*doc);
        parser->_parse();
        return ZN_OK;
    }

    bool is_unicode(const char *unicode, std::string s_unicode)
    {
        return ((s_unicode == std::string(unicode)) || (s_unicode == std::string("U+") + std::string(unicode)));
    }
};

#endif