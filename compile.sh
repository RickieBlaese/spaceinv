clang++ -o spaceinv src/spaceinv.cc src/models.cc src/utility.cc $(ncursesw5-config --cflags --libs) -g -std=c++20
