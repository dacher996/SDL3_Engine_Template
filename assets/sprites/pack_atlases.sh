#!/bin/bash

cd "$(dirname "$0")"

rm -rf output
mkdir output

for atlas_dir in */; do
    # Remove trailing slash to get the atlas name
    atlas_name="${atlas_dir%/}"
    
    # Skip the output directory
    if [ "$atlas_name" == "output" ]; then
        continue
    fi

    # Prioritized list of subdirectories to search.
    # An empty string ("") represents the root of the atlas directory itself.
    search_dirs=("" "inhouse" "aseprite")
    
    found_data=false

    for sub_dir in "${search_dirs[@]}"; do
        # Construct target path based on whether it's root or a subdirectory
        if [ -z "$sub_dir" ]; then
            target_path="$atlas_dir"
        else
            target_path="$atlas_dir/$sub_dir"
        fi

        # Only proceed if it's the root directory or the subdirectory actually exists
        if [ -z "$sub_dir" ] || [ -d "$target_path" ]; then
            # First look for .bin file
            if [ -f "$target_path/$atlas_name.bin" ]; then
                cp "$target_path/$atlas_name.bin" "output/"
                echo "Packed atlas: $atlas_name (.bin)"
                found_data=true
                break # Stop searching lower priority directories once found
            # If no .bin, fallback to .png + .json pair
            elif [ -f "$target_path/$atlas_name.png" ] && [ -f "$target_path/$atlas_name.json" ]; then
                cp "$target_path/$atlas_name.png" "output/"
                cp "$target_path/$atlas_name.json" "output/"
                echo "Packed atlas: $atlas_name (.png+.json)"
                found_data=true
                break # Stop searching lower priority directories once found
            fi
        fi
    done

    if [ "$found_data" = false ]; then
        echo "Warning: No valid atlas data (.bin OR .png + .json) found for '$atlas_name' in any expected location."
    fi
done