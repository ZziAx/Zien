
class ZN_StyleObj
{
public:
    FT_Face face = nullptr;
    bool enabled;

    ZN_StyleObj() : enabled(true) {}

    ZN_StyleObj(bool enabled) : face(nullptr), enabled(enabled) {}

    virtual void apply() {}

    virtual void load_face(FT_Face &_face)
    {
        face = _face;
    }
};

// printf("reuwoiuioewr %s",face->family_name);
