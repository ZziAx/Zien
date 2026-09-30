void ZN_SvgHandler::to_svg()
{
    canvas = new ZN_TextCanvas(this);
    canvas->to_svg();
}

std::string ZN_SvgHandler::get_svg()
{

    return std::string("hello");
}
