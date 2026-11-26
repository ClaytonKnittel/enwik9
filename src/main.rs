mod compress;
mod decompress;
mod error;

use std::process::ExitCode;

use clap::Parser;

use crate::{
  compress::compress,
  decompress::decompress,
  error::{EnWikError, EnWikResult},
};

#[derive(Parser, Debug)]
struct Args {
  #[arg(long)]
  compress: Option<String>,

  #[arg(long)]
  decompress: Option<String>,
}

fn run() -> EnWikResult {
  let args = Args::try_parse()?;

  match (args.compress, args.decompress) {
    (Some(input), None) => compress(&input, ""),
    (None, Some(input)) => decompress(&input, ""),
    _ => {
      Err(EnWikError::new("Must specify one of `--compress` or `--decompress".to_owned()).into())
    }
  }
}

fn main() -> ExitCode {
  let result = run();
  if let Err(err) = result {
    eprintln!("Error: {err}");
    return ExitCode::FAILURE;
  }

  ExitCode::SUCCESS
}
