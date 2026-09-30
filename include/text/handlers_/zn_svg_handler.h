
class ZN_SvgHandler : public ZN_HandlerObj
{

    // friend class ZN_TextShaper;
public:

    ZN_SvgHandler() : ZN_HandlerObj(ZN_RenderFlag::SVG) {}
    

    std::string get_svg();
    void to_svg();
};