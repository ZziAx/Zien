class ZN_TextRenderer
{
private:
    ZN_TextLayout *text_layout;
    ZN_TextCanvas *canvas;
    ZN_BitmapHandler bmp_handler;
    ZN_SvgHandler svg_handler;

public:
    ZN_FontCollection font_collection;
    ZN_TextRenderer() = default;

    bool render_text(const char *text, std::string path)
    {
        // ZN_EngineContext::init();
        // // ZN_Font normal("Regular", "/Users/bitels/Downloads/Vazirmatn-Regular.ttf");

        // ZN_Font bold("Bold", "/Users/bitels/Downloads/Vazirmatn-Regular.ttf");
        // ZN_Font normal("light", "/Users/bitels/Desktop/Ffonts/Morabba_Pro/ttf/Morabba-Regular.ttf");
        // ZN_Font black("black", "/Users/bitels/Desktop/Ffonts/Morabba_Pro/ttf/Morabba-Black.ttf");

        // std::vector<ZN_Font> fonts;
        // fonts.push_back(normal);
        // fonts.push_back(bold);
        // fonts.push_back(black);

        // ZN_FontBundle *bundle = new ZN_FontBundle(fonts);

        // // font_collection.add_font(black);

        // ZN_Font f("normal", path);
        // // font_collection.add_font(f);

        // font_collection.add_bundle(bundle);

        // font_collection.add_emoji_set("/Users/bitels/Desktop/project/python/ttx/emoji.svgb");

        // bundle->use_font("black");

        // ZN_TextStyle *style = new ZN_TextStyle();

        // style->italic.set_angle(0.0);
        // // style->bold.set_strength(20.0);

        // // style->stroker.add_stroke(3.0);
        // // style->scale.set_scale(2.0);
        // ZN_BitmapHandler *bmp_handler;

        // ZN_TextShaper shaper = ZN_TextShaper(font_collection, style, bmp_handler);

        // ZN_ERROR err = shaper.shape_text(text);

        // text_layout = new ZN_TextLayout(shaper);

        // text_layout->set_split_mode(SPLIT_BY_WORD);
        // text_layout->set_line_space(0);

        // text_layout->set_width(MAXWIDTH);

        // text_layout->generate_lines();
        // save_png();

        return true;
    }

    bool render_text(std::string text, std::string path,    std::optional<double> width = std::nullopt)
    {

        ZN_EngineContext::init();

        ZN_Font normal("normal", path);

        ZN_TextStyle *style = new ZN_TextStyle();

        font_collection.add_font(normal);

        ZN_TextShaper shaper = ZN_TextShaper(font_collection, style, &bmp_handler);

        ZN_ERROR err = shaper.shape_text(text.c_str());
        
        text_layout = bmp_handler.create_layout();

        if(width.has_value()){
            text_layout->set_width(*width);
        }
        text_layout->generate_lines();

        bmp_handler.to_bitmap();

        return true;
    }

    void render_svg(std::string text, std::string path)
    {
        ZN_Font normal("normal", path);

        font_collection.add_font(normal);

        ZN_TextStyle *style = new ZN_TextStyle();

        style->italic.set_angle(0.0);
        style->bold.set_strength(0.0);

        ZN_TextShaper shaper = ZN_TextShaper(font_collection, style, &svg_handler);

        ZN_ERROR err = shaper.shape_text(text.c_str());

        text_layout = svg_handler.create_layout();

        text_layout->generate_lines();

        svg_handler.to_svg();
    }

    void set_width(int w)
    {
        // text_layout.set_width(w);
        // text_layout.generate_lines();
        // text_painter.update_layout(text_layout);
        // text_painter.to_bitmap();
    }

    std::vector<unsigned char> &get_pixels()
    {

        return bmp_handler.get_pixels();
    }

    void get_size(int &w, int &h)
    {

        text_layout->get_size(w, h);
    }

    void save_png()
    {
        int width, height;

        get_size(width, height);

        std::vector<unsigned char> &pixels = get_pixels();

        flipY(false);
       
        stbi_write_png("zien_test.png", width, height, 4, pixels.data(), width * 4);
    }


     void save_png(std::string path)
    {
        int width, height;

        get_size(width, height);

        std::vector<unsigned char> &pixels = get_pixels();

        flipY(false);
       
        stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);
    }

    void flipY(bool flip = true)
    {
        stbi_flip_vertically_on_write(flip); // Flip the image vertically for correct rendering
    }
};