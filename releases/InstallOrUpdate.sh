#! /bin/bash
BASE=/usr/share/cppknife
BASE_BIN=/usr/local/bin
BASE_TEMP=/tmp/knifeinstall
BINARY_FILES="dbknife fileknife geoknife sesknife textknife libcppknife.so"
DOWNLOAD_URL=https://github.com/seapluspro/seapluspro.github.io/tree/main/releases
DOWNLOAD_SUFFIX=?raw=true
ARCHITECTURE=amd64
if [ "$1" = "arm64" ]; then
  ARCHITECTURE=arm64
fi
TAR=cppknife.$ARCHITECTURE.tgz
function MkLink(){
  local name=$1
  if [ ! -l $BASE_BIN/$name ]; then
    ln -s $BASE/$name $BASE_BIN/$name
  fi
} 
function PrepareBase(){
  mkdir -p $BASE
  if [ "$0" = $BASE/InstallOrUpdate.sh ]; then
    echo "= installing script..."
    cp -a $0 $BASE/InstallOrUpdate.sh
    chmod +x BASE/InstallOrUpdate.sh
  fi
  for file in $BINS ; do
    MkLink $file
  done
}
function Update(){
  mkdir -p /tmp/cppknife
  cd /tmp/cppknife
  rm -f version.txt $BINARY_FILES
  wget $DOWNLOAD_URL/version.txt$DOWNLOAD_SUFFIX -o version.txt
  local version=$(cat version.txt)
  local versionOld=$(cat $BASE/version.txt)
  if [ $version = $versionOld ]; then
    echo "= already up to date: version: $version"
  else
    wget $DOWNLOAD_URL/$TAR$DOWNLOAD_SUFFIX -o $TAR
    tar xzf $TAR
  fi
  rsync -auv *knife *.so version.txt $BASE/ 
}

if [ "$(id -u)" != 0 ]; then
  echo "++ Be root"
else
  PrepareBase
  Update
fi

