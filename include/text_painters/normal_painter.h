// #include "atlas_helper.h"

class NormalPainter
{
    bool msdfApplied = true;

    int get_x(int col, int pen_x)
    {
        return pen_x + col;
    }

    int get_y(int row, int pen_y)
    {

        int top_line = row + pen_y;

        //    printf("\nwoeiruiooiwer %i\n",y);
        return top_line;
    }

    int ch = 1;

    msdfgen::Bitmap<float, 1> apply_msdf(FT_Face face, ZN_GlyphData glyph, int width, int height, int codepoint)
    {



        float font_size = fnts;

// ANCHOR: printf debug
// $&
        float scale = font_size / face->units_per_EM;
        // float scale = 200.0;


        msdfgen::FontHandle *font_handle = msdfgen::adoptFreetypeFont(face);

        msdfgen::Shape shape;

    
        msdfgen::loadGlyph(shape, font_handle, msdfgen::GlyphIndex(codepoint), msdfgen::FONT_SCALING_NONE);
      
        msdfgen::Shape::Bounds bounds = shape.getBounds();
        float msdf_width = bounds.r - bounds.l;
        float msdf_height = bounds.t - bounds.b;


        float pxRange = 2.0f;

        int translate = 0;

        int l = round(scale*bounds.l+translate-0.5*pxRange);
        int r = round(scale*bounds.r+translate+0.5*pxRange);
        int t = round(scale*bounds.t+translate+0.5*pxRange);
        int b = round(scale*bounds.b+translate-0.5*pxRange);

        int w1 = r - l;
        int h1 = t - b;



        msdfgen::Bitmap<float, 1> msdf(w1, h1);
        // msdfgen::Vector2 translate(400, 400);

        msdfgen::generateSDF(msdf, shape, pxRange, scale, translate);

        // msdfgen::savePng(msdf,"iwoqe.png");

        // msdfgen::generateMSDF(
        //     msdf,
        //     shape,
        //     .0, // range
        //     scale,
        //     translate,
        //     msdfgen::ErrorCorrectionConfig(),
        //     true // overlapSupport
        // );

        return msdf;
    }

public:
    AtlasHelper *atlasHelper = new AtlasHelper();

    void init_atlas(FT_Face face, char *atlas, char *path)
    {
        // this.face = face;

        *atlasHelper = AtlasHelper::from_path(atlas, path);

        atlasHelper->add_package(face,atlas, path);
    }

    FT_ULong findCodepointForGlyph(FT_Face face, FT_UInt glyphIndex)
    {
        FT_UInt gindex;
        FT_ULong charcode = FT_Get_First_Char(face, &gindex);

        while (gindex != 0)
        {
            if (gindex == glyphIndex)
                return charcode;
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
        }
        return 0; // Not found
    }

    int xx = 0;
    void paint_glyph(unsigned char *pixels, FT_Face face, int width, int height, ZN_GlyphData glyph, ZN_TextMetrics metrics, int left, int top, int max_height)
    {

        // if (true)
        // {
        //     char *atlas = "atlas.png";
        //     char *path = "ewriiuower2.json";
        //     init_atlas(atlas, path);
        //     atlasHelper->draw_glyph(glyph.codepoint, 50, 0, width, pixels);

        //     // atlasHelper->get_glyph_buffer(glyph.codepoint);

        //     // render_atlas();

        //     return;
        // }

        FT_Bitmap &bitmap = face->glyph->bitmap;

        if (false)
        {
            msdfgen::Bitmap<float, 1> msdf = apply_msdf(face, glyph, width, height, glyph.codepoint);
            top = (max_height - glyph.get_height()) - top;
            // printf("\nuerwuiouoiwer %i\n",max_height);
            render_msdf(pixels, msdf,10, 0, width, height);
            // xx+=80;
        }

        else
        {
            // printf("eowriuoiwer %i",bitmap.width);
            render_bitmap(pixels, bitmap, left, top, width, height);
        }
    }

    void render_msdf(unsigned char *pixels, msdfgen::Bitmap<float, 1> msdf, int left, int top, int width, int height)
    {
        for (unsigned int row = 0; row < msdf.height(); ++row)
        {
            for (unsigned int col = 0; col < msdf.width() - left; ++col)
            {

                int x, y;

                x = get_x(col, left);
                y = get_y(row, top);

                bool is_black = msdfgen::pixelFloatToByte(msdf(col, row)[0]) == 0.0 && msdfgen::pixelFloatToByte(msdf(col, row)[1]) == 0.0 && msdfgen::pixelFloatToByte(msdf(col, row)[2]) == 0.0;
                msdfgen::byte r = msdfgen::pixelFloatToByte(msdf(col, row)[0]);
                msdfgen::byte g = msdfgen::pixelFloatToByte(msdf(col, row)[1]);
                msdfgen::byte b = msdfgen::pixelFloatToByte(msdf(col, row)[2]);
                msdfgen::byte a = msdfgen::pixelFloatToByte(msdf(col, row)[3]);

                pixels[(x + width * y) * 4 + 0] = fmax(r, pixels[(x + width * y) * 4 + 0]);
                pixels[(x + width * y) * 4 + 1] = fmax(g, pixels[(x + width * y) * 4 + 1]);
                pixels[(x + width * y) * 4 + 2] = fmax(b, pixels[(x + width * y) * 4 + 2]);
                // pixels[(x + width * y) * 4 + 3] = fmax(a, pixels[(x + width * y) * 4 + 3]);

                // pixels[(x + tex_width * y) * 4 + 3] = fmax(a,pixels[(x + tex_width * y) * 4 + 3])
                //    ;

                // else{
                pixels[(x + width * y) * 4 + 3] = 255;

                // }
            }
        }
    }

    void render_atlas(unsigned char *pixels, int left, int top, int width, int height)
    {
    }
    void render_bitmap(unsigned char *pixels, FT_Bitmap &bitmap, int left, int top, int width, int height)
    {


        for (unsigned int y = 0; y < bitmap.rows; ++y)
        {
            for (unsigned int bx = 0; bx < bitmap.width; ++bx)
            {
                int px = left + bx;
                int py = height - (top + y); // Invert Y coordinate for OpenGL
                // int py = top + y;

                if (px >= 0 && px < width && py >= 0 && py < height)
                {

                    unsigned char alpha = bitmap.buffer[y * bitmap.pitch + bx];
                    if (alpha == 0)
                        continue;

                    int idx = (py * width + px) * 4;
                    pixels[idx + 0] = fmax(pixels[idx + 0], alpha); // R
                    pixels[idx + 1] = fmax(pixels[idx + 1], alpha); // G
                    pixels[idx + 2] = fmax(pixels[idx + 2], alpha); // B
                    pixels[idx + 3] = fmax(pixels[idx + 3], alpha);
                }
            }
        }
    }
};