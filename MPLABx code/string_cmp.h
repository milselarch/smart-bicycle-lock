#ifndef STRINGCMP_H_INCLUDED
#define STRINGCMP_H_INCLUDED

int strcmp(char input[], char check[]) {
    for (int i = 0;; i++) {
        if (input[i] == '\0' && check[i] == '\0') { break; }
        else if (input[i] == '\0' && check[i] != '\0') { return 1; }
        else if (input[i] != '\0' && check[i] == '\0') { return -1; }
        else if (input[i] > check[i]) { return 1; }
        else if (input[i] < check[i]) { return -1; }
        else {}
    }
    return 0;
}

void clearArray(char *array, const unsigned short length) {
    for (int i = 0; i < length; i++) {
        array[i] = '\0';
    }
}

#endif