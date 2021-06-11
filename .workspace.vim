let g:ale_linters = {
			\'c':['gcc','cppcheck'],
			\'cpp':['g++','cppcheck']
		\}
let g:ale_c_cc_options = '-std=gnu99 -Wall -I /opt/ros/noetic/include'
let g:ale_cpp_cc_options = '-Wall -std=c++11 -I /opt/ros/noetic/include'
