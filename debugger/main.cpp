#define SDL_MAIN_HANDLED
#include <iostream>
#include <chrono>
#include <popl.hpp>
#include "debugger.h"
#include "config.h"
#include "scheduler.h"

using namespace std;

void measure_frame_without_render(Scheduler *scheduler) {
    long long frames = 0;
    auto t1 = std::chrono::steady_clock::now();
    while (true){
        scheduler->tick_frame();
        frames++;
        auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
        double elapse = chrono::duration<double, std::milli>(t2-t1).count();
        if (elapse/1000 > 5) {
            std::cout<<"Frames count: "<<frames<<" Seconds: "<<elapse / 1000<<" Average FPS: "<<frames / (elapse/1000)<<"\n";
            break;
        }
    }
}

std::string get_save_file_path(std::string file_path) {
    std::size_t ext_delim = file_path.find_last_of('.');
    file_path = file_path.substr(0, ext_delim) + ".sav";
    return file_path;
}

bool read_file(const char *file, uint8_t **output, uint32_t *size) {
    FILE *f = fopen(file, "rb");
    if (f == nullptr) {
        std::cerr << "File not found or invalid! "<<file<<"\n";
        return false;
    }
    fseek(f, 0L, SEEK_END);
    if (*size == 0) *size = ftell(f);
    if (*output == nullptr) *output = new uint8_t[*size];
    fseek(f, 0L, SEEK_SET);
    fread(*output, sizeof(uint8_t),*size, f);
    fclose(f);
    return true;
}

bool write_file(const char *file, uint8_t *data, uint32_t size) {
    FILE *f = fopen(file, "wb");
    if (f == nullptr) {
        std::cerr << "Something went wrong while opening save file\n";
        return false;
    }
    uint32_t bytes_written = fwrite(data, sizeof(uint8_t), size, f);
    if (bytes_written != size) {
        std::cerr << "Written less than input!\n";
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

Scheduler scheduler;
Debugger debugger(&scheduler);

void render_callback(uint16_t *frame_buffer) {
    debugger.render(frame_buffer);
}

void audio_callback(short *sample, uint16_t count) {
    debugger.queue_audio(sample, count);
}

void joypad_callback(uint8_t *key_state) {
    debugger.capture_keyboard(key_state);
}

int main(int argc , char **argv){
    // Parse arguments
    popl::OptionParser op("Allowed Options");
    auto help_options = op.add<popl::Switch>("h", "help", "Get help");
    auto rom_path_option = op.add<popl::Value<std::string>>("r", "rom", "path to rom file");
    auto debug_option = op.add<popl::Switch>("d", "debug", "Enable debugging");
    op.parse(argc, argv);

    bool debug_mode = false;
    if (debug_option->is_set()) {
        debug_mode = true;
    }
    if (!rom_path_option->is_set()) {
        cerr<<"No rom specified\n";
        return -1;
    }

    // Read ROM data
    string rom_path = rom_path_option->value();
    cout<<rom_path<<endl;

    uint8_t *rom_data = nullptr;
    uint32_t rom_size = 0;
    bool is_rom_success = read_file(rom_path.c_str(), &rom_data, &rom_size);

    if (is_rom_success) {
        bool cart_init = scheduler.set_cartridge_rom(rom_data, rom_size);
        if (!cart_init) return -1;
    }
    else return -1;

    // Load boot rom. In this implementation, load based on ROM type
    const char *boot_path = scheduler.cpu.bus.cartridge.header.cgb_flag ? "cgb_boot.bin" : "dmg_boot.bin";
    uint8_t *boot_data = nullptr;
    uint32_t boot_size = 0;
    bool is_boot_found = read_file(boot_path, &boot_data, &boot_size);

    if (is_boot_found) scheduler.set_boot_rom(boot_data, scheduler.cpu.bus.cartridge.header.cgb_flag);
    else return -1;

    // Load save (if exists)
    std::string save_path = get_save_file_path(rom_path);
    uint8_t *sram_data = nullptr;
    uint32_t sram_size = 0;
    bool is_save_exist = read_file(save_path.c_str(), &sram_data, &sram_size);
    if (is_save_exist) scheduler.load_sram(sram_data);
    else std::cout << "No save file found for rom "<<rom_path<<"\n";

    // measure_frame_without_render(&scheduler);

    debugger.init(debug_mode);
    scheduler.set_render_callback(&render_callback);
    scheduler.set_audio_callback(&audio_callback);
    scheduler.set_joypad_callback(&joypad_callback);

    auto t1 = std::chrono::steady_clock::now();
    while (!debugger.done){
        scheduler.tick_frame();
        auto t2 = std::chrono::steady_clock::now(); // Capture render + cycle time
        double elapse = chrono::duration<double, std::milli>(t2-t1).count();
        debugger.last_frame_duration = elapse;

        // if (elapse < MS_PER_FRAME) { // If still have some time left in this frame -> Sleep
        //     std::this_thread::sleep_for(chrono::duration<double, milli>(MS_PER_FRAME - elapse));
        // }
        t1 = std::chrono::steady_clock::now(); // Capture time at the start of new frame

        // debugger.render(scheduler.cpu.bus.ppu.frame_buffer);
    }
    debugger.end();

    // Save SRAM if exists
    if (scheduler.get_sram() != nullptr) write_file(save_path.c_str(), scheduler.get_sram(), sram_size);
    return 0;
}