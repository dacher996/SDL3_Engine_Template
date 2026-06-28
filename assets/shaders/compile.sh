# Requires shadercross CLI installed from SDL_shadercross

cd "$(dirname "$0")"

# Create directories
mkdir -p ./compiled/
mkdir -p ./compiled/{SPIRV,MSL,DXIL}

echo $(pwd)
cd ./sources

# Loop through all HLSL files that match the patterns
for filename in *.vert.hlsl *.frag.hlsl *.comp.hlsl; do
    if [ -f "$filename" ]; then
        shadercross "$filename" -o "../compiled/SPIRV/${filename/.hlsl/.spv}"
        shadercross "$filename" -o "../compiled/MSL/${filename/.hlsl/.msl}"
        shadercross "$filename" -o "../compiled//DXIL/${filename/.hlsl/.dxil}"
    fi
done

cd ..