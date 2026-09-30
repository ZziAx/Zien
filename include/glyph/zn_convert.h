int ZN_Get_Codepoint(int gindex, FT_Face face)
{
    FT_ULong charcode;
    FT_UInt gidx;
    charcode = FT_Get_First_Char(face, &gidx);
    while (gidx != 0)
    {
        if (gidx == gindex)
        {
            break;
        }
        charcode = FT_Get_Next_Char(face, charcode, &gidx);
    }
    return (int)charcode;
}