# ZN_TextRenderer

`ZN_TextRenderer` is a simple C++ interface for rendering text using a font file. It supports rendering text to a bitmap and SVG, optional text-width constraints, retrieving rendered pixels, and saving the result as a PNG.

## Features

* Load a font from a `.ttf`/font file
* Render text to a bitmap
* Render text to SVG(Experimental)
* Optional text width for line wrapping
* Get rendered image size
* Access RGBA pixel data
* Save rendered text as PNG
* Flip PNG output vertically

## Basic Usage

### Render text to bitmap

```cpp
ZN_TextRenderer renderer;

renderer.render_text(
    "Hello World",
    "font.ttf"
);
```

The rendered bitmap can then be accessed with:

```cpp
auto& pixels = renderer.get_pixels();
```

### Set a maximum width

`width` is optional:

```cpp
renderer.render_text(
    "This is a long text that may wrap",
    "font.ttf",
    500.0
);
```

If no width is provided, the text is rendered without explicitly setting a width.

### Get rendered size

```cpp
int width;
int height;

renderer.get_size(width, height);
```

### Save as PNG

```cpp
renderer.save_png("output.png");
```

Or use the default output:

```cpp
renderer.save_png();
```

## SVG Rendering

To render text as SVG:

```cpp
renderer.render_svg(
    "Hello World",
    "font.ttf"
);
```

The SVG is generated through `ZN_SvgHandler`.

## Pixel Data

For bitmap rendering, RGBA pixel data can be accessed with:

```cpp
std::vector<unsigned char>& pixels = renderer.get_pixels();
```

Each pixel contains 4 bytes:

```text
R G B A
```

## Main API

| Function                         | Description                                   |
| -------------------------------- | --------------------------------------------- |
| `render_text(text, font)`        | Render text to bitmap                         |
| `render_text(text, font, width)` | Render text with an optional width constraint |
| `render_svg(text, font)`         | Render text to SVG                            |
| `get_pixels()`                   | Get rendered RGBA pixels                      |
| `get_size(w, h)`                 | Get rendered dimensions                       |
| `save_png()`                     | Save to `zien_test.png`                       |
| `save_png(path)`                 | Save to the specified PNG path                |
| `flipY(bool)`                    | Enable/disable vertical image flipping        |

## Dependencies

The renderer relies on the ZN text rendering components, including:

* `ZN_EngineContext`
* `ZN_Font`
* `ZN_FontCollection`
* `ZN_TextShaper`
* `ZN_TextLayout`
* `ZN_TextStyle`
* `ZN_BitmapHandler`
* `ZN_SvgHandler`
* `stb_image_write`

## Notes

`render_text()` initializes the rendering engine and creates a new font/style for the requested font path.

The optional width uses:

```cpp
std::optional<double>
```

so it can be omitted:

```cpp
renderer.render_text("Hello", "font.ttf");
```

or specified:

```cpp
renderer.render_text("Hello", "font.ttf", 300.0);
```
