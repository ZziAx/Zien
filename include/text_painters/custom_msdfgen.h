#include "external/msdfgen/msdfgen.h";
#include "external/msdfgen/ext/import-font.h";
#include <msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include "json.hpp"

#include "atlas_helper.h"

using namespace msdf_atlas;
using namespace nlohmann;

class Msdfgen
{

public:
    Msdfgen() {}

    void collect_all_charset(Charset &charset, FT_Face face)
    {
// std::vector<int> chrs = {1587};
        std::vector<int> chrs = {
            64509,
            0,
            65194,
            65239,
            32,
            65197,
            65166,
            64510,
            32,
            65262,
            65175,
            32,
            65258,
            65169,
            32,
            65249,
            65276,
            65203};


            for(auto&c : chrs){
                charset.add(c);
            }

        // FT_UInt glyph_index;

        // // charset.add(65169);
        // // charset.add(65258);
        // FT_ULong charcode = FT_Get_First_Char(face, &glyph_index);

        // while (glyph_index != 0)
        // {
        //     charset.add(charcode);
        //     charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
        // }
    }

    bool generate_atlas(const char *font_path, FT_Face face)
    {

        const int channel = 1;

        bool success = false;

        float font_size = fnts;

        // printf("eorwuiowe %f",face->units_per_EM);
        float scale =font_size;
        // Initialize instance of FreeType library
        if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype())
        {
            // Load font file
            if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, font_path))
            {

                std::vector<GlyphGeometry> glyphs;

                FontGeometry fontGeometry(&glyphs);

                Charset charset = Charset();

                collect_all_charset(charset, face);

                // return false;

                fontGeometry.loadCharset(font, scale, charset);

                const double maxCornerAngle = 0.0;

                for (GlyphGeometry &glyph : glyphs)
                {
                    // glyph.load(font,1.0,32);

                    // glyph.load(font, glyph.getCodepoint(), msdfgen::FONT_SCALING_LEGACY); // 👈 raw FreeType units
                    // glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 20.0);
                }

                // TightAtlasPacker class computes the layout of the atlas.
                TightAtlasPacker packer;
                // Set atlas parameters:
                // setDimensions or setDimensionsConstraint to find the best value
                packer.setDimensionsConstraint(DimensionsConstraint::NONE);
                // setScale for a fixed size or setMinimumScale to use the largest that fits

                // packer.setScale(100);
                // packer.setPaddi
                // packer.setMinimumScale(1.0);
                // setPixelRange or setUnitRange
        //  packer.setMinimumScale(scale);
                packer.setPixelRange(0.0);
                packer.setMiterLimit(0.0);
                // Compute atlas layout - pack glyphs
                packer.pack(glyphs.data(), glyphs.size());
                // Get final atlas dimensions
                int width = 0, height = 0;
                packer.getDimensions(width, height);
                // The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
                ImmediateAtlasGenerator<
                    float,                            // pixel type of buffer for individual glyphs depends on generator function
                    channel,                          // number of atlas color channels
                    sdfGenerator,                   // function to generate bitmaps for individual glyphs
                    BitmapAtlasStorage<byte, channel> // class that stores the atlas bitmap
                    // For example, a custom atlas storage class that stores it in VRAM can be used.
                    >
                    generator(width, height);
                // GeneratorAttributes can be modified to change the generator's default settings.
                GeneratorAttributes attributes;
                generator.setAttributes(attributes);
                generator.setThreadCount(4);
                // Generate atlas bitmap
                generator.generate(glyphs.data(), glyphs.size());

                BitmapAtlasStorage<byte, channel> atlasStorage = generator.atlasStorage();
                msdfgen::BitmapConstRef<byte, channel> atlas = atlasStorage;

                msdf_atlas::JsonAtlasMetrics metrics = JsonAtlasMetrics();
                metrics.yDirection = msdf_atlas::YDirection::TOP_DOWN;

                exportJSON(&fontGeometry, 1, msdf_atlas::ImageType::SDF, metrics, "atlas.json", true);

                msdf_atlas::saveImage(atlas, ImageFormat::PNG, "atlas.png", metrics.yDirection);

                msdfgen::destroyFont(font);
            }
            msdfgen::deinitializeFreetype(ft);
        }
        return success;
    }

    void paint_glyph(FT_Face face)
    {
        generate_atlas("/Users/bitels/Desktop/Ffonts/Persian/kalameh(Eco)/kalameh(Eco)/01- Standard Fonts/TTF/Kalameh-Black.ttf", face);
    }
};