DOCKER_ID=$(ssh kurt@selkify.de "echo \$(sudo docker load -q < docker_image | cut -c 25-)")
echo $DOCKER_ID
ssh -Y selkify.de "sudo docker run --rm --net=host --volume ~/.Xauthority:/root/.Xauthority --volume /tmp/.X11-unix:/tmp/.X11-unix --env DISPLAY=\$DISPLAY $DOCKER_ID"