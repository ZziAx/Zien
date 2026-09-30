#define ZN_CACHE_ENABLE
#include <map>
#include "zien.h"

int main()
{

  std::string s = "hello world!";
  std::string fontPath = "font.ttf";

  ZN_TextRenderer text_renderer = ZN_TextRenderer();
  text_renderer.render_text(s, fontPath);
  text_renderer.save_png();
  return 0;
}
