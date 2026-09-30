// font_to_svg.hpp - Read Font in TrueType (R) format, write SVG
// Copyright Don Bright 2013 <hugh.m.bright@gmail.com>

// modified by github user willzyba,
// see https://github.com/donbright/font_to_svg/pull/3

/*

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.

 License based on zlib license, by Jean-loup Gailly and Mark Adler
 */

/*

 This program reads a TTF (TrueType (R)) file and outputs an SVG path.

 See these sites for more info.
 Basic Terms: http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
 FType + outlines: http://www.freetype.org/freetype2/docs/reference/ft2-outline_processinhtml
 FType + contours: http://www.freetype.org/freetype2/docs/glyphs/glyphs-6.html
 TType contours: https://developer.apple.com/fonts/TTRefMan/RM01/Chap1.html
 TType contours2: http://www.truetype-typography.com/ttoutln.htm
 Non-zero winding rule: http://en.wikipedia.org/wiki/Nonzero-rule
 SVG paths: http://www.w3schools.com/svg/svg_path.asp
 SVG paths + nonzero: http://www.w3.org/TR/SVG/paintinhtml#FillProperties

 TrueType is a trademark of Apple Inc. Use of this mark does not imply
 endorsement.

 */

#ifndef __font_to_svg_h__
#define __font_to_svg_h__

// #include "Stdafx.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace LatexDrawGraphics
{

    std::stringstream debug;

    class CFreeType
    {
    public:
        std::string filename;
        FT_Library library;
        FT_Face face;
        FT_Error error;

        CFreeType()
        {
            filename = std::string("");
        }

        CFreeType(FT_Face face) : face(face) {}

        CFreeType(std::string fname)
        {
            filename = fname;
            error = FT_Init_FreeType(&library);
            debug << "Init error code: " << error;

            // Load a typeface
            error = FT_New_Face(library, filename.c_str(), 0, &face);
            debug << "\nFace load error code: " << error;
            debug << "\nfont filename: " << filename;
            if (error)
            {
                std::cerr << "problem loading file " << filename << "\n";
                exit(1);
            }
            debug << "\nFamily Name: " << face->family_name;
            debug << "\nStyle Name: " << face->style_name;
            debug << "\nNumber of faces: " << face->num_faces;
            debug << "\nNumber of glyphs: " << face->num_glyphs;
        }

        void free()
        {
            debug << "\n<!--";
            error = FT_Done_Face(face);
            debug << "\nFree face. error code: " << error;
            error = FT_Done_FreeType(library);
            debug << "\nFree library. error code: " << error;
            debug << "\n-->\n";
        }
    };

    class CFreeGlypth
    {
    public:
        int _codepoint;
        FT_GlyphSlot _slot;
        FT_Error _error;
        FT_Outline _outline;
        FT_Glyph_Metrics _gm;
        FT_Face _face;
        CFreeType _file;

        FT_Vector *_points;
        unsigned char *_tags;
        unsigned short *_contours;

        long _bbwidth, _bbheight;

        CFreeGlypth(CFreeType &f, std::string unicode_str)
        {
            _file = f;
            init(unicode_str);
        }

        CFreeGlypth(CFreeType &f, int gidx)
        {
            _file = f;
            init(gidx);
        }

        CFreeGlypth(const char *filename, std::string unicode_str)
        {
            _file = CFreeType(std::string(filename));
            init(unicode_str);
        }

        CFreeGlypth(const char *filename, const char *unicode_c_str)
        {
            _file = CFreeType(std::string(filename));
            init(std::string(unicode_c_str));
        }

        void free()
        {
            _file.free();
        }

        void init(FT_UInt glyph_index)
        {
            _face = _file.face;

            // Load the Glyph into the face's Glyph Slot + print details

            _error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_SCALE);

            _slot = _face->glyph;
            _outline = _slot->outline;

            // char glyph_name[1024];
            // FT_Get_Glyph_Name(_face, glyph_index, glyph_name, 1024);
            _gm = _slot->metrics;

            _points = _outline.points;
            for (int i = 0; i < _outline.n_points; i++)
                _points[i].y *= 1;

            _bbheight = _face->bbox.yMax - _face->bbox.yMin;
            _bbwidth = _face->bbox.xMax - _face->bbox.xMin;

            _tags = _outline.tags;
            _contours = _outline.contours;
        }

        void init(std::string unicode_s)
        {
            _face = _file.face;
            _codepoint = unicode_s.c_str()[0];

            // Load the Glyph into the face's Glyph Slot + print details
            FT_UInt glyph_index = FT_Get_Char_Index(_face, _codepoint);

            _error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_SCALE);

            _slot = _face->glyph;
            _outline = _slot->outline;

            // char glyph_name[1024];
            // FT_Get_Glyph_Name(_face, glyph_index, glyph_name, 1024);
            _gm = _slot->metrics;

            _points = _outline.points;
            for (int i = 0; i < _outline.n_points; i++)
                _points[i].y *= -1;

            _bbheight = _face->bbox.yMax - _face->bbox.yMin;
            _bbwidth = _face->bbox.xMax - _face->bbox.xMin;
            _tags = _outline.tags;
            _contours = _outline.contours;
        }

        // Create Header
        std::string svgheader()
        {
            std::stringstream tmp;

            tmp << "\n<svg width='" << _bbwidth << "px'"
                << " height='" << _bbheight << "px'"
                << " xmlns='http://www.w3.org/2000/svg' version='1.1'>";

            return tmp.str();
        }

        // Draw Border
        std::string svgborder()
        {
            std::stringstream tmp;

            tmp << "\n <rect fill='none' stroke='black'"
                << " width='" << _bbwidth - 1 << "'"
                << " height='" << _bbheight - 1 << "'/>";
            return tmp.str();
        }

        std::string svgtransform()
        {
            // TrueType points are not in the range usually visible by SVG.
            // they often have negative numbers etc. So.. here we
            // 'transform' to make visible.
            //
            // note also that y coords of all points have been flipped during
            // init() so that SVG Y positive = Truetype Y positive
            std::stringstream tmp("");
            long yadj = _gm.horiBearingY + _gm.vertBearingY + 100;
            long xadj = 100;

            tmp << "\n <g fill-rule='nonzero' "
                << " transform='translate(" << xadj << " " << yadj << ")'"
                << ">";
            return tmp.str();
        }

        // Add Axis
        std::string axes()
        {
            std::stringstream tmp("");
            tmp << "\n\n  <!-- draw axes --> ";
            tmp << "\n <path stroke='blue' stroke-dasharray='5,5' d='"
                << " M" << -_bbwidth << "," << 0
                << " L" << _bbwidth << "," << 0
                << " M" << 0 << "," << -_bbheight
                << " L" << 0 << "," << _bbheight
                << " '/>";
            return tmp.str();
        }

        // Draw Bearing + Advance Box
        std::string typography_box()
        {
            std::stringstream tmp("");

            long x1 = 0;
            long x2 = _gm.horiAdvance;
            long y1 = -_gm.vertBearingY - _gm.height;
            long y2 = y1 + _gm.vertAdvance;

            tmp << "\n <path stroke='blue' fill='none' stroke-dasharray='10,16' d='"
                << " M" << x1 << "," << y1
                << " M" << x1 << "," << y2
                << " L" << x2 << "," << y2
                << " L" << x2 << "," << y1
                << " L" << x1 << "," << y1
                << " '/>";

            return tmp.str();
        }

        // Draw points as circles
        std::string points()
        {
            return std::string("");
            std::stringstream tmp("");

            for (int i = 0; i < _outline.n_points; i++)
            {
                bool this_is_ctrl_pt = !(_tags[i] & 1);
                bool next_is_ctrl_pt = !(_tags[(i + 1) % _outline.n_points] & 1);
                long x = _points[i].x;
                long y = _points[i].y;
                long nx = _points[(i + 1) % _outline.n_points].x;
                long ny = _points[(i + 1) % _outline.n_points].y;
                int radius = 5;
                if (i == 0)
                    radius = 10;

                std::string color;
                if (this_is_ctrl_pt)
                    color = "none";
                else
                    color = "blue";
                if (this_is_ctrl_pt && next_is_ctrl_pt)
                {
                    tmp << "\n  <!-- halfway pt between 2 ctrl pts -->";
                    tmp << "<circle"
                        << " fill='" << "blue" << "'"
                        << " stroke='black'"
                        << " cx='" << (x + nx) / 2 << "' cy='" << (y + ny) / 2 << "'"
                        << " r='" << 2 << "'"
                        << "/>";
                };
                tmp << "\n  <!--" << i << "-->";
                tmp << "<circle"
                    << " fill='" << color << "'"
                    << " stroke='black'"
                    << " cx='" << _points[i].x << "' cy='" << _points[i].y << "'"
                    << " r='" << radius << "'"
                    << "/>";
            }

            return tmp.str();
        }

        // Draw straight lines between points
        std::string pointlines()
        {
            return std::string("");

            std::stringstream tmp("");
            tmp << "\n  <path fill='none' stroke='green' d='";
            tmp << "\n   M " << _points[0].x << "," << _points[0].y << "\n";
            tmp << "\n  '/>";

            for (int i = 0; i < _outline.n_points - 1; i++)
            {
                std::string dash_mod("");
                for (int j = 0; j < _outline.n_contours; j++)
                {
                    if (i == _contours[j])
                        dash_mod = " stroke-dasharray='3'";
                }
                tmp << "\n  <path fill='none' stroke='green'";
                tmp << dash_mod;
                tmp << " d='";
                tmp << " M " << _points[i].x << "," << _points[i].y;
                tmp << " L " << _points[(i + 1) % _outline.n_points].x << "," << _points[(i + 1) % _outline.n_points].y;
                tmp << "\n  '/>";
            }
            return tmp.str();
        }

        // Label points
        std::string labelpts()
        {
            return std::string("");

            std::stringstream tmp("");
            for (int i = 0; i < _outline.n_points; i++)
            {
                tmp << "\n <g font-family='SVGFreeSansASCII,sans-serif' font-size='10'>\n";
                tmp << "  <text id='revision'";
                tmp << " x='" << _points[i].x + 5 << "'";
                tmp << " y='" << _points[i].y - 5 << "'";
                tmp << " stroke='none' fill='darkgreen'>\n";
                tmp << "  " << _points[i].x << "," << _points[i].y;
                tmp << "  </text>\n";
                tmp << " </g>\n";
            }
            return tmp.str();
        }

        /* Draw the outline of the font as svg.
         There are three main components.
         1. the points
         2. the 'tags' for the points
         3. the contour indexes (that define which points belong to which contour)
         */
        std::string outline()
{
    if (_outline.n_points == 0) return "<!-- font had 0 points -->";
    if (_outline.n_contours == 0) return "<!-- font had 0 contours -->";

    std::stringstream svg;
    svg << "d='";

    FT_Outline outline = _outline;
    FT_Vector *points = _points;
    unsigned char *tags = _tags;
    unsigned short *contours = _contours;

    auto fx = [&](long v){ return (float)v / 64.0f; }; // scale from 26.6
    auto fy = [&](long v){ return (float) -v / 64.0f; };

    int contour_start = 0;

    for(int ci = 0; ci < outline.n_contours; ci++)
    {
        int contour_end = contours[ci];
        int count = contour_end - contour_start + 1;

        // Move to start
        float sx = fx(points[contour_start].x);
        float sy = fy(points[contour_start].y);
        svg << "M" << sx << "," << sy;

        char mode = 'M';

        for(int j = 0; j < count; j++)
        {
            int a = contour_start + j;
            int b = contour_start + ((j + 1) % count);
            int c = contour_start + ((j + 2) % count);

            float x  = fx(points[a].x);
            float y  = fy(points[a].y);
            float nx = fx(points[b].x);
            float ny = fy(points[b].y);
            float nnx= fx(points[c].x);
            float nny= fy(points[c].y);

            bool on     = tags[a] & 1;
            bool nextOn = tags[b] & 1;
            bool nnOn   = tags[c] & 1;

            if (!on && !nextOn)
            {
                // midpoint between two off-curve points
                nx = (x + nx) * 0.5f;
                ny = (y + ny) * 0.5f;
                on = true;
            }

            if (on && !nextOn)  // Q segment
            {
                if (!nnOn)
                {
                    nnx = (nx + nnx) * 0.5f;
                    nny = (ny + nny) * 0.5f;
                }

                if (mode != 'Q') svg << " Q";
                svg << nx << "," << ny << " " << nnx << "," << nny;
                mode = 'Q';
            }
            else if (on && nextOn) // L line
            {
                if (mode != 'L') svg << " L";
                svg << nx << "," << ny;
                mode = 'L';
            }
        }

        svg << " Z";
        contour_start = contour_end + 1;
    }

    svg << "'";
    return svg.str();
}

        std::string outline2()
        {

            if (_outline.n_points == 0)
                return "<!-- font had 0 points -->";
            if (_outline.n_contours == 0)
                return "<!-- font had 0 contours -->";

            std::stringstream svg;

            svg << "d='";

            /* tag bit 1 indicates whether its a control point on a bez curve
             or not. two consecutive control points imply another point halfway
             between them */

            // Step 1. move to starting point (M x-coord y-coord )
            // Step 2. decide whether to draw a line or a bezier curve or to move
            // Step 3. for bezier: Q control-point-x control-point-y,
            //		         destination-x, destination-y
            //         for line:   L x-coord, y-coord
            //         for move:   M x-coord, y-coord

            int contour_starti = 0;

            // Create a local copy of pointers
            FT_Outline outline = _outline;
            FT_Vector *points = _points;
            unsigned char *tags = _tags;
            unsigned short *contours = _contours;

            // printf("horizontal adv %i\n",_gm.horiAdvance);
            for (int i = 0; i < outline.n_contours; i++)
            {
                int contour_endi = contours[i];

                int offset = contour_starti;
                int npts = contour_endi - contour_starti + 1;

                svg << "M" << points[contour_starti].x << "," << -points[contour_starti].y;

                char mode = 'M';
                for (int j = 0; j < npts; j++)
                {
                    int a = j % npts + offset;
                    int nexti = (j + 1) % npts + offset;
                    int nextnexti = (j + 2) % npts + offset;

                    long x = points[a].x;
                    long y = -points[a].y;
                    long nx = points[nexti].x;
                    long ny = -points[nexti].y;

                    bool this_tagbit1 = (tags[a] & 1);
                    bool next_tagbit1 = (tags[nexti] & 1);
                    bool nextnext_tagbit1 = (tags[nextnexti] & 1);
                    bool this_isctl = !this_tagbit1;
                    bool next_isctl = !next_tagbit1;
                    bool nextnext_isctl = !nextnext_tagbit1;

                    if (this_isctl && next_isctl)
                    {
                        // two adjacent ctl pts. adding point halfway between;
                        // reseting x and y ";
                        x = (x + nx) / 2;
                        y = (y + ny) / 2;
                        this_isctl = false;

                        //  Check if first pt in contour was a ctrl pt. moving to non-ctrl pt
                        if (j == 0)
                        {
                            if (mode != 'M')
                                svg << "M";
                            else
                                svg << " ";
                            svg << x << " " << y;
                            mode = 'M';
                        }
                    }

                    if (!this_isctl && next_isctl)
                    {
                        long nnx = points[nextnexti].x;
                        long nny = -points[nextnexti].y;
                        if (nextnext_isctl)
                        {
                            // two ctl pts coming. adding point halfway between
                            // reseting nnx and nny to halfway pt
                            nnx = (nx + nnx) / 2;
                            nny = (ny + nny) / 2;
                        }
                        // produce bezier path
                        if (mode != 'Q')
                            svg << "Q";
                        else
                            svg << " ";
                        svg << nx << " " << ny << " " << nnx << " " << nny;
                        mode = 'Q';
                    }
                    else if (!this_isctl && !next_isctl)
                    {
                        // line to new point
                        if (mode != 'L')
                           svg  << "L";
                        else
                            svg << " ";
                        svg << nx << " " << ny;
                        mode = 'L';
                    }
                    else if (this_isctl && !next_isctl)
                    {
                        // this is ctrl pt. skipping to
                    }
                }

                contour_starti = contour_endi + 1;
                svg << "Z";
                mode = 'Z';
            }

            svg << "'";
            return svg.str();
        }

        std::string svgfooter()
        {
            return "\n </g>\n</svg>\n";
        }

    };

} // namespace

#endif


// #ifndef __font_to_svg_h__
// #define __font_to_svg_h__

// // #include "Stdafx.h"

// // #include <ft2build.h>
// // #include FT_FREETYPE_H
// #include <iostream>
// #include <sstream>
// #include <vector>
// #include <string>

// namespace LatexDrawGraphics
// {

//     std::stringstream debug;

//     class CFreeType
//     {
//     public:
//         std::string filename;
//         FT_Library library;
//         FT_Face face;
//         FT_Error error;

//         CFreeType()
//         {
//             filename = std::string("");
//         }

//         CFreeType(FT_Face face) : face(face) {}

//         CFreeType(std::string fname)
//         {
//             filename = fname;
//             error = FT_Init_FreeType(&library);
//             debug << "Init error code: " << error;

//             // Load a typeface
//             error = FT_New_Face(library, filename.c_str(), 0, &face);
//             debug << "\nFace load error code: " << error;
//             debug << "\nfont filename: " << filename;
//             if (error)
//             {
//                 std::cerr << "problem loading file " << filename << "\n";
//                 exit(1);
//             }
//             debug << "\nFamily Name: " << face->family_name;
//             debug << "\nStyle Name: " << face->style_name;
//             debug << "\nNumber of faces: " << face->num_faces;
//             debug << "\nNumber of glyphs: " << face->num_glyphs;
//         }

//         void free()
//         {
//             debug << "\n<!--";
//             error = FT_Done_Face(face);
//             debug << "\nFree face. error code: " << error;
//             error = FT_Done_FreeType(library);
//             debug << "\nFree library. error code: " << error;
//             debug << "\n-->\n";
//         }
//     };

//     class CFreeGlypth
//     {
//     public:
//         int _codepoint;
//         FT_GlyphSlot _slot;
//         FT_Error _error;
//         FT_Outline _outline;
//         FT_Glyph_Metrics _gm;
//         FT_Face _face;
//         CFreeType _file;

//         FT_Vector *_points;
//         unsigned char *_tags;
//         unsigned short *_contours;

//         long _bbwidth, _bbheight;

//         CFreeGlypth(CFreeType &f, std::string unicode_str)
//         {
//             _file = f;
//             init(unicode_str);
//         }

//         CFreeGlypth(CFreeType &f, int gidx)
//         {
//             _file = f;
//             init(gidx);
//         }

//         CFreeGlypth(const char *filename, std::string unicode_str)
//         {
//             _file = CFreeType(std::string(filename));
//             init(unicode_str);
//         }

//         CFreeGlypth(const char *filename, const char *unicode_c_str)
//         {
//             _file = CFreeType(std::string(filename));
//             init(std::string(unicode_c_str));
//         }

//         void free()
//         {
//             _file.free();
//         }

//         void init(FT_UInt glyph_index)
//         {
//             _face = _file.face;

//             // Load the Glyph into the face's Glyph Slot + print details

//             _error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_SCALE);

//             _slot = _face->glyph;
//             _outline = _slot->outline;

//             // char glyph_name[1024];
//             // FT_Get_Glyph_Name(_face, glyph_index, glyph_name, 1024);
//             _gm = _slot->metrics;

//             _points = _outline.points;
//             // NOTE: **DO NOT** flip Y here. Keep FreeType coordinates as-is.
//             // for (int i = 0; i < _outline.n_points; i++)
//             //     _points[i].y *= -1;

//             _bbheight = _face->bbox.yMax - _face->bbox.yMin;
//             _bbwidth = _face->bbox.xMax - _face->bbox.xMin;

//             _tags = _outline.tags;
//             _contours = _outline.contours;
//         }

//         void init(std::string unicode_s)
//         {
//             _face = _file.face;
//             _codepoint = unicode_s.c_str()[0];

//             // Load the Glyph into the face's Glyph Slot + print details
//             FT_UInt glyph_index = FT_Get_Char_Index(_face, _codepoint);

//             _error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_NO_SCALE);

//             _slot = _face->glyph;
//             _outline = _slot->outline;

//             // char glyph_name[1024];
//             // FT_Get_Glyph_Name(_face, glyph_index, glyph_name, 1024);
//             _gm = _slot->metrics;

//             _points = _outline.points;
//             // NOTE: do NOT flip Y here.
//             // for (int i = 0; i < _outline.n_points; i++)
//             //     _points[i].y *= -1;

//             _bbheight = _face->bbox.yMax - _face->bbox.yMin;
//             _bbwidth = _face->bbox.xMax - _face->bbox.xMin;
//             _tags = _outline.tags;
//             _contours = _outline.contours;
//         }

//         // Create Header
//         std::string svgheader()
//         {
//             std::stringstream tmp;

//             tmp << "\n<svg width='" << _bbwidth << "px'"
//                 << " height='" << _bbheight << "px'"
//                 << " xmlns='http://www.w3.org/2000/svg' version='1.1'>";

//             return tmp.str();
//         }

//         // Draw Border
//         std::string svgborder()
//         {
//             std::stringstream tmp;

//             tmp << "\n <rect fill='none' stroke='black'"
//                 << " width='" << _bbwidth - 1 << "'"
//                 << " height='" << _bbheight - 1 << "'/>";
//             return tmp.str();
//         }

//         std::string svgtransform()
//         {
//             // TrueType points are not in the range usually visible by SVG.
//             // they often have negative numbers etc. So.. here we
//             // 'transform' to make visible.
//             //
//             // note also that y coords of all points have been kept the same as FreeType.
//             long yadj = _gm.horiBearingY + _gm.vertBearingY + 100;
//             long xadj = 100;

//             std::stringstream tmp("");
//             tmp << "\n <g fill-rule='nonzero' "
//                 << " transform='translate(" << xadj << " " << yadj << ")'"
//                 << ">";
//             return tmp.str();
//         }

//         // Add Axis
//         std::string axes()
//         {
//             std::stringstream tmp("");
//             tmp << "\n\n  <!-- draw axes --> ";
//             tmp << "\n <path stroke='blue' stroke-dasharray='5,5' d='"
//                 << " M" << -_bbwidth << "," << 0
//                 << " L" << _bbwidth << "," << 0
//                 << " M" << 0 << "," << -_bbheight
//                 << " L" << 0 << "," << _bbheight
//                 << " '/>";
//             return tmp.str();
//         }

//         // Draw Bearing + Advance Box
//         std::string typography_box()
//         {
//             std::stringstream tmp("");

//             long x1 = 0;
//             long x2 = _gm.horiAdvance;
//             long y1 = -_gm.vertBearingY - _gm.height;
//             long y2 = y1 + _gm.vertAdvance;

//             tmp << "\n <path stroke='blue' fill='none' stroke-dasharray='10,16' d='"
//                 << " M" << x1 << "," << y1
//                 << " M" << x1 << "," << y2
//                 << " L" << x2 << "," << y2
//                 << " L" << x2 << "," << y1
//                 << " L" << x1 << "," << y1
//                 << " '/>";

//             return tmp.str();
//         }

//         // Draw points as circles
//         std::string points()
//         {
//             std::stringstream tmp("");

//             for (int i = 0; i < _outline.n_points; i++)
//             {
//                 bool this_is_ctrl_pt = !(_tags[i] & 1);
//                 bool next_is_ctrl_pt = !(_tags[(i + 1) % _outline.n_points] & 1);
//                 long x = _points[i].x;
//                 long y = _points[i].y; // now use stored coordinates directly
//                 long nx = _points[(i + 1) % _outline.n_points].x;
//                 long ny = _points[(i + 1) % _outline.n_points].y;
//                 int radius = 5;
//                 if (i == 0)
//                     radius = 10;

//                 std::string color;
//                 if (this_is_ctrl_pt)
//                     color = "none";
//                 else
//                     color = "blue";
//                 if (this_is_ctrl_pt && next_is_ctrl_pt)
//                 {
//                     tmp << "\n  <!-- halfway pt between 2 ctrl pts -->";
//                     tmp << "<circle"
//                         << " fill='" << "blue" << "'"
//                         << " stroke='black'"
//                         << " cx='" << (x + nx) / 2 << "' cy='" << (y + ny) / 2 << "'"
//                         << " r='" << 2 << "'"
//                         << "/>";
//                 };
//                 tmp << "\n  <!--" << i << "-->";
//                 tmp << "<circle"
//                     << " fill='" << color << "'"
//                     << " stroke='black'"
//                     << " cx='" << _points[i].x << "' cy='" << _points[i].y << "'"
//                     << " r='" << radius << "'"
//                     << "/>";
//             }

//             return tmp.str();
//         }

//         // Draw straight lines between points
//         std::string pointlines()
//         {
//             std::stringstream tmp("");
//             tmp << "\n  <path fill='none' stroke='green' d='";
//             tmp << "\n   M " << _points[0].x << "," << _points[0].y << "\n";
//             tmp << "\n  '/>";

//             for (int i = 0; i < _outline.n_points - 1; i++)
//             {
//                 std::string dash_mod("");
//                 for (int j = 0; j < _outline.n_contours; j++)
//                 {
//                     if (i == _contours[j])
//                         dash_mod = " stroke-dasharray='3'";
//                 }
//                 tmp << "\n  <path fill='none' stroke='green'";
//                 tmp << dash_mod;
//                 tmp << " d='";
//                 tmp << " M " << _points[i].x << "," << _points[i].y;
//                 tmp << " L " << _points[(i + 1) % _outline.n_points].x << "," << _points[(i + 1) % _outline.n_points].y;
//                 tmp << "\n  '/>";
//             }
//             return tmp.str();
//         }

//         // Label points
//         std::string labelpts()
//         {
//             std::stringstream tmp("");
//             for (int i = 0; i < _outline.n_points; i++)
//             {
//                 tmp << "\n <g font-family='SVGFreeSansASCII,sans-serif' font-size='10'>\n";
//                 tmp << "  <text id='revision'";
//                 tmp << " x='" << _points[i].x + 5 << "'";
//                 tmp << " y='" << _points[i].y - 5 << "'";
//                 tmp << " stroke='none' fill='darkgreen'>\n";
//                 tmp << "  " << _points[i].x << "," << _points[i].y;
//                 tmp << "  </text>\n";
//                 tmp << " </g>\n";
//             }
//             return tmp.str();
//         }

//         /* Draw the outline of the font as svg.
//          There are three main components.
//          1. the points
//          2. the 'tags' for the points
//          3. the contour indexes (that define which points belong to which contour)
//          */
//         std::string outline()
//         {
//             if (_outline.n_points == 0)
//                 return "<!-- font had 0 points -->";
//             if (_outline.n_contours == 0)
//                 return "<!-- font had 0 contours -->";

//             std::stringstream svg;

//             svg << "d='";

//             int contour_starti = 0;

//             // Create a local copy of pointers
//             FT_Outline outline = _outline;
//             FT_Vector *points = _points;
//             unsigned char *tags = _tags;
//             unsigned short *contours = _contours;

//             for (int i = 0; i < outline.n_contours; i++)
//             {
//                 int contour_endi = contours[i];

//                 int offset = contour_starti;
//                 int npts = contour_endi - contour_starti + 1;

//                 // Use stored coordinates directly (no extra negation)
//                 svg << "M" << points[contour_starti].x << "," << points[contour_starti].y;

//                 char mode = 'M';
//                 for (int j = 0; j < npts; j++)
//                 {
//                     int a = j % npts + offset;
//                     int nexti = (j + 1) % npts + offset;
//                     int nextnexti = (j + 2) % npts + offset;

//                     long x = points[a].x;
//                     long y = points[a].y;
//                     long nx = points[nexti].x;
//                     long ny = points[nexti].y;

//                     bool this_tagbit1 = (tags[a] & 1);
//                     bool next_tagbit1 = (tags[nexti] & 1);
//                     bool nextnext_tagbit1 = (tags[nextnexti] & 1);
//                     bool this_isctl = !this_tagbit1;
//                     bool next_isctl = !next_tagbit1;
//                     bool nextnext_isctl = !nextnext_tagbit1;

//                     if (this_isctl && next_isctl)
//                     {
//                         // two adjacent ctl pts. adding point halfway between;
//                         // reseting x and y ";
//                         x = (x + nx) / 2;
//                         y = (y + ny) / 2;
//                         this_isctl = false;

//                         //  Check if first pt in contour was a ctrl pt. moving to non-ctrl pt
//                         if (j == 0)
//                         {
//                             if (mode != 'M')
//                                 svg << "M";
//                             else
//                                 svg << " ";
//                             svg << x << " " << y;
//                             mode = 'M';
//                         }
//                     }

//                     if (!this_isctl && next_isctl)
//                     {
//                         long nnx = points[nextnexti].x;
//                         long nny = points[nextnexti].y;
//                         if (nextnext_isctl)
//                         {
//                             // two ctl pts coming. adding point halfway between
//                             // reseting nnx and nny to halfway pt
//                             nnx = (nx + nnx) / 2;
//                             nny = (ny + nny) / 2;
//                         }
//                         // produce bezier path (TrueType uses quadratic->Q)
//                         if (mode != 'Q')
//                             svg << "Q";
//                         else
//                             svg << " ";
//                         svg << nx << " " << ny << " " << nnx << " " << nny;
//                         mode = 'Q';
//                     }
//                     else if (!this_isctl && !next_isctl)
//                     {
//                         // line to new point
//                         if (mode != 'L')
//                             svg << "L";
//                         else
//                             svg << " ";
//                         svg << nx << " " << ny;
//                         mode = 'L';
//                     }
//                     else if (this_isctl && !next_isctl)
//                     {
//                         // this is ctrl pt. skipping to
//                     }
//                 }

//                 contour_starti = contour_endi + 1;
//                 svg << "Z";
//                 mode = 'Z';
//             }

//             svg << "'";
//             return svg.str();
//         }

//         std::string svgfooter()
//         {
//             return "\n </g>\n</svg>\n";
//         }

//     };

// } // namespace

// #endif
