class ZN_HandlerObj
{

    friend class ZN_TextShaper;

protected:
    ZN_TextCanvas *canvas;

public:
    ZN_RenderFlag type;
    ZN_TextShaper *shaper;
    ZN_TextLayout *layout;

    ZN_HandlerObj() = default;
    ZN_HandlerObj(ZN_RenderFlag type) : type(type) {}

    ZN_TextLayout *create_layout();

    void set_shaper(ZN_TextShaper *_shaper)
    {
        shaper = _shaper;
    }

    void set_layout(ZN_TextLayout *_layout)
    {

        layout = _layout;
    }

    void set_painter(ZN_TextCanvas *_canvas)
    {
        canvas = _canvas;
    }

    ZN_TextLayout *get_layout()
    {
        return layout;
    }
};