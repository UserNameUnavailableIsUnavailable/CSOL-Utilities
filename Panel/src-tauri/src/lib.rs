#[cfg_attr(mobile, tauri::mobile_entry_point)]

const EXECUTOR_TEMPLATE: &str = include_str!("data/Executor.lua");

use regex::Regex;

#[tauri::command]
fn generate_executor_src(path: &str) -> String {
    // NOTE: Rust's regex is not PRCE (Perl Compatible Regular Expressions) by default.
    // We need to prefix (?m) to enable multi-line mode.
    let re = Regex::new(r"(?m)^PATH = .*$").unwrap();
    re.replace(EXECUTOR_TEMPLATE, format!("PATH = [[{}]]", path)).to_string()
}

pub fn run() {
  tauri::Builder::default()
    .invoke_handler(tauri::generate_handler![generate_executor_src])
    .setup(|app| {
      if cfg!(debug_assertions) {
        app.handle().plugin(
          tauri_plugin_log::Builder::default()
            .level(log::LevelFilter::Info)
            .build(),
        )?;
      }
      Ok(())
    })
    .run(tauri::generate_context!())
    .expect("error while running tauri application");
}
