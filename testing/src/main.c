#include <stdio.h>
#include <string.h>

#include <cdat/linked-list.h>

int main(){
    ll_test();

    char *x = "testing";
    char *y = "hello there mister how are you I am really good thankyou for asking";
    uint32_t h = hash_digit_fold(x, strlen(x), 50000);
    printf("%i\n", h);
    h = hash_digit_fold(y, strlen(y), 100000000);
    printf("%i\n", h);
}
