#![no_std]
#![no_main]

mod compress;
mod decompress;

use core::panic::PanicInfo;

use crate::{compress::compress, decompress::decompress};

use rustix::fs::{Mode, OFlags, open};
use rustix::io::{read, write};
use rustix::stdio::stdout;

unsafe extern "C" {
  fn _exit(status: i32) -> !;
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
  unsafe { _exit(1) }
}

#[unsafe(no_mangle)]
pub extern "C" fn main(_argc: i32, _argv: *const *const u8) -> i32 {
  let _ = write(unsafe { stdout() }, b"Hello from macOS no_std!\n");
  0
}
