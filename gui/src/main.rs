use std::path::Path;
use iced::{Task, Element, Padding};
use iced::alignment;
use iced::widget::{text, column, Column, container, text_input, button, row, toggler, horizontal_space, scrollable, Row};
use rfd::AsyncFileDialog;

fn main() -> iced::Result {
    iced::application("OhBoy", RomSelector::update, RomSelector::view)
        .window_size((600.0,400.0))
        .run()
}

#[derive(Default)]
struct RomSelector {
    rom_folder: String,
    debug_mode: bool,
    scale: String,
    rom_rows: Vec<RomEntry>
}
#[derive(Clone, Debug)]
enum Message {
    ROMFolderUpdated(String),
    DebugChanged(bool),
    RequestFileDialog,
    ROMTitlesUpdated(Vec<String>),
    RunEmulator(String),
    Empty
}

impl RomSelector {
    fn update(&mut self, message: Message) -> Task<Message> {
        match message {
            Message::ROMFolderUpdated(path) => {
                self.rom_folder = path.clone();
                Task::perform(scan_files(path), Message::ROMTitlesUpdated)
            }
            Message::DebugChanged(is_debug) => {
                self.debug_mode = is_debug;
                Task::none()
            }
            Message::RequestFileDialog => {
                Task::perform(select_folder(),  Message::ROMFolderUpdated)
            }
            Message::ROMTitlesUpdated(rom_list) => {
                self.rom_rows.clear();
                for entry in rom_list {
                    self.rom_rows.push(RomEntry::new(entry))
                }
                Task::none()
            }
            Message::RunEmulator(rom_path) => {
                println!("Run emulator with rom: {}",rom_path);
                Task::perform(run_emulator(rom_path, self.debug_mode), |_| Message::Empty)
            }
            Message::Empty => {
                Task::none()
            }
        }
    }
    fn view(&self) -> Element<Message> {
        let folder_text = text("ROM folder:");
        let folder_input = text_input("", &self.rom_folder).on_input(Message::ROMFolderUpdated);
        let select_folder_but = button("Select").on_press(Message::RequestFileDialog);
        let folder_row = row![folder_text, folder_input, select_folder_but]
            .spacing(3)
            .align_y(alignment::Vertical::Center);

        let debug_text = text("Debug Mode:");
        let debug_toggle = toggler(self.debug_mode).on_toggle(Message::DebugChanged);

        let settings_row = row![debug_text, debug_toggle]
            .align_y(alignment::Vertical::Center)
            .spacing(3);

        let rom_list = Column::with_children(self.rom_rows.iter().map(|entry| {entry.view()}))
            .spacing(5)
            .padding(Padding {
                top: 0f32,
                right: 20f32,
                bottom: 0f32,
                left: 0f32
            });

        let scrollable = scrollable(rom_list);

        let view = column![folder_row, settings_row, scrollable].padding(5).spacing(10);
        container(view).padding(5).into()
    }
}

#[derive(Clone)]
struct RomEntry {
    rom_path: String
}

impl RomEntry {
    fn new(path: String) -> Self {
        RomEntry {
            rom_path: path
        }
    }

    fn view(&self) -> Element<Message> {
        let rom_title = text(Path::new(&self.rom_path).file_name().unwrap().to_os_string().into_string().unwrap());
        let play_button = button("Play").on_press(Message::RunEmulator(self.rom_path.clone()));
        let rom_row = row![rom_title, horizontal_space(), play_button].align_y(alignment::Vertical::Center);

        rom_row.into()
    }
}

async fn select_folder() -> String {
    let folder = AsyncFileDialog::new().pick_folder().await;
    folder.unwrap().path().to_path_buf().into_os_string().into_string().unwrap()
}

async fn scan_files(path: String) -> Vec<String> {
    let mut rom_list = Vec::new();
    let mut dir_iter = tokio::fs::read_dir(path).await.unwrap();

    while let entry = dir_iter.next_entry().await.unwrap() {
        if entry.is_none() {
            break
        }
        let file_entry = entry.as_ref().unwrap().file_type().await.unwrap();
        if file_entry.is_file() {
            let file_name = entry.unwrap().path().to_str().unwrap().to_string();
            if file_name.ends_with(".gb") || file_name.ends_with(".gbc") {
                println!("{}", file_name.to_string());
                rom_list.push(file_name.to_string());
            }
        }
    }
    rom_list
}

async fn run_emulator(path: String, is_debug: bool) {
    let debug_flag = if is_debug {"-d"} else {""};
    let output = tokio::process::Command::new("OhBoy.exe")
        .args(["-r", &path, debug_flag]).output().await.unwrap();
    let err = std::str::from_utf8(&output.stderr).unwrap();
    println!("{}", err);
}

