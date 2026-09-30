
class ZN_SvgBuilder
{

public:
    std::stringstream body;
    std::vector<ZN_SvgTag> tags;

    float width;
    float height;

    ZN_Vec2 t = ZN_Vec2(0, 0);

    ZN_SvgBuilder() = default;
    ZN_SvgBuilder(int width, int height) : width(width), height(height) {

                                           };

    void put_path(std::string path)
    {
        body << "\n\n";
        body << "<path ";
        body << path;
        body << "/>";
    }

    void put_svg(ZN_SvgBundle bundle)
    {

        tags.push_back(bundle.to_svg_tag());
        // body << "\n\n";
        // body << "<path ";
        // body << "transform=\"translate(" << bundle.get_tr().x << "," << bundle.get_tr().y << ")\" ";
        // body << bundle.path;
        // body << "/>";
    }

    void put_svg(std::string svg_data)
    {
        body << svg_data;
    }

    void translate_body(ZN_Vec2 t)
    {
        this->t = t;
    }

    void put_tag(ZN_SvgTag tag)
    {
        tags.push_back(tag);
    }
    std::string get_raw()
    {
        std::stringstream out;

        // out << "<svg width='" << width << "' height='" << height
        //     << "' xmlns='http://www.w3.org/2000/svg' version='1.1'>";

        ZN_SvgTag svg("svg", {
                                 ZN_SvgAttr{"xmlns", "http://www.w3.org/2000/svg"},
                                 ZN_SvgAttr{"version", "1.1"},
                                 ZN_SvgAttr{"width", std::to_string(width)},
                                 ZN_SvgAttr{"height", std::to_string(height)},
                             });

        ZN_SvgTag g("g");
        // ZN_SvgTag path_g("g");

        // path_g.transform(new ZN_SvgTransform{t.x, t.y});
        g.put_data(ZN_SvgTag("rect", {
                                         ZN_SvgAttr{"width", "100%"},
                                         ZN_SvgAttr{"height", "100%"},
                                         ZN_SvgAttr{"fill", "red"},
                                     }));

        for (ZN_SvgTag &tag : tags)
        {
            g.put_data(tag);
        }

        // g.put_data(path_g);

        out << svg.create_header();

        out << "\n"
            << g.as_str();
        out
            << "\n</svg>";

        return out.str();
        //  svg.str();
    }
};