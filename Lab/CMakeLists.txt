cmake_minimum_required(VERSION 3.23)
project(Lab)

set(CMAKE_CXX_STANDARD 11)

add_executable(Lab1 lab1.cpp password.h pwd_shift.cpp pwd_singleTableReplace.cpp)
add_executable(Lab2 lab2.cpp password.h pwd_DES.cpp)
add_executable(Lab3 lab3.cpp password.h pwd_AES.cpp)
add_executable(Lab4 lab4.cpp password.h pwd_RSA.cpp BigInt.cpp BigInt.h)
add_executable(Lab5 lab5.cpp password.h hash_MD5.cpp)
add_executable(Test test.cpp BigInt.cpp BigInt.h)
