#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " INPUT OUTPUT" << std::endl;
    return 1;
  }

  int retcode = 0;
  std::ifstream input_stream;
  std::ofstream output_stream;
  try {
    if (sizeof(uint32_t) != 4 ||
        sizeof(char) != 1 ||
        sizeof(float) != 4) {
      throw std::runtime_error("Wrong data type sizes");
    }

    std::string program_name, input_fname, output_fname;
    char buf[80];
    bool ascii2bin;

    input_fname = argv[1];
    output_fname = argv[2];

    input_stream.open(input_fname, std::ios::in | std::ios::binary);
    if (!input_stream) {
      throw std::runtime_error("Error opening " + input_fname + " for reading");
    }
    if (!input_stream.read(buf, 5)) {
      throw std::runtime_error("Unable to determine input type: EOF reached");
    }
    input_stream.close();

    buf[5] = '\0';
    ascii2bin = (strcmp(buf, "solid") == 0);

    if (ascii2bin) {
      input_stream.open(input_fname, std::ios::in);
      output_stream.open(output_fname, std::ios::out | std::ios::binary);
    } else {
      input_stream.open(input_fname, std::ios::in | std::ios::binary);
      output_stream.open(output_fname, std::ios::out);
    }

    if (!input_stream) {
      throw std::runtime_error("Error opening " + input_fname + " for reading");
    }
    if (!output_stream) {
      throw std::runtime_error("Error opening " + output_fname +
          " for writing");
    }

    uint32_t facet_count = 0;
    if (ascii2bin) {  // ascii to binary
      std::runtime_error format_error("Invalid STL format");
      std::string token;

      input_stream >> token;
      if (token != "solid") throw format_error;

      std::getline(input_stream, token);
      token.erase(0, token.find_first_not_of(' '));
      if (token.size() > 80) throw format_error;
      memset(buf, 0, 80);
      std::copy(token.begin(), token.end(), buf);
      output_stream.write(buf, 80);

      memset(buf, 0, 80);
      output_stream.write(buf, 4);

      while (input_stream >> token) {
        if (token == "facet") {
          float vals[12];

          input_stream >> token;
          if (token != "normal") throw format_error;
          for (int i = 0; i < 3; i++) {
            input_stream >> vals[i];
            if (!input_stream) throw format_error;
          }

          input_stream >> token;
          if (token != "outer") throw format_error;
          input_stream >> token;
          if (token != "loop") throw format_error;

          for (int i = 3; i < 12;) {
            input_stream >> token;
            if (token != "vertex") throw format_error;
            for (int j = 0; j < 3; i++, j++) {
              input_stream >> vals[i];
              if (!input_stream) throw format_error;
            }
          }

          input_stream >> token;
          if (token != "endloop") throw format_error;
          input_stream >> token;
          if (token != "endfacet") throw format_error;

          facet_count++;
          output_stream.write(reinterpret_cast<char*>(vals), 48);
          output_stream.write(buf, 2);
        } else if (token == "endsolid") {
          break;
        } else {
          throw format_error;
        }
        std::cerr << facet_count << std::endl;
      }
      if (!input_stream) throw format_error;
      output_stream.seekp(80);
      output_stream.write(reinterpret_cast<char*>(&facet_count), 4);

      std::cerr << "Succesfully converted " <<  facet_count << " polygons";
      std::cerr << std::endl;
    } else {  // binary to ascii
      char title[81];
      input_stream.read(title, 80);
      title[80] = '\0';
      output_stream << "solid " << title << std::endl;

      input_stream.read(reinterpret_cast<char*>(&facet_count), 4);
      for (uint32_t k = 0; k < facet_count; k++) {
        float vals[12];
        input_stream.read(reinterpret_cast<char*>(vals), 48);
        input_stream.ignore(2);

        output_stream << "  facet normal";
        for (int i = 0; i < 3; i++) {
          output_stream << ' ' << vals[i];
        }
        output_stream << std::endl;
        output_stream << "    outer loop" << std::endl;
        for (int i = 3; i < 12; ) {
          output_stream << "      vertex";
          for (int j = 0; j < 3; i++, j++) {
            output_stream << ' ' << vals[i];
          }
          output_stream << std::endl;
        }
        output_stream << "    endloop" << std::endl;
        output_stream << "  endfacet" << std::endl;
      }

      output_stream << "endsolid " << title << std::endl;
      std::cerr << "Succesfully converted " <<  facet_count << " polygons";
      std::cerr << std::endl;
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    retcode = 1;
  }
  if (input_stream.is_open()) input_stream.close();
  if (output_stream.is_open()) output_stream.close();
  return retcode;
}
