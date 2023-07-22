#! /bin/bash
BASE=/usr/share/cppknife
BASE_BIN=/usr/local/bin
BINARY_FILES="dbknife fileknife geoknife sesknife textknife libcppknife.so"

function MkLink(){
  local name=$1
  if [ ! -l $BASE_BIN/$name ]; then
    ln -s $BASE/$name $BASE_BIN/$name
  fi
} 
function PrepareBase(){
  mkdir -p $BASE
  for file in $BINS ; do
    MkLink $file
  done
}
function Update(){
  mkdir -p /tmp/cppknife
  cd /tmp/cppknife
  rm -f version.txt $BINARY_FILES
  wget https://github.com/seapluspro/cppknife/tree/main/releases/amd64/version.txt
  local version=$(cat version.txt)
  if [ $version = $(cat $BASE/version.txt) ]; then
    echo "= already up to date: version: $version"
  else
    for file in $BINS; do
      wget https://github.com/seapluspro/cppknife/tree/main/releases/amd64/$file -o $file
      chmod uog+x $file
    done
  fi
  rsync -auv $BINARY_FILES version.txt $BASE/ 
}

if [ "$(id -u)" != 0 ]; then
  echo "++ Be root"
else
  PrepareBase
  Update
fi

