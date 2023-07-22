#! /bin/bash
SCRIPT=$(pwd)/InstallOrUpdate.sh
BASE_KNIFE=/usr/share/cppknife
BASE_BIN=/usr/local/bin
BASE_TEMP=/tmp/knifeinstall
BINARY_FILES="dbknife fileknife geoknife sesknife textknife libcppknife.so"
DOWNLOAD_URL=https://github.com/seapluspro/cppknife/blob/main/releases
DOWNLOAD_SUFFIX=?raw=true
ARCHITECTURE=amd64
if [ "$1" = "arm64" ]; then
  ARCHITECTURE=arm64
fi
TAR=cppknife.$ARCHITECTURE.tgz
function MkLink(){
  local name=$1
  if [ ! -l $BASE_BIN/$name ]; then
    ln -s $BASE_KNIFE/$name $BASE_BIN/$name
  fi
} 
function PrepareBase(){
  mkdir -p $BASE_KNIFE
  if [ "$0" = $BASE_KNIFE/InstallOrUpdate.sh ]; then
    echo "= installing script..."
    cp -a $0 $BASE_KNIFE/InstallOrUpdate.sh
    chmod +x BASE_KNIFE/InstallOrUpdate.sh
  fi
  for file in $BINS ; do
    MkLink $file
  done
}
function Update(){
  mkdir -p /tmp/cppknife
  cd /tmp/cppknife
  rm -f version.txt $BINARY_FILES
  wget $DOWNLOAD_URL/version.txt$DOWNLOAD_SUFFIX -O version.txt
  local version=$(cat version.txt)
  local versionOld=$(cat $BASE_KNIFE/version.txt)
  if [ "$version" = "$versionOld" ]; then
    echo "= already up to date: version: $version"
    READY=true
  else
    wget $DOWNLOAD_URL/$TAR$DOWNLOAD_SUFFIX -O $TAR
    tar xzf $TAR
    rsync -auv *knife *.so version.txt $BASE_KNIFE/ 
    chmod +x $SCRIPT
    cp -a $SCRIPT $BASE_KNIFE/
  fi
}

if [ "$(id -u)" != 0 ]; then
  echo "++ Be root"
else
  PrepareBase
  Update
fi

