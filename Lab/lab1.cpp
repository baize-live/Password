#include "password.h"
#include <iostream>

int main() {
//    string clear = "I love three things in the world the sun the moon and you "
//                   "The sun for the day The moon for the night and you forever ";
    string clear = "yolo means that you only live once";

    // ÒÆÎ»ÃÜÂë
    printf("shift_pwd: \n");
    int shift_num = 5;
    printf("encrypt_shift: %s\n", encrypt_shift(clear, shift_num).c_str());
    printf("decrypt_shift: %s\n", decrypt_shift(encrypt_shift(clear, shift_num), shift_num).c_str());
    // ÒÆÎ»ÃÜÂë¹¥»÷
    printf("===================================\n");
    attack_shift("kaxa yqmze ftmf kag azxk xuhq azoq");

    // µ¥±íÖÃ»»
    printf("===================================\n");
    printf("singleTableReplace_pwd: \n");
    string key = "I love three things";
    printf("encrypt_singleTableReplace: %s\n", encrypt_singleTableReplace(key, clear).c_str());
    printf("decrypt_singleTableReplace: %s\n", decrypt_singleTableReplace(key, encrypt_singleTableReplace(key, clear)).c_str());

    // ÆµÂÊ¹¥»÷
    printf("===================================\n");
    cout << frequency_attack(
            "SIC GCBSPNA XPMHACQ JB GPYXSMEPNXIY JR SINS MF SPNBRQJSSJBE JBFMPQNSJMB FPMQ N XMJBS N SM N XMJBS H HY QCNBR MF N XMRRJHAY JBRCGZPC GINBBCA JB RZGI N VNY SINS SIC MPJEJBNA QCRRNEC GNB MBAY HC PCGMTCPCD HY SIC PJEISFZA PCGJXJCBSR SIC XNPSJGJXNBSR JB SIC SPNBRNGSJMB NPC NAJGC SIC MPJEJBNSMP MF SIC QCRRNEC HMH SIC PCGCJTCP NBD MRGNP N XMRRJHAC MXXMBCBS VIM VJRICR SM ENJB ZBNZSIMPJOCD GMBSPMA MF SIC QCRRNEC");

    return 0;
}
//
//    { a ==> l } { b ==> n } { c ==> e } { d ==> d } { e ==> g }
//    { f ==> f } { g ==> c } { h ==> b } { i ==> h } { j ==> i }
//    { k ==> j } { l ==> x } { m ==> o } { n ==> a } { o ==> z }
//    { p ==> r } { q ==> m } { r ==> s } { s ==> t } { t ==> v }
//    { u ==> q } { v ==> w } { w ==> z } { x ==> p } { y ==> y } { z ==> u }


