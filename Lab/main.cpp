#include <iostream>
#include <string>
#include <algorithm>
#include "password.h"

using namespace std;

int main() {
//    string clear = "I love youdlafnalnf,anf,anf, a,nf,anf,anf,anf,angabgkajsdvfguavgmsvgfvuxkbfslgiusgfw em smfvjsvfzfmnvsjoqwujiuwherqwriuqtyoqakfhkvbgzv zv";
//    string clear = "uuuuuuu";
    string clear = "dadada";
    string cipher = encrypt_shift(clear, 10);
    attack_shift(cipher);
    return 0;
}
