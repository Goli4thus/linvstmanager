#!/bin/bash

# Purpose:
# - offer a menu for running a specific are all configured docker builds
# - aggregate build console output
# Hint:
# - script needs to be run with sudo (due to docker commands)

targets=(
    debian_buster
    fedora_31
    fedora_34
    opensuse_tumbleweed
    ubuntu_1804
    ubuntu_2004
)

CMD='docker-compose -f docker-compose__build_app.yml up --build --force-recreate'

# Ansi color code variables
red="\e[0;91m"
blue="\e[0;94m"
expand_bg="\e[K"
blue_bg="\e[0;104m${expand_bg}"
red_bg="\e[0;101m${expand_bg}"
green_bg="\e[0;102m${expand_bg}"
green="\e[0;92m"
white="\e[0;97m"
bold="\e[1m"
uline="\e[4m"
reset="\e[0m"


if [[ $EUID != 0 ]]; then
    echo "Script needs to be run as root (due to docker commands)."
    exit 0
fi

echo "0) ALL OF THEM"

for ((i = 0; i < ${#targets[@]}; ++i)); do 
    position=$(( $i + 1))
    echo "$position) ${targets[$i]}"
done

echo "--------------------"
echo "q) quit"
echo "--------------------"
echo "input: "
read sel
echo "===================="

if [[ $sel =~ [1-${#targets[@]}] ]]; then
    target=${targets[$sel-1]}
    echo "Building target: $target"
    cd $target
    $CMD
    cd ..
elif [[ $sel == 0 ]]; then
    echo "Building target: ALL"
    for ((i = 0; i < ${#targets[@]}; ++i)); do 
        progress=$(( $i + 1))/${#targets[@]}
        echo "  ($progress) Building sub-target: ${targets[$i]}"
    #for target in "${targets[@]}"; do 
        #echo "  Building sub-target: $target"
        cd ${targets[$i]}
        $CMD
        cd ..
    done
elif [[ $sel == q ]]; then
    echo "quit"
else
    echo "wrong input"
fi


# Aggregate build results
for filename in build_output/*; do
    container=$(echo $filename | sed 's/.*__host_\(.*\)\.txt/\1/g')
    if [[ $sel != 0 ]] && [[ $container != ${targets[$sel-1]} ]] ; then
        # If only a specfic target was built, then only show the associated result.
        continue
    fi
    echo \#\#\#\#\# $container \#\#\#\#\#
    grep "Qt version" ./$filename | sed 's/.*\(Qt version.*\) in.*/\1/g'
    retval=$(grep -c "\[100%\] Built target linvstmanager" ./$filename)
    if [[ $retval == 1 ]]; then
        echo -e "${green}BUILD SUCCESS${reset}"
    else
        echo -e "${red}BUILD FAILED${reset}"
    fi
    printf "\n"
done

