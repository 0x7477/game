unit=$1
echo "Downloading $unit"

dir="/home/kurt/Documents/TUB/game/resources/images/units/$unit/"
mkdir -p "$dir"

mkdir -p "$dir/1/"
mkdir -p "$dir/2/"


# echo "https://awbw.amarriner.com/terrain/aw2/movement/gs/gs$(echo $unit)_mside.gif"

curl -o "$dir/1/idle.gif" "https://awbw.amarriner.com/terrain/ani/tg$unit.gif"
curl -o "$dir/1/right.gif" "https://awbw.amarriner.com/terrain/aw2/movement/tg/tg$(echo $unit)_mside.gif"
curl -o "$dir/1/up.gif" "https://awbw.amarriner.com/terrain/aw2/movement/tg/tg$(echo $unit)_mup.gif"
curl -o "$dir/1/down.gif" "https://awbw.amarriner.com/terrain/aw2/movement/tg/tg$(echo $unit)_mdown.gif"
convert "$dir/1/right.gif" -flop "$dir/1/left.gif"


curl -o "$dir/2/idle.gif" "https://awbw.amarriner.com/terrain/ani/gs$unit.gif"
curl -o "$dir/2/right.gif" "https://awbw.amarriner.com/terrain/aw2/movement/gs/gs$(echo $unit)_mside.gif"
curl -o "$dir/2/up.gif" "https://awbw.amarriner.com/terrain/aw2/movement/gs/gs$(echo $unit)_mup.gif"
curl -o "$dir/2/down.gif" "https://awbw.amarriner.com/terrain/aw2/movement/gs/gs$(echo $unit)_mdown.gif"
convert "$dir/2/right.gif" -flop "$dir/2/left.gif"

/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/1/idle.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/1/up.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/1/down.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/1/left.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/1/right.gif"
#
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/2/idle.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/2/up.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/2/down.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/2/left.gif"
/home/kurt/Documents/TUB/game/scripts/convert_gif_to_animation.bash "$dir/2/right.gif"
#
