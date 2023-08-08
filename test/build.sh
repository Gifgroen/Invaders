
BASE_DIR="/Users/karstenwestra/Projects/Games/Test"

mkdir -p $BASE_DIR/build/
pushd ../build/
    g++ -o Game.out -Wall $BASE_DIR/src/main.cc
popd