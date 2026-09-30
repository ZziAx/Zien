ZN_TextLayout *ZN_HandlerObj::create_layout()
{
    layout = new ZN_TextLayout(*shaper);

    return layout;
};