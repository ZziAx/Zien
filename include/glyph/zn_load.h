void ZN_Load_Glyph(FT_Face face, int codepoint, ZN_TextStyle *style = nullptr, bool render = true)
{
    style->pre_load(face);
    FT_Load_Glyph(face, codepoint, FT_LOAD_NO_BITMAP);
    style->load(face);
    if (!render)
        return;
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
}