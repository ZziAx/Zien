class ZN_TextStyle
{

public:
    ZN_TextStyle() = default;

    ZN_Bold bold;
    ZN_Italic italic;
    ZN_Scale scale;
    ZN_Stroker stroker;

    std::vector<ZN_StyleObj*> styles = {&bold};

    std::vector<ZN_StyleObj*> pre_load_styles = {&italic};


    void pre_load(FT_Face &face)
    {
        for(auto& style : pre_load_styles){
            if(style->enabled){
                style->load_face(face);
                style->apply();
            }
        }

    }

    void load(FT_Face &face)
    {

         for(auto& style : styles){
            if(style->enabled){
                style->load_face(face);
                style->apply();
            }
        }
        // scale.load_face(face);
        // scale.apply();
        // stroker.load_face(face);
        // stroker.apply();
    }
};