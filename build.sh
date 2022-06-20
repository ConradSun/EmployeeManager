#！/bin/zsh

rm -rf "./bin"
cd "./lib" && make
cd "../src" && make
