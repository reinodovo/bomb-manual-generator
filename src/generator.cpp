#include <filesystem>
#include <iostream>
#include <manual.h>
#include <string>

namespace fs = std::filesystem;

void generate_data_for_module(const fs::path &module_path, int code) {
#ifdef _WIN32
  fs::path executable_relative_path = ".pio/build/manual/program.exe";
#else
  fs::path executable_relative_path = ".pio/build/manual/program";
#endif
  fs::path template_relative_folder = "manual/templates";

  fs::path executable_path =
      fs::absolute(module_path / executable_relative_path);

  std::string command = "cd " + module_path.string() + " && " +
                        executable_path.string() + " " + std::to_string(code);

  std::cout << "Running command: " << command << std::endl;

  int result = system(command.c_str());

  if (result != 0) {
    std::cerr << "Error running program" << std::endl;
    return;
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <code> <module_path1> [module_path2] ..." << std::endl;
    return 1;
  }

  int code = std::stoi(argv[1]);

  std::vector<fs::path> module_paths;
  for (int i = 2; i < argc; i++) {
    fs::path module_path = argv[i];
    generate_data_for_module(module_path, code);
    module_paths.push_back(module_path);
  }

  manual::render(code, module_paths);
}
