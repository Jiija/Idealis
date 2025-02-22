find . -path "./excluded_folder" -prune -o -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i -style=file
