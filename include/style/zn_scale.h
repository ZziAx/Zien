#include "freetype/ftoutln.h"

class ZN_Scale : public ZN_StyleObj
{

public:
    float scale;

    ZN_Scale() = default;

    ZN_Scale(float scale, bool enabled) : scale(scale), ZN_StyleObj(enabled) {}

    void set_scale(float _scale)
    {
        scale = _scale;
    }

    void apply()
        override
    {
 

       

    }
};