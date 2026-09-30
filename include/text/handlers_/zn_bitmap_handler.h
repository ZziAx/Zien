
class ZN_BitmapHandler : public ZN_HandlerObj
{

    // friend class ZN_TextShaper;
public:
    ZN_BitmapHandler() : ZN_HandlerObj(ZN_RenderFlag::BITMAP) {}

    std::vector<unsigned char> &get_pixels();
    void to_bitmap();
};