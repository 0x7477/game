echo "Converting $1"

filename=$(basename "$1")
filename_without_extension="${filename%.*}"
echo "Create Animation: $filename_without_extension"

mkdir -p "$(dirname "$1")/$filename_without_extension"

cd "$(dirname "$1")/$filename_without_extension"
mkdir tmp
convert "../$filename" -alpha on tmp/$filename_without_extension%02d.png

montage -background none "tmp/*.png" -tile x1 -geometry +0+0 sprite_sheet.png
identify -format "%T " "../$filename" > timing.txt
identify -format "%w " "../$filename" > width.txt
rm -rf tmp