use std::ops::Range;
use unicode_bidi::{BidiInfo, Level};

pub struct Bidi {}

 impl Bidi {
    pub fn new() -> Self {
        Bidi {}
    }

    pub fn process(&self, text: &str) -> String {
        // let text = "سلامHello world دنیا! ";

        let mut f = String::new(); // mutable String

        let bidi_info = BidiInfo::new(text, None); // None → auto base direction
        
        
        let mut i = 0;
        
        for para in &bidi_info.paragraphs {
            let line_range: Range<usize> = para.range.clone(); // <-- this is what reorder_line needs
            let visual = bidi_info.reorder_line(para, line_range);
            // f += "\n{}" + visual;

            if(i!=0){
            f.push_str("\n"); // add newline

            }
            f.push_str(&visual);
            // println!("{}", visual);

            i+=1;
        }

    //    f =  f.strip_prefix("\n").unwrap_or(&f).to_string();
    //    f =  f.strip_suffix("\n").unwrap_or(&f).to_string();

        // f.strip_suffix("\n");
        return f;
    }
}
