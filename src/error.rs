use std::{error::Error, fmt::Display};

#[derive(Debug)]
pub struct EnWikError {
  message: String,
}

impl EnWikError {
  pub fn new(message: String) -> Self {
    EnWikError { message }
  }
}

impl Error for EnWikError {}

impl Display for EnWikError {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    write!(f, "error: {}", self.message)
  }
}

pub type EnWikResult<T = ()> = Result<T, Box<dyn Error + Send + Sync + 'static>>;
