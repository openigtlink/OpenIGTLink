#! /usr/bin/env bash

# Bash script to release a new revision of OpenIGTLink library.
# Run the command without arugment for the usage.

GIT_URL='https://github.com/tokjun/OpenIGTLink.git'

# Major, minor, and patch version numbers
MAJ=0
MIN=0
PAT=0

printUsage() {
    echo ''
    echo 'Usage: Utilities/OpenIGTLinkReleaseScript.sh <RELEASE_TYPE>'
    echo ''
    echo '  RELEASE_TYPE: Type of release' 
    echo '      major : Major version release'
    echo '      minor : Minor version release'
    echo '      patch : Patch release'
}

# Extract OpenIGTLink protocol version numbers from the specified CMake file
getVersionNumbers() {

    local FILE=$1

    echo "Examining the existing version numbers in $FILE..."
    MAJ=`sed -rn 's/(\s*SET\(OpenIGTLink_VERSION_MAJOR\s+\")([0-9]+)(.*)/\2/p' $FILE`
    MIN=`sed -rn 's/(\s*SET\(OpenIGTLink_VERSION_MINOR\s+\")([0-9]+)(.*)/\2/p' $FILE`
    PAT=`sed -rn 's/(\s*SET\(OpenIGTLink_VERSION_PATCH\s+\")([0-9]+)(.*)/\2/p' $FILE`

    echo "Old version: $MAJ.$MIN.$PAT"

}

# Update OpenIGTLink protocol version numbers in the specified CMake file
updateVersionNumbers() {
    
    local FILE=$1

    echo "New version: $MAJ.$MIN.$PAT"

    cat $FILE \
	| sed -r 's/(\s*SET\(OpenIGTLink_VERSION_MAJOR\s+\")([0-9]+)(.*)/\1'"$MAJ"'\3/g' \
	| sed -r 's/(\s*SET\(OpenIGTLink_VERSION_MINOR\s+\")([0-9]+)(.*)/\1'"$MIN"'\3/g' \
	| sed -r 's/(\s*SET\(OpenIGTLink_VERSION_PATCH\s+\")([0-9]+)(.*)/\1'"$PAT"'\3/g' > $FILE.back
    mv $FILE.back $FILE

}


# Validate the number of arguments
if [ "$#" -ne 1 ]; then
    printUsage
    exit 1
fi

RELEASE_TYPE=$1

getVersionNumbers CMakeLists.txt

if [ $RELEASE_TYPE == "major" ]; then
    MAJ=$(($MAJ+1))
    MIN=0
    PAT=0
elif [ $RELEASE_TYPE == "minor" ]; then
    MIN=$(($MIN+1))
    PAT=0
elif [ $RELEASE_TYPE == "patch" ]; then
    PAT=$(($PAT+1))
else
    echo "ERROR: Invaid release type: $TYPE"
    exit 1
fi

BRANCH_NAME="release-$MAJ.$MIN"
TAG_NAME="v$MAJ.$MIN.$PAT"
# Following line require git 1.6.3 or later
CURRENT_BRANCH=`git rev-parse --abbrev-ref HEAD`

## Major release must be performed under the master branch
if [ $RELEASE_TYPE == "major" ]]; then
    if [ $CURRENT_BRANCH != "master" ]]; then
	echo "ERROR: Cannot perform major release from non-master branch."
	exit 1
    fi
fi

updateVersionNumbers CMakeLists.txt

# If the current branch is not master, switch to the appropriate release branch
if [ $CURRENT_BRANCH != "master" ]]; then
    git checkout -B $BRANCH_NAME
    CURRENT_BRANCH=$BRANCH_NAME
fi

# Commit the updated CMakeFileLists.txt to the current branch
git commit -a -m "Update version number in CMakeLists.txt to $MAJ.$MIN.$PAT."
git push $GIT_URL $CURRENT_BRANCH

# Change to the release branch, if the current branch is master
git checkout -B $BRANCH_NAME

# Add release tag
git tag -a $TAG_NAME -m "Relase $MAJ.$MIN.$PAT"    
git push $GIT_URL $CURRENT_BRANCH

	




