use resvg::render; // <- FitTo is from resvg
use resvg::tiny_skia::Pixmap;
use resvg::usvg;
use std::fs;

pub struct Svg {
    pub data: Vec<u8>,
}

impl Svg {
    pub fn from_buffer(buf: &[u8]) -> Self {
        Svg { data: buf.to_vec() }
    }

    pub fn from_vec(avec: Vec<u8>) -> Self {
        Svg { data: avec }
    }

    pub fn from_string(svg_data: String) -> Self {
        Svg {
            data: svg_data.into_bytes(), // consumes the String
        }
    }

   
    pub fn get_svg_buffer(self, scale: f32)-> Pixmap {
 
        let svg_data: &[u8] = &self.data;


        let opt = usvg::Options::default();
        let tree = usvg::Tree::from_data(&svg_data, &opt).unwrap();

        let mut pixmap = Pixmap::new(
            (tree.size().width() as u32) * scale as u32,
            (tree.size().height() as u32) * scale as u32,
        )
        .expect("Failed to create Pixmap");
        let mut pixmap_mut = pixmap.as_mut();
let transform = usvg::Transform::from_scale(scale, scale);


// transform.;
        render(
            &tree,
            transform
            ,
            &mut pixmap_mut,
        );


          return pixmap;
        // pixmap.
        // Save as PNG
    }

        // pixmap.save_png("output.png").unwrap();

}
