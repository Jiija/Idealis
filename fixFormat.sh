find . -path "./external/*" -prune -o \( -name "*.cpp" -o -name "*.hpp" \) -print | xargs clang-format -i -style=file
