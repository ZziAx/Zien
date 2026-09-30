class BitmapHelper
{

private:
    inline static int threshold = 0;

    static void debug_highlight(int idx, unsigned char *pixels)
    {

        // pixels[idx + 0] = 255.0; // R
        // pixels[idx + 1] = 1.0;   // G
        // pixels[idx + 2] = 1.0;   // B

        pixels[idx + 0] = 1.0;   // R
        pixels[idx + 1] = 1.0;   // G
        pixels[idx + 2] = 255.0; // B

        pixels[idx + 3] = 255.0;
    }

public:
    // static void paint_raw_buffer(unsigned char *pixels, int x, int y, const int width, const int height, const uint8_t *buffer, int buffer_length)
    // {

    //     int channels = 4; // assume RGBA

    //     for (int y = 0; y < height; y++)
    //     {
    //         std::memcpy(pixels + y * buffer_length,
    //                     buffer + y * buffer_length,
    //                     width * channels);

    //     }

    //     //     std::memcpy(pixels, buffer, buffer_length);

    //     // printf("erwiuoiuoerw %i", buffer_length);

    //     // int left = x;
    //     // int top = y;
    //     // int h = width;
    //     // int w = bitmap.width;

    //     // for (int y = 0; y < h; y++)
    //     // {
    //     //     for (unsigned int bx = 0; bx < w; ++bx)
    //     //     {
    //     //         int px = left + bx;
    //     //         int py = top + y; // Or flip Y if needed

    //     //         if (px >= 0 && px < width && py >= 0 && py < height)
    //     //         {
    //     //             // unsigned char *src = &buffer[y * bitmap.pitch + bx * 4]; // 4 channels
    //     //         }
    //     //     }

    //     //     // std::memcpy(pixels, buffer, buffer_length);
    //     // }
    // }

    static void paint_raw_buffer(
        unsigned char *pixels,
        int x, int y,

        const uint8_t *buffer, int buffer_length,
        int src_width, int src_height,
        int dst_width, int dst_height, // destination dimensions (scale to fit)

        int canvas_width, int canvas_height)
    {

        // printf("ewroiuuioewr %i %i %i %i\n", canvas_width, canvas_height, dst_width, dst_height);

        if (!pixels || !buffer)
            return;

        // Each pixel = 4 bytes (RGBA)
        const int src_stride = src_width * 4;
        const int dst_stride = canvas_width * 4;

        const int canvas_stride = canvas_width * 4;

        for (int row = 0; row < dst_height; ++row)
        {
            int src_row = row * src_height / dst_height;
            for (int col = 0; col < dst_width; ++col)
            {
                int src_col = col * src_width / dst_width;

                const uint8_t *src_px = buffer + (src_row * src_width + src_col) * 4;
                unsigned char *dst_px = pixels + ((y + row) * canvas_width + (x + col)) * 4;

                unsigned char r = src_px[0];
                unsigned char g = src_px[1];
                unsigned char b = src_px[2];
                unsigned char a = src_px[3];

                dst_px[0] = fmax(dst_px[0], r); // R
                dst_px[1] = fmax(dst_px[1], g); // G
                dst_px[2] = fmax(dst_px[2], b); // B
                dst_px[3] = fmax(dst_px[3], a); // A
            }
        }
        // // Loop rows
        // for (int row = 0; row < src_height; ++row) {
        //     int dst_y = y + row;
        //     if (dst_y < 0 || dst_y >= canvas_height) continue;

        //     const uint8_t *src_row = buffer + row * src_stride;
        //     unsigned char *dst_row = pixels + dst_y * dst_stride;

        //     for (int col = 0; col < src_width; ++col) {
        //         int dst_x = x + col;
        //         if (dst_x < 0 || dst_x >= canvas_width) continue;

        //         const uint8_t *src_px = src_row + col * 4;
        //         unsigned char *dst_px = dst_row + dst_x * 4;

        //         unsigned char r = src_px[0];
        //         unsigned char g = src_px[1];
        //         unsigned char b = src_px[2];
        //         unsigned char a = src_px[3];

        //         // Alpha blending (over)
        //         float alpha = a / 255.0f;
        //         dst_px[0] = static_cast<unsigned char>(r * alpha + dst_px[0] * (1 - alpha));
        //         dst_px[1] = static_cast<unsigned char>(g * alpha + dst_px[1] * (1 - alpha));
        //         dst_px[2] = static_cast<unsigned char>(b * alpha + dst_px[2] * (1 - alpha));
        //         dst_px[3] = static_cast<unsigned char>(a); // keep opaque
        //     }
        // }
    }

    static void paint_emoji(unsigned char *pixels, FT_Bitmap &bitmap, int x, int y, int width, int height, int line = 0)
    {

        int left = x;
        int top = y;
        int h = bitmap.rows;
        int w = bitmap.width;

        for (unsigned int y = 0; y < h; ++y)
        {
            for (unsigned int bx = 0; bx < w; ++bx)
            {
                int px = left + bx;
                int py = top + y; // Or flip Y if needed

                if (px >= 0 && px < width && py >= 0 && py < height)
                {
                    unsigned char *src = &bitmap.buffer[y * bitmap.pitch + bx * 4]; // 4 channels

                    unsigned char a = src[3];
                    int idx = (py * width + px) * 4;

                    if (a > threshold)
                    {
                        unsigned char r = src[2];
                        unsigned char g = src[1];
                        unsigned char b = src[0];

                        pixels[idx + 0] = r;
                        pixels[idx + 1] = g;
                        pixels[idx + 2] = b;
                        pixels[idx + 3] = a;
                    }
                    else
                    {

                        debug_highlight(idx, pixels);
                    }
                }
            }
        }
    }

    static void paint(unsigned char *pixels, unsigned char *buffer,int pitch, int x, int y, int w, int h, int cw, int ch, int line = 0, bool enable_overlap = false)
    {

        // int threshold = 80;
        int left = x;
        int top = y;

        // int h = bitmap.rows;
        // int w = bitmap.width;

        bool flipY = false;
        for (unsigned int y = 0; y < h; ++y)
        {
            for (unsigned int bx = 0; bx < w; ++bx)
            {
                int px = left + bx;
                //   int py =  (top + y);
                int py;
                if (flipY)
                {
                }
                else
                {
                    py = top + y; // Invert Y coordinate for OpenGL
                }

                if (px >= 0 && px < cw && py >= 0 && py < ch)
                {

                    int idx = (py * cw + px) * 4;

                    unsigned char alpha = buffer[y * pitch + bx];

                    if (alpha > threshold)

                    {

                        if (line == 1)
                        {
                            if (enable_overlap)
                            {
                            }

                            int current_alpha = pixels[idx + 3];
                            if (current_alpha != 255)
                            {
                                pixels[idx + 0] = fmax(200, 0); // R
                                pixels[idx + 1] = fmax(0, 0);   // G
                                pixels[idx + 2] = fmax(0, 0);   // B
                                pixels[idx + 3] = fmax(alpha, current_alpha);
                            }
                            // if (current_alpha != 0)
                            // {
                            //     // pixels[idx + 3] = fmin(alpha,current_alpha);
                            // }
                            // else
                            // {
                            //     pixels[idx + 3] = alpha;
                            // }
                            // }
                        }
                        else
                        {

                            pixels[idx + 0] = fmax(pixels[idx + 0], alpha); // R
                            pixels[idx + 1] = fmax(pixels[idx + 1], alpha); // G
                            pixels[idx + 2] = fmax(pixels[idx + 2], alpha); // B
                            pixels[idx + 3] = fmax(pixels[idx + 3], alpha);
                            // pixels[idx + 0] = fmax(pixels[idx + 0], alpha); // R
                            // pixels[idx + 1] = fmax(pixels[idx + 1], alpha); // G
                            // pixels[idx + 2] = fmax(pixels[idx + 2], alpha); // B
                            // pixels[idx + 3] = fmax(pixels[idx + 3], alpha);
                        }
                    }

                    else
                    {
                    }

                        // debug_highlight(idx, pixels);

                // debug_highlight((y + row) * canvas_width + (x + col) * 4, pixels);

                }
            }
        }
    }
};