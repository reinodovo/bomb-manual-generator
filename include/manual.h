#ifndef MANUAL_H
#define MANUAL_H

#include <filesystem>
#include <inja.hpp>
#include <iostream>
#include <string>
#include <utils/b64e.h>
#include <vector>

namespace fs = std::filesystem;

namespace manual {
using json = inja::json;

enum class module_type { puzzle, needy };

void clear_html_files(const fs::path &folder_path) {
  try {
    if (fs::exists(folder_path) && fs::is_directory(folder_path)) {
      for (const auto &entry : fs::directory_iterator(folder_path)) {
        if (fs::is_regular_file(entry.path()) &&
            entry.path().extension() == ".html") {
          fs::remove(entry.path());
        }
      }
    }
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error("Error clearing folder: " + std::string(e.what()));
  }
}

void ensure_directories_for_file(const fs::path &path) {
  fs::path folder_path = path.parent_path();
  try {
    if (!fs::exists(folder_path)) {
      if (!fs::create_directories(folder_path)) {
        throw std::runtime_error("Failed to create directories: " +
                                 folder_path.string());
      }
    } else {
      clear_html_files(folder_path);
    }
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error("Filesystem error: " + std::string(e.what()));
  }
}

json init(const std::string &name, const std::string &subject,
          const std::string &flavour_text, const std::string &version,
          module_type type = module_type::puzzle) {
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

fs::path data_path(int code) {
  return fs::path("./manual/data") / fs::path(std::to_string(code)) /
         fs::path("data.json");
}

void save(json &data, int code) {
  fs::path output_path = data_path(code);
  ensure_directories_for_file(output_path);
  std::ofstream output_file(output_path);
  output_file << data;
}

json render_module_pages(const fs::path &module_path, int code) {
  fs::path template_folder = module_path / fs::path("manual/templates");
  fs::path data_path = module_path / manual::data_path(code);
  fs::path diagram_path = module_path / fs::path("manual/diagram.svg");

  // read diagram
  std::ifstream ifs_diagram(diagram_path);
  std::string diagram((std::istreambuf_iterator<char>(ifs_diagram)),
                      std::istreambuf_iterator<char>());
  diagram = base64Encode(diagram);

  std::ifstream ifs(data_path);
  json data = json::parse(ifs);

  data["metadata"]["diagram"] = diagram;

  fs::path output_folder =
      fs::path("./manual") / fs::path(data["metadata"]["name"]);

  inja::Environment env;
  env.set_trim_blocks(true);
  env.set_lstrip_blocks(true);

  inja::Template module_header =
      env.parse_template("./templates/module_header.html");

  std::vector<std::string> templates_paths;
  for (const auto &entry : fs::directory_iterator(template_folder)) {
    if (fs::is_regular_file(entry.path()) &&
        entry.path().extension() == ".html") {
      templates_paths.push_back(entry.path().string());
    }
  }
  sort(templates_paths.begin(), templates_paths.end());

  json module_data;
  module_data["name"] = data["metadata"]["name"];
  module_data["pages"] = json::array();
  for (int page = 0; page < templates_paths.size(); page++) {
    inja::Template temp = env.parse_template(templates_paths[page]);
    std::string result = env.render(temp, data);
    if (page == 0)
      result = env.render(module_header, data) + result;
    module_data["pages"].push_back(json({{"content", result}}));
  }

  return module_data;
}

void render(int code, std::vector<fs::path> &module_paths) {
  fs::path output_path =
      fs::path("./web/manuals") / fs::path(std::to_string(code) + ".html");

  json data;
  data["code"] = code;
  data["pages"] = json::array();

  inja::Environment env;
  env.set_trim_blocks(true);
  env.set_lstrip_blocks(true);

  inja::Template manual_template =
      env.parse_template("./templates/manual.html");

  int rendered_pages = 0;
  for (const auto &module_path : module_paths) {
    json module_data = render_module_pages(module_path, code);
    for (auto module_page : module_data["pages"]) {
      json page;
      page["module_name"] = module_data["name"];
      page["content"] = module_page["content"];
      data["pages"].push_back(page);
      rendered_pages++;
    }
  }
  data["rendered_pages"] = rendered_pages;

  ensure_directories_for_file(output_path);
  std::ofstream output_file(output_path);
  output_file << env.render(manual_template, data);
  output_file.close();
}
} // namespace manual

#endif // MANUAL_H