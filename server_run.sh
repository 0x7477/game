echo "a"
DOCKER_ID=$(ssh kurt@selkify.de "echo \$(sudo docker load -q < docker_image | cut -c 25-)")
echo "b"
echo $DOCKER_ID
echo "c"
ssh -Y selkify.de "sudo docker run --rm --net=host --volume ~/.Xauthority:/root/.Xauthority --volume /tmp/.X11-unix:/tmp/.X11-unix --env DISPLAY=\$DISPLAY $DOCKER_ID"