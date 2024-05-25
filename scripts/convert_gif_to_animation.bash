echo "Converting $1"

filename=$(basename "$1")
filename_without_extension="${filename%.*}"
echo "Create Animation: $filename_without_extension"

mkdir -p "$(dirname "$1")/$filename_without_extension"

cd "$(dirname "$1")/$filename_without_extension"
mkdir tmp

flatpak run com.github.libresprite.LibreSprite -b "../$filename" --save-as tmp/frame1.png
# convert "../$filename" -alpha on tmp/$filename_without_extension%02d.png

montage -background none "tmp/*.png" -tile x1 -geometry +0+0 spritesheet.png
identify -format "%T " "../$filename" > timing.txt
> "width.txt"
for image in "tmp/"*.png; do
    width=$(identify -format "%w" "$image")
    echo -n "$width " >> "width.txt"
done
# identify -format "%w " "../$filename" > width.txt
rm -rf tmp
