#！/bin/zsh

cov_dir
rm -rf "./bin"
cd "./test" && make
cd "../bin" && ./test

rm -rf "./code_cov_report" && mkdir "./code_cov_report"
cd "./code_cov_report"
lcov -b ../../test -d ../ -c -o code_cov.info
lcov -e ./code_cov.info '*/employee_manager/lib*' '*/employee_manager/src/*' -o ./code_cov.info
genhtml -o code_cov_html code_cov.info
