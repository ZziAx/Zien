class ZN_SvgShaper : public ZN_ShaperObj
{

private:
    std::string svg_data;
    std::vector<ZN_SvgBundle> bundles;
    char *out_svg;
        std::vector<ZN_BoundingBox> bboxes;

    void _shape() override
    {

        // ZN_SvgBuilder;


        out_svg = ZN_GlyphToSvg::to_svg(path, std::string(text));
        // printf("\n\n\nerwuooiuwre %s\n\n\n",out_svg);
        svg_data = std::string(out_svg);
        _load_svg_bundles();
    }

public:
    ZN_MetricsCalculator size_c; ///< Helper class to compute metrics
    ZN_SvgParser parser;

    ZN_SvgShaper() = default;

    ZN_ERROR _init(FT_Face _face)
        override
    {

        face = _face;
    }

    void _load_svg_bundles()
    {

        parser = ZN_SvgParser::from_string(svg_data);

        // printf("\nerwiuyyewr %i\n",bboxes.size());
        bundles = parser.to_bundles();
    }

    static ZN_ERROR from_face(FT_Face _face, ZN_SvgShaper &zn_svg_shaper)
    {
        ZN_ERROR err = zn_svg_shaper._init(_face);
        return err;
    }

    ZN_ERROR load()
    {
    }

    void set_text(const char *_text) override
    {
        text = _text;
    };

    void shape(const char *text) override
    {

        set_text(text);
        size_c = ZN_MetricsCalculator();
        _shape();
    }

    ZN_ERROR get_metrics(ZN_TextMetrics &metric)
        override
    {

        ZN_ERROR err = size_c.compute_font_metrics(face,bundles, metric);

        return err;
    }
};