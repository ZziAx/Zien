// #include
#include <sstream>

struct ZN_Vec2
{

public:
    float x;
    float y;
    ZN_Vec2(float x, float y) : x(x), y(y) {};
};

class ZN_SvgTransform
{

public:
    float translate_x = 0.;
    float translate_y = 0.;

    std::string as_str()
    {
        std::stringstream transform_stream;

        transform_stream << "transform=\"translate(" << translate_x << "," << translate_y << ")\"";

        return transform_stream.str();
    }
};

class ZN_SvgAttr
{
public:
    std::string name;
    std::string value;

    std::string as_str()
    {
        std::stringstream attr_stream;
        attr_stream << name << " = " << "\"" << value << "\"";

        return attr_stream.str();
    }
};
class ZN_SvgTag
{
public:
    ZN_SvgTag() = default;

    ZN_SvgTransform *_transform = nullptr;

    ZN_SvgTag(std::string name) : name(name) {}
    ZN_SvgTag(std::string name, std::vector<ZN_SvgAttr> attrs) : name(name), attrs(attrs) {}

    std::vector<ZN_SvgAttr> attrs;
    std::vector<ZN_SvgTag> body;
    std::string name;

    std::string close()
    {
    }

    void transform(ZN_SvgTransform *transform)
    {
        this->_transform = transform;
    }

    void push_attr(std::string name, std::string value)
    {
        attrs.push_back(ZN_SvgAttr{name, value});
    }

    void push_attr_list(std::vector<ZN_SvgAttr> attr_list)
    {
        for (ZN_SvgAttr &attr : attr_list)
        {
            attrs.push_back(attr);
        }
    }

    std::string create_attrs()
    {
        std::stringstream attr_stream;

        for (int i = 0; i < attrs.size(); i++)
        {
            ZN_SvgAttr &attr = attrs[i];

            attr_stream << " " << attr.as_str();
        }

        if (_transform)
        {
            attr_stream << " " << _transform->as_str();
        }

        return attr_stream.str();
    }

    std::string create_body()
    {
        std::stringstream body_stream;

        for (int i = 0; i < body.size(); i++)
        {
            ZN_SvgTag &b = body[i];

            body_stream << "\n"
                        << b.as_str();
        }

        return body_stream.str();
    }

    std::string create_header(bool self_close = false)
    {
        std::stringstream header;

        header << "<" << name << create_attrs() << (self_close ? "/" : "") << ">";
        return header.str();
    }

    std::string as_str()
    {

        std::stringstream data;

        data << create_header(body.empty());

        if (!body.empty())
        {
            data << create_body();
            data << "\n</" << name << ">";
        }

        return data.str();
    }

    void put_data(ZN_SvgTag data)
    {
        body.push_back(data);
    }
};
struct ZN_SvgArgs
{

    ZN_Vec2 translate = ZN_Vec2(0, 0);
    float x_advance;
    float y_offset;
    float x_offset;

    float x;
    float y;
    float left;
    float top;
    float right;
    float bottom;


    float width;
    float height;
};

struct ZN_SvgBundle
{
    ZN_SvgArgs args;

    std::string path;

    static ZN_SvgBundle from_xml_node(ZN_XmlNode &node)
    {

        ZN_SvgBundle b;
        // b.path = node.as_string();
        b.path =  node.get_str_attr("d");

        b.args.width = node.get_float_attr("_width");
        b.args.height = node.get_float_attr("_height");
        b.args.x = -node.get_float_attr("_x");
        b.args.y = -node.get_float_attr("_y");
        

        b.args.left = -node.get_float_attr("_left");
        b.args.top = -node.get_float_attr("_top");
        b.args.right = -node.get_float_attr("_right");
        b.args.bottom = -node.get_float_attr("_bottom");

        // printf("ewruouoiwer %f %f\n",b.args.left,b.args.right);


        return b;
    }

    ZN_Vec2 get_tr()
    {
        return args.translate;
    }

    void set_tr(ZN_Vec2 vec)
    {
        args.translate = vec;
    }

    void copy_attr(ZN_XmlNode &node)
    {

        // printf("\nerwuoouiwer. %s \n", node.as_string().c_str());
        args.x_advance = node.get_float_attr("x-advance");
        args.x_offset = node.get_float_attr("x-offset");
        args.y_offset = node.get_float_attr("y");

        // node.tra
    }

    ZN_SvgTag to_svg_tag(){
        ZN_SvgTag tag("path");
        tag.push_attr_list({
            ZN_SvgAttr{"d", path},
            ZN_SvgAttr{"transform", "translate(" + std::to_string(args.translate.x) + "," + std::to_string(args.translate.y) + ")"},
        });

        return tag;

    }
};