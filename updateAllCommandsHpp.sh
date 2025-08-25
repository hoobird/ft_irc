#!/bin/bash

output_file="includes/AllCommands.hpp"
commands_dir="includes/commands"

echo "#ifndef ALLCOMMANDS_HPP" > "$output_file"
echo "#define ALLCOMMANDS_HPP" >> "$output_file"
echo "" >> "$output_file"

for hpp in "$commands_dir"/*.hpp; do
    filename=$(basename "$hpp")
    echo "#include \"commands/$filename\"" >> "$output_file"
done

echo "" >> "$output_file"
echo "#endif // ALLCOMMANDS_HPP" >> "$output_file"
echo "// this file is auto generated when you run make"