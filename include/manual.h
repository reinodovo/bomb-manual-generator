#ifndef MANUAL_H
#define MANUAL_H

#include <utils/b64e.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace manual {
using json = nlohmann::json;

enum class module_type { puzzle, needy };

std::string code_to_string(uint16_t code) {
  std::ostringstream ss;
  ss << std::setw(4) << std::setfill('0') << code;
  return ss.str();
}

json init(const std::string& name, const std::string& subject, const std::string& flavour_text,
          const std::string& version, module_type type = module_type::puzzle) {
  json metadata;
  metadata["name"] = name;
  metadata["subject"] = subject;
  metadata["flavour_text"] = flavour_text;
  metadata["version"] = version;
  metadata["type"] = type == module_type::puzzle ? "puzzle" : "needy";

  json data;
  data["metadata"] = metadata;
  return data;
}

fs::path data_path(uint16_t code) { return fs::path("./manual/data") / fs::path(code_to_string(code) + ".json"); }

void save(json& data, uint16_t code) {
  std::ofstream output_file(data_path(code));
  output_file << data;
}

void run(int argc, char** argv, std::function<json(uint16_t)> generate_json_for_code) {
  fs::create_directories("./manual/data");
  const int num_threads = std::thread::hardware_concurrency();
  std::atomic<uint16_t> next_code(0);

  std::vector<std::thread> workers;

  for (int t = 0; t < num_threads; ++t) {
    workers.emplace_back([&]() {
      try {
        while (true) {
          uint16_t code = next_code.fetch_add(1);
          if (code >= 10000) break;

          json data = generate_json_for_code(code);
          save(data, code);
        }
      } catch (const std::exception& e) {
        std::cerr << "Thread error: " << e.what() << "\n";
      }
    });
  }

  for (auto& t : workers) t.join();
}

}  // namespace manual

#endif  // MANUAL_H