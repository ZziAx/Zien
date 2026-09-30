
void ZN_BitmapHandler::to_bitmap()
{
    canvas = new ZN_TextCanvas(this);
    canvas->to_bitmap();
};

std::vector<unsigned char> &ZN_BitmapHandler::get_pixels()
{
    return canvas->get_pixels();
};

