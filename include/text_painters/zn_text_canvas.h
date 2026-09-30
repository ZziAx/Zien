
#include "bitmap_helper.h"

using LineRenderIteratorCallback = std::function<void(ZN_LineData *, float, int, float)>;
using GlyphRenderIteratorCallback = std::function<void(float, float, ZN_GlyphData)>;

class ZN_TextCanvas
{

private:
    float width;
    float height;

    std::vector<ZN_LineData> lines;
    std::vector<unsigned char> pixels;

    void to_bitmap()
    {
        pre_proccess();
        paint_lines();

        // paint_lines([this](ZN_LineData *line, float y, int x_shift, float pen_y)
        //             { render_line(line, y, x_shift, pen_y); });
    }
    void to_bitmap(ZN_TextLayout *layout)
    {
        update_layout(layout);
        to_bitmap();
    }

    std::string to_svg(ZN_TextLayout *layout)
    {
        update_layout(layout);
        return to_svg();
    }

    std::string to_svg()
    {

        pre_proccess();

        init_metrics();

        // ZN_SvgBuilder svg_builder = ZN_SvgBuilder(width, height);
        ZN_SvgBuilder svg_builder;

        float w = 0.;
        float h = 0.;

        paint_lines([this, &svg_builder, &w, &h](ZN_LineData *line, float y, int x_shift, float pen_y)
                    {
                       
                        float p = 0.;
                        float xf = 0.;
                        float top = 0.0;

                        ZN_SvgTag group("g");

                        // printf("ewruoiiuwre %f",line->glyphs.front().svg.args.x);

                        float x_offset = line->glyphs.front().svg.args.x;


                        render_line(line, y, x_shift, pen_y,
                                    [this, &svg_builder, &w, &h, &p, &xf, &group,&top](float aligned_pos, float t, ZN_GlyphData glyph)
                                    {
                                        ZN_SvgBundle svg = glyph.svg;
                                        //   svg.set_tr(ZN_Vec2(float(aligned_pos) + svg.args.x, float(t) + svg.args.y));
                                        // printf("\nrewouiower %f dsf\n", svg.args.top);

                                        // svg.set_tr(ZN_Vec2( svg.args.x + p,  0));
                                        svg.set_tr(ZN_Vec2(p + svg.args.x_offset,0));

                                        xf += svg.args.x;

                                        p += svg.args.x_advance;

                                        top = fmax(top,svg.args.top);

                                        group.put_data(svg.to_svg_tag());

                                        //   w += svg.args.width;
                                        h += svg.args.height;
                                        // printf("erwouioewr %f\n", svg.args.y);

                                        // formula. translate whole group left svg amount svg.args.x  to lift
                                        // width  right svg svg.args.x - left svg svg.args.x
                                    });
                        group.transform(new ZN_SvgTransform{x_offset, top});

                        svg_builder.put_tag(group);

                        // printf("\nrewuoi %f\n",xf);
                    });

        // svg_builder.width = w;
        // svg_builder.height = h;

        svg_builder.width = width;
        svg_builder.height = height;
        // svg_builder.height = h;

        // svg_builder.close();
        // svg_builder.translate_body(ZN_Vec2(2.0, 6.0));

        printf("\n%s\n", svg_builder.get_raw().c_str());
        return svg_builder.get_raw();
    }

    friend class ZN_BitmapHandler;
    friend class ZN_SvgHandler;

public:
    ZN_HandlerObj *handler;
    ZN_TextLayout *layout;

    ZN_TextCanvas() = default;

    ZN_TextCanvas(ZN_TextLayout *layout) : layout(layout)
    {
        pixels.reserve(3000 * 3000 * 4);
    }

    ZN_TextCanvas(ZN_BitmapHandler *handler) : layout(handler->get_layout())
    {
        pixels.reserve(3000 * 3000 * 4);
        handler->set_painter(this);
    }

    ZN_TextCanvas(ZN_SvgHandler *handler) : layout(handler->get_layout())
    {
        handler->set_painter(this);
    }

    std::vector<unsigned char> &get_pixels()
    {
        return pixels;
    }

    void update_layout(ZN_TextLayout *layout)
    {
        this->layout = layout;
    }

    void init_surface(float w, float h)
    {

        // pixels.clear();

        pixels.assign(w * h * 4, 0);

        // std::cout << "Elapsed time: " << duration << " s\n";
    }

    void init_metrics()
    {

        width = layout->metrics.get_width();
        height = layout->metrics.get_height();
    }

    void pre_proccess()
    {
        this->lines = lines;
        init_metrics();
    }

    void paint_lines(std::vector<ZN_LineData> lines, LineRenderIteratorCallback iter_line = nullptr)
    {
                                        

        // line, pen_y + line->max_y, x_shift, pen_y

        if (layout->metrics.render_flag == ZN_RenderFlag::BITMAP)
        {
            init_surface(width, height);
        }

        float pen_y = 0;

        for (int i = 0; i < lines.size(); i++)
        {

            ZN_LineData *line = &lines[i];

            if (i > 0)
            {
                pen_y += layout->line_space;
            }

            float x_shift = layout->metrics.get_line_x_align(line);

            if (iter_line == nullptr)
            {
                render_line(line, pen_y + line->max_y, x_shift, pen_y);
            }
            else
            {
                iter_line(line, pen_y + line->max_y, x_shift, pen_y);
            }

            pen_y += line->height;
        }
    }

    void paint_lines(LineRenderIteratorCallback iter_line = nullptr)
    {

        paint_lines(layout->lines, iter_line);
    }
    float align_right(int w, int s, int x)
    {

        return w - s - x;
    }

    void render_line(ZN_LineData *line, float y = 0.0f, float x_shift = 0.0f, float pen_y = 0.0f, GlyphRenderIteratorCallback glyph_iter = nullptr)
    {


        // std::vector<ZN_GlyphData> gl = line->glyphs;

        std::vector<ZN_GlyphData> gl = line->glyphs;

        // ifp2
        // None
        // std::reverse(gl.begin(), gl.end());

        float x = 0;

        float sh = 0;

        float pen_x = 0;

        // printf("wuroiuewr %i",to);
        for (int i = 0; i < gl.size(); i++)
        {
            ZN_GlyphData glyph = gl[i];

            if (i != 0)
            {
                x = glyph.get_x();
            }

            float x_advance = glyph.get_x_advance();
            if (i == 0)
            {
                glyph.load();

                float l, t, r, b;

                r = 0;
                l = 0;
                t = 0;
                b = 0;
                // GlyphHelper::get_paddings(glyph.face, l, t, r, b);
                x_advance = glyph.get_width();
                sh -= r;
            }

            float t = 0;
            if (glyph.font_type == FontType::SVGB)
            {
                t = pen_y + layout->padding() / 2;
            }

            else
            {
                t = y + layout->padding() / 2;
            }

            float aligned_pos = align_right(layout->metrics.get_width(), x_advance, pen_x - x + sh);
            // float aligned_pos = (x_advance+x+sh);
            // float aligned_pos = 0.0;

            if (glyph_iter == nullptr)
            {
                paint(aligned_pos, t, glyph, 0);
            }

            else
            {

                glyph_iter(aligned_pos, t, glyph);
            }

            pen_x += glyph.get_x_advance();

            if (i == 0)
            {
                sh -= glyph.get_x_advance() - x_advance;

                sh += glyph.get_x();
            }
        }
    }

    void paint(float x, float y, ZN_GlyphData &glyph, int line = 0)
    {

        switch (glyph.font_type)
        {

        case FontType::NORMAL:
        {

            FT_Face face = glyph.face;
            glyph.load();

            FT_Bitmap &bitmap = face->glyph->bitmap;
            int left = x;
            int top = y - glyph.metrics.top;

            BitmapHelper::paint(pixels.data(), glyph.buffer, glyph.pitch, left, top, glyph.metrics.width, glyph.metrics.height, width, layout->metrics.height, 0);
            break;
        }
        case FontType::COLR0:
        {

            FT_Face face = glyph.face;
            glyph.load();
            FT_Bitmap &bitmap = face->glyph->bitmap;
            int left = x;
            int top = y - face->glyph->bitmap_top;

            BitmapHelper::paint(pixels.data(), glyph.buffer, glyph.pitch, left, top, glyph.metrics.width, glyph.metrics.height, width, layout->metrics.height, 0);

            break;
        }

        case FontType::SVGB:
        {

#if ZN_TINYXML_IMPLEMENTATION

            ZN_SvgbParser emoji_parser = EmojiManager::instance().get_parser_ref();
            ZN_SvgbEmojiSlot *slot = emoji_parser.get_slot(glyph.character.c_str(), *(new bool));

            slot->load_buffer();

            float width;

            layout->get_width(width);
            float height = layout->metrics.height;

            BitmapHelper::paint_raw_buffer(pixels.data(), x, y, slot->pixmap_data, slot->pixmap_len, slot->viewbox.width, slot->viewbox.height, glyph.get_width(), glyph.get_height(), (int)width, (int)height);
#endif
            break;
        }
        }
    }
};
