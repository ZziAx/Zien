#pragma once


int fnts = 200;


// default
#include <fstream>
#include <functional>


#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"


#include <ft2build.h>
#include FT_FREETYPE_H


#include <hb.h>
#include <hb-ft.h>

#include "../external/utfcpp/utf8.h"


#include "models/classes.h"

#include "models/zn_glyph_models.h"
#include "zn_engine_context.h"

#include "style/zn_style_obj.h"
#include "style/zn_stroke.h"
#include "style/zn_stroker.h"
#include "style/zn_bold.h"
#include "style/zn_italic.h"
#include "style/zn_scale.h"




#include "style/zn_text_style.h"
#include "glyph/zn_load.h"
#include "core/zn_types.h"


// ZIEN
#include "ext/zn_svg.h"
#include "ext/zn_bidi.h"
#include "layout/bidi_parser.h"
#include "ext/svg_/zn_xml_parser.h"
#include "ext/svg_/zn_svg_bundle.h"
#include "ext/svg_/zn_glyph_to_svg.h"
#include "ext/svg_/zn_svg_builder.h"
#include "ext/svg_/zn_svg_parser.h"

#include "utils/zn_utf_helper.h"
#include "ext/svgb/zn_viewbox.h"
#include "ext/svgb/zn_svgb_emoji_slot.h"
#include "ext/svgb/zn_svgb_parser.h"
#include "font/emoji_manager.h"
#include "text/zn_text_blob.h"
#include "models/zn_models.h"
#include "layout/zn_layout_iterator.h"
#include "glyph/zn_convert.h"

#ifdef ZN_CACHE_ENABLE
    #include "cache_/zn_cache_manager.h"
#endif

#include "glyph/zn_glyph_helper.h"
#include "layout/zn_layout_metrics.h"
#include "text/zn_text_metrics.h"
#include "glyph/zn_metrics_calculator.h"

#include "shapers/zn_shaper_obj.h"
#include "shapers/zn_harfbuzz_shaper.h"
#include "shapers/zn_svg_shaper.h"



#include "font/zn_glyph_bundle.h"

#include "font/zn_font.h"

#include "font/zn_font_bundle.h"

#include "font/zn_emoji_bundle.h"

#include "font/zn_shaper_run.h"
#include "font/zn_font_collection.h"


// #include "models/classes.h"


#include "text/handlers_/zn_handler_obj.h"
#include "text/handlers_/zn_bitmap_handler.h"
#include "text/handlers_/zn_svg_handler.h"

#include "text/zn_text_shaper.h"
#include "text/zn_text_layout.h"
#include "text_painters/zn_text_canvas.h"



#include "text/handlers_/zn_handler_impl.h"
#include "text/handlers_/zn_bitmap_handler_impl.h"
#include "text/handlers_/zn_svg_handler_impl.h"

#include "text/zn_text_renderer.h"






