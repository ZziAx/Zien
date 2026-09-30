#define ZN_CACHE_ENABLE
#include <map>
#include "zien.h"

int main()
{

  std::string s = "hello world!";

  ZN_TextRenderer text_renderer = ZN_TextRenderer();
  text_renderer.render_text(s, std::string("/Users/bitels/Desktop/Ffonts/YekanBakh 3 ProPlus [@fontiranir]/Yekan Bakh Family/ttf/YekanBakh-Regular.ttf"));
  text_renderer.save_png();
  return 0;
}
