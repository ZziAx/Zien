#include "json.hpp"
#include <fstream>
#include <map>
#include <memory> // For std::shared_ptr
using namespace nlohmann;

struct GlyphInf
{
public:
    float w;
    float h;
    int unicode;
    float advance;
    float planeLeft, planeTop, planeRight, planeBottom;
    float atlasLeft, atlasTop, atlasRight, atlasBottom;

    static GlyphInf from_json(nlohmann::json &g)
    {

        GlyphInf glyph;
        glyph.unicode = g["unicode"];

        glyph.advance = g["advance"];

        if (g.contains("planeBounds"))
        {
            glyph.planeLeft = g["planeBounds"]["left"];

            glyph.planeTop = g["planeBounds"]["top"];

            glyph.planeRight = g["planeBounds"]["right"];

            glyph.planeBottom = g["planeBounds"]["bottom"];

            glyph.atlasLeft = g["atlasBounds"]["left"];
            glyph.atlasTop = g["atlasBounds"]["top"];
            glyph.atlasRight = g["atlasBounds"]["right"];
            glyph.atlasBottom = g["atlasBounds"]["bottom"];

            glyph.w = abs(glyph.atlasRight - glyph.atlasLeft);
            glyph.h = abs(glyph.atlasTop - glyph.atlasBottom);
        }

        return glyph;

        // glyphs.push_back(glyph);
    }

    void get_bound_size(int &width, int &height)
    {
        width = int(w) + 1;
        height = int(h) + 1;
        // width = int(w + 0.5);
        // height = int(h + 0.5);
    }
};

class AtlasMetrics
{
public:
    int width;
    int height;
    int channel;
    unsigned char *data;
    AtlasMetrics() {};
    AtlasMetrics(int w, int h, int c, unsigned char *d) : width(w), height(h), channel(c), data(d) {};

    void get(int &width, int &height, int &channels,

             unsigned char *&atlas)
    {
        width = this->width;
        height = this->height;
        channels = this->channel;
        atlas = this->data;
    }
};

class AtlasPackage
{
    char *atlasPath;
    nlohmann::json atlasJson;
    std::map<int, GlyphInf> glyphs;
    unsigned char *atlas;
    AtlasMetrics atlasMetrics;

public:
    AtlasPackage() {};
    AtlasPackage(nlohmann::json j, AtlasMetrics m) : atlasJson(j), atlasMetrics(m)
    {
        glyphs.clear();

        nlohmann::json glyphs = j["glyphs"];

        load_glyphs(glyphs);
    };

    void load_glyphs(nlohmann::json j)
    {

        for (auto &el : j.items())
        {
            int unicode = el.value()["unicode"].get<int>();

            GlyphInf glyph = GlyphInf::from_json(el.value());

            glyphs[unicode] = glyph;
        }
    }

    GlyphInf get_glyph_info(int codepoint)
    {

        // auto glyphParam = get_info_by_codepoint(codepoint);
        return glyphs[codepoint];
    }

    GlyphInf get_glyph_info(ZN_GlyphData glyph_data)
    {

        // auto glyphParam = get_info_by_codepoint(codepoint);
        return glyphs[glyph_data.get_charcode()];
    }

    int le = 0;

    void draw_glyph(int codepoint, int left, int top, int surface_width, int surface_height, int &glyph.get_width(), int &glyph.get_height(), unsigned char *pixels, bool rtl = false)
    {

        // unsigned int top = static_cast<unsigned int>(t);

        // int top = t;
        int width, height, channels;

        unsigned char *atlas;

        atlasMetrics.get(width, height, channels, atlas);

        if (!atlas)
        {
            return;
        }

        GlyphInf inf = get_glyph_info(codepoint);

        int glyphW, glyphH;

        inf.get_bound_size(glyphW, glyphH);

        // glyphH+=-;
        // glyphW+=10;
        glyph.get_width() = glyphW;

        glyph.get_height() = glyphH;

        if (rtl)
        {
            // left-=1;

            // set right as start point
            // left = surface_width - glyphW;
        }
        else
        {
        }

        if (!pixels)
        {
            return;
        }

        // printf("\nweouriuioewr %i\n",glyph.get_height());

        float planeTop = inf.planeTop;

        // printf("wreouiouoiouewr %f %f",(inf.planeTop - inf.planeBottom)*200, inf.atlasTop - inf.atlasBottom);

        // final y = baseline position + global shift
        // top = planeTop * fnts + 120;
        // top = planeTop * fnts;

        // top = round(top);
        // top+=80;
        // int t = top;
        // top = 0;

        //    int(inf.w) + 1
        // int glyphH = int(inf.h) + 1;

        // float f = round((inf.planeTop * fnts)+0.1);
        float f = round((inf.planeTop ) );


        top = f ;
        
        // printf("rewouoiwre %f %f %i\n",inf.planeTop * fnts,f,top);

        // top = fmin(top,t);

        // t = ceil(t-0.5);
        // top = fmax(top,t);

        // printf("\n weoiuioewr %i %f\n",round(t+0.5),t);

// ANCHOR: printf debug
// $&
        top += 30;
        int startX = abs(int(inf.atlasLeft));
        // int startY = int(height + inf.atlasTop);
        // int startY = abs(int(height-inf.atlasTop));

        int startY = int(height - abs(inf.atlasTop));

        std::vector<unsigned char> glyph(glyphW * glyphH * 4);

        // top=100;
        for (int y = 0; y < glyphH; ++y)
        {
            for (int x = 0; x < glyphW; ++x)
            {

                int srcIndex = ((startY + y) * width + (startX + x)) * 4;

                int dstIndex = ((top + y) * (surface_width) + (x + left)) * 4;

                // if(dstIndex> 0){
                //     // printf("eroi");
                //     return;
                // }

                if (dstIndex < 0 || dstIndex + 3 >= surface_width * surface_height * 4)
                {
                    continue;
                    ;
                }

                int r = pixels[dstIndex + 0];
                int g = pixels[dstIndex + 1];
                int b = pixels[dstIndex + 2];
                int a = pixels[dstIndex + 3];

                int R = atlas[srcIndex + 0];
                int G = atlas[srcIndex + 1];
                int B = atlas[srcIndex + 2];
                int A = atlas[srcIndex + 3];

                pixels[dstIndex + 0] = fmax(r, R);
                pixels[dstIndex + 1] = fmax(g, G);
                pixels[dstIndex + 2] = fmax(b, B);
                pixels[dstIndex + 3] = fmax(a, A);
            }
        }

        le += glyphW;
    }

    static AtlasPackage load_atlas(char *a, char *p)
    {

        std::string path = std::string(p);

        std::ifstream file(path);

        if (!file.is_open())
        {
        }

        nlohmann::json j;

        file >> j;
        file.close();

        int width, height, channels;
        unsigned char *atlas = stbi_load(a, &width, &height, &channels, 4);
        if (!atlas)
        {
            // return;
        }

        return AtlasPackage(j, AtlasMetrics(width, height, channels, atlas));
    }
};

class AtlasHelper
{

    std::vector<AtlasPackage> atlas_packages;

    std::vector<ZN_GlyphData> glyphs;

    char *atlasPath;

    int surface_width;
    int surface_height;
    // std::vector<unsigned char> pixels;

    nlohmann::json atlasJson;

public:
    AtlasHelper() {
    };
    AtlasHelper(nlohmann::json j) : atlasJson(j) {};

    static AtlasHelper from_path(char *a, char *p)
    {

        // File path to your JSON
        std::string path = std::string(p);

        // Open the file
        std::ifstream file(path);

        if (!file.is_open())
        {
        }

        // Parse JSON
        nlohmann::json j;

        file >> j;
        file.close();

        return AtlasHelper(j);
    }

    void extract_glyph(const char *atlasPath, GlyphInf inf, const char *outPath)
    {
        int width, height, channels;
        unsigned char *atlas = stbi_load(atlasPath, &width, &height, &channels, 4);
        if (!atlas)
        {
            return;
        }

        int glyphW = int(inf.w) + 1;

        int glyphH = int(inf.h) + 1;

        int startX = abs(int(inf.atlasLeft));
        int startY = int(4096 - abs(inf.atlasTop));

        std::vector<unsigned char> glyph(glyphW * glyphH * 4);

        for (int y = 0; y < glyphH; ++y)
        {
            for (int x = 0; x < glyphW; ++x)
            {

                int srcIndex = ((startY + y) * width + (startX + x)) * 4;

                int dstIndex = (y * glyphW + x) * 4;

                glyph[dstIndex + 0] = atlas[srcIndex + 0];
                glyph[dstIndex + 1] = atlas[srcIndex + 1];
                glyph[dstIndex + 2] = atlas[srcIndex + 2];
                glyph[dstIndex + 3] = atlas[srcIndex + 3];
            }
        }

        // printf("erwoiuiower %i %i",glyphW, width);
        stbi_write_png(outPath, glyphW, glyphH, 4, glyph.data(), glyphW * 4);
        stbi_image_free(atlas);
    }

    void extract_glyph(const char *atlasPath, GlyphInf inf, int left, int top, int surface_width, unsigned char *pixels)
    {
        int width, height, channels;

        unsigned char *atlas = stbi_load(atlasPath, &width, &height, &channels, 4);

        if (!atlas)
        {
            return;
        }

        int glyphW = int(inf.w) + 1;

        int glyphH = int(inf.h) + 1;

        int startX = abs(int(inf.atlasLeft));
        int startY = int(4096 - abs(inf.atlasTop));

        // std::vector<unsigned char> glyph(glyphW * glyphH * 4);

        for (int y = 0; y < glyphH; ++y)
        {
            for (int x = 0; x < glyphW; ++x)
            {

                int srcIndex = ((startY + y) * width + (startX + x)) * 4;

                int dstIndex = ((top + y) * surface_width + (x + left)) * 4;

                pixels[dstIndex + 0] = atlas[srcIndex + 0];
                pixels[dstIndex + 1] = atlas[srcIndex + 1];
                pixels[dstIndex + 2] = atlas[srcIndex + 2];
                pixels[dstIndex + 3] = atlas[srcIndex + 3];
            }
        }
    }

    void load_atlas()
    {
    }

    void load_glyph_set(std::vector<ZN_GlyphData> &glyphs)
    {

        this->glyphs = glyphs;
    }

    void init_surface(std::vector<unsigned char> &pixels, int w, int h, int channels)
    {
        this->surface_width = w;
        this->surface_height = h;

        pixels.resize(w * h * channels);
    }
    void measure_max_size(int &w, int &h)
    {
        //  this->glyphs[0].face->family_name;

        int width = 0.0;
        int height = 0.0;

        for (auto &glyph : glyphs)
        {
            GlyphInf inf = atlas_packages[0].get_glyph_info(glyph);
            // int w, h;
            inf.get_bound_size(w, h);
            width += w;
            height = fmax(height, h);
        }

        w = width;
        h = height;
    }

    nlohmann::json get_info_by_index(int index)
    {

        auto glyphs = atlasJson["glyphs"];

        for (auto &el : glyphs.items())
        {
            // std::string key = el.key(); // could be the unicode "65"
            int glyphIndex = el.value()["unicode"].get<int>();

            if (glyphIndex == index)
            {

                return el.value();
            }

            // std::cout << "Unicode: " << key << ", advance: " << val["advance"] << "\n";
        }
        throw std::runtime_error("Glyph index not found");

        // return false;
    }

    nlohmann::json get_info_by_codepoint(int codepoint)
    {

        auto glyphs = atlasJson["glyphs"];

        for (auto &el : glyphs.items())
        {
            // std::string key = el.key(); // could be the unicode "65"
            int glyphCodepoint = el.value()["unicode"].get<int>();

            if (glyphCodepoint == codepoint)
            {

                return el.value();
            }

            // std::cout << "Unicode: " << key << ", advance: " << val["advance"] << "\n";
        }
        throw std::runtime_error("Glyph codepoint not found");

        // return false;
    }

    GlyphInf get_glyph_info(int codepoint)
    {

        auto glyphParam = get_info_by_codepoint(codepoint);

        return GlyphInf::from_json(glyphParam);
    }

    int pen_x = 0;

    void render_atlas(std::vector<unsigned char> &pixels)
    {
        AtlasPackage &p = atlas_packages[0];
        // AtlasPackage p = atlas_packages[0];
        int i = 0;

        for (auto &g : glyphs)
        {

            float m = 1.0;

            int w, h;

            int glyphY = -g.get_y(); // make a local copy
            // printf("erwiouuioerw %i\n",g.get_height());
            // printf("erowuiuoiwer %f", g.max_y);
            // printf("erwuoiuower %f\n\n", (inf.planeTop) );

            // printf("ewruioweoiur %i", g.get_charcode());

            // if(glyphY == -112){
            //     glyphY = -111;
            // }

            bool df = false;
            if (i == 0)
            {

                df = true;
            }
            p.draw_glyph(g.get_charcode(), pen_x, g.top, surface_width, surface_height, w, h, pixels.data(), df);

            pen_x += w;
            i += 1;
        }
    }
    void draw_glyph(int codepoint, int x, int y, int surface_width, unsigned char *pixels

    )
    {

        // atlas_packages[0].draw_glyph(codepoint, x, y, surface_width, pixels);

        // GlyphInf glyph_inf = get_glyph_info(codepoint);

        // extract_glyph("atlas.png", glyph_inf, x, y, canvasWidth, pixels);
    }

    std::shared_ptr<FT_Face> face;
    void add_package(FT_Face &face, char *atlasPath, char *atlasJson)
    {

        // this->face = face;

        AtlasPackage p = AtlasPackage::load_atlas(atlasPath, atlasJson);

        atlas_packages.push_back(p);

        GlyphInf inf = atlas_packages[0].get_glyph_info('s');
    }

    void get_glyph_buffer(int index)
    {

        GlyphInf glyph_inf = get_glyph_info(index);

        // printf("rweuooiiewr %i",glyph_inf.x);

        extract_glyph("atlas.png", glyph_inf, "atlas2.png");
    }
};