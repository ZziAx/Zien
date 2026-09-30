mod bidi;


fn main() {
    // let text = "سلام Hello دنیا";
    let text = "سلامHello world دنیا! ";

    let bidi = bidi::bidi::Bidi::new();

    let s = bidi.process(text);

    println!("------------------");
    println!("{}", s);


}

// svg render test

// use std::fs;
// mod svg_;
// // use self::sv

// fn main() {
//     let svg_data =
//         fs::read("/Users/bitels/Desktop/git/notoemoji/noto-emoji/svg/emoji_u1f30d.svg").unwrap();
//     let svg = svg_::svg::Svg::from_vec(svg_data);

//     let pixmap = svg.get_svg_buffer(1.0);
//     pixmap.save_png("output.png");

// }
