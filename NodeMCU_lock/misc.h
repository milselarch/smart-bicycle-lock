#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

class misc {
public:
    static void clearArray(char *arr, const short arrLength) {
        for (int i = 0; i < arrLength; i++) {
            arr[i] = 0;
        }
    };

    static String charArrayToString(const char *charArray) {
        String answer = "";
        for (int i = 0; charArray[i] != '\0'; i++) {
            answer += charArray[i];
        }
    
        return answer;
    }

    static void printCharArray(char charArray[]) {
        Serial.print("[");

        for (int i = 0; charArray[i] != '\0'; i++) {
            Serial.print(int(charArray[i]));
            Serial.print(", ");
        }

        Serial.println("]");
    }

    static int strEquals(const char input[], const char check[]) {
        return (misc::cmp(input, check) == 0);
    }

    static int cmp(const char input[], const char check[]) {
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
    
    static int cmp(const char input[], String check) {
        return cmp(input, check.c_str());
    }
};

#endif


