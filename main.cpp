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

// std::map<std::string, std::vector<std::string>> test = {
//     {"/Users/bitels/Desktop/Ffonts/dejavu-sans/DejaVuSans-Bold.ttf",
//      {"j",
//       "jjjj",
//       "fj",
//       "jf",
//       "Tj",
//       "jT",
//       "Ay",
//       "yA",
//       "AV",
//       "VA",
//       "WA",
//       "To",
//       "Yo",
//       "Qy",
//       "fij",
//       "jfy"}},

//       {"/Users/bitels/Desktop/Ffonts/dejavu-sans/DejaVuSansCondensed.ttf",
//      {"j",
//       "jjjj",
//       "fj",
//       "jf",
//       "Tj",
//       "jT",
//       "Ay",
//       "yA",
//       "AV",
//       "VA",
//       "WA",
//       "To",
//       "Yo",
//       "Qy",
//       "fij",
//       "jfy"}},

//           {"/Users/bitels/Desktop/Ffonts/dejavu-sans/DejaVuSans-BoldOblique.ttf",
//      {"j",
//       "jjjj",
//       "fj",
//       "jf",
//       "Tj",
//       "jT",
//       "Ay",
//       "yA",
//       "AV",
//       "VA",
//       "WA",
//       "To",
//       "Yo",
//       "Qy",
//       "fij",
//       "jfy"}},
//   {"/Users/bitels/Desktop/Ffonts/Liberation Serif/LiberationSerif-Italic.ttf",
//      {"j",
//       "jjjj",
//       "fj",
//       "jf",
//       "Tj",
//       "jT",
//       "Ay",
//       "yA",
//       "AV",
//       "VA",
//       "WA",
//       "To",
//       "Yo",
//       "Qy",
//       "fij",
//       "jfy"}},

//     };

// for (const auto &[fontPath, strings] : test)
// {
//   std::filesystem::path path(fontPath);

//   std::string fontName = path.stem().string();

//   std::cout << "Font: " << fontName << '\n';

//   for (const auto &text : strings)
//   {
//     text_renderer.render_text(text, fontPath);
//     std::string result = fontName + std::string("_") + text + std::string(".png");
//     // std::cout<<result;
//     text_renderer.save_png(result);
//   }
// }