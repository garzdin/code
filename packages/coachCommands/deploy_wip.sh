#!/bin/bash
#
# JFEI 2015-03-10 creation
# JFEI 2016-03-28 change from old SVN to new GIT
#
# little script to deploy SVN WIP to a dedicated robot without having to 
# go through SVN

cd $TURTLEROOT



# construct list of robots to apply to
robots=""
files=""
while :; do
    # check if it is a number
    found=0
    for irobot in `seq 1 6`; do
        if [ "$1" = "$irobot" -o "$1" = "r$irobot" ]; then
            robots="$robots r$irobot"
            found=1
        fi
    done
    if [ "$found" = "1" ]; then
        shift
    else
        # check if it is a filename
        if [ -f "$1" ]; then
            files="$files $1"
            shift
        else
            break
        fi
    fi
done
if [ -z "$robots" ]; then
    # none specified = all
    robots="r1 r2 r3 r4 r5 r6"
fi

# not just falcons/code but also teamplayData
for repoDir in $TURTLEROOT /home/robocup/falcons/teamplayData ; do

    cd $repoDir
    # determine files to send
    if [ -z "$files" ]; then
        files2=`git status --porcelain | grep -E "^ M|^A |^AM" | awk '{print $2}'`
    else
        files2=$files
    fi

    # run the command
    for robot in $robots; do
        ping -W 5 -c 1 $robot >/dev/null 2>/dev/null
        if [ "$?" == "0" ]
        then
            echo "deploying to $robot"
            for f in $files2 ; do
                echo "  $f"
                scp $f $robot:$repoDir/$f
            done
        else
            echo "ERROR: could not reach $robot"
        fi
    done

done

echo
echo "do a build on robot if necessary"
echo "don't forget to cleanup!"


