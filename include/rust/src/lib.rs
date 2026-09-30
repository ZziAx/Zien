use std::ffi::CStr;
use std::fs;
use std::os::raw::{c_char, c_float, c_int, c_uchar};
mod bidi;
mod svg_;
use std::ffi::CString;

pub fn set_ptr(
    out_ptr: *mut *mut c_uchar,
    out_len: *mut c_int,
    width: *mut c_int,
    height: *mut c_int,

    pixmap: resvg::tiny_skia::Pixmap,
) {
    let mut buffer = pixmap.data().to_vec();

    // Leak Vec to return pointer safely
    let ptr = buffer.as_mut_ptr();
    let len = buffer.len() as c_int;
    std::mem::forget(buffer); // prevent Rust from freeing memory

    // println!("{}",buffer.len())

    unsafe {
        *out_ptr = ptr;
        *out_len = len;
        *width = pixmap.width() as c_int;
        *height = pixmap.height() as c_int;
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn get_svg_buffer_from_path(
    path: *const c_char,
    scale: c_float,
    out_ptr: *mut *mut c_uchar,
    out_len: *mut c_int,
    width: *mut c_int,
    height: *mut c_int,
) -> c_int {
    unsafe {
        let c_str = CStr::from_ptr(path);
        let file_path = match c_str.to_str() {
            Ok(s) => s,
            Err(_) => return -2, // invalid UTF-8
        };
        let svg_data = fs::read(file_path).unwrap();

        let svg: svg_::svg::Svg = svg_::svg::Svg::from_vec(svg_data);

        let pixmap = svg.get_svg_buffer(scale);

        set_ptr(out_ptr, out_len, width, height, pixmap);
    }

    0
}

#[unsafe(no_mangle)]
pub extern "C" fn get_svg_buffer_from_buffer(
    buf: *const c_uchar,
    len: c_int,
    scale: c_float,
    out_ptr: *mut *mut c_uchar,
    out_len: *mut c_int,
    width: *mut c_int,
    height: *mut c_int,
) -> c_int {
    let slice: &[u8] = unsafe { std::slice::from_raw_parts(buf, len as usize) };

    let svg = svg_::svg::Svg::from_buffer(slice);

    let pixmap = svg.get_svg_buffer(scale);

    set_ptr(out_ptr, out_len, width, height, pixmap);
    0
}

#[unsafe(no_mangle)]
pub extern "C" fn parse_bidi_text(text: *const c_char, out_ptr: *mut *mut i8) {
    unsafe {
        let c_str = CStr::from_ptr(text);
        let input_str = match c_str.to_str().unwrap_or("s") {
            "s" => "",
            s => s,
        };

        let bidi = crate::bidi::bidi::Bidi::new();

        let  processed_str = bidi.process(input_str);
        let c_string = CString::new(processed_str).unwrap();

        let ptr = c_string.into_raw(); // transfer ownership to caller
        *out_ptr = ptr;
    }
}

