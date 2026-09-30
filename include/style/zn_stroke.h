
struct GlyphMetrics
{
    float x_offset;
    float y_offset;
    float x_advance;
    float width;
    float height;
    float min_y;
    float max_y;
    int left;
    int top;
};

struct ZN_Stroke
{
public:
    GlyphMetrics metrics;
    unsigned char *buffer;
    FT_Bitmap bitmap;

    float width;
    bool enabled;
    ZN_Stroke() = default;
    ZN_Stroke(float width, bool enabled) : width(width), enabled(enabled) {};
};