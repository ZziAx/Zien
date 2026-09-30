
typedef struct ZN_BoundingBox{
    double x;
    double y;
    double width;
    double height;
};

extern "C"
{

    int get_svg_buffer_from_path(const char *path, float scale, const uint8_t **out_ptr, int *out_len, int *width, int *height);

    int get_svg_buffer_from_buffer(const uint8_t *data, int len, float scale, const uint8_t **out_ptr, int *out_len, int *width, int *height);

    // int zn_text_to_svg(const char *text, const char *font,  char ** out_svg);
}
