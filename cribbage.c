#include "cribbage.h"

char suits[] = {'S', 'H', 'C', 'D'};
char numbers[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

char* cardStr(int8_t card) {
    char* to_return = calloc(3, sizeof(char));
    to_return[0] = numbers[card & 0xF];
    to_return[1] = suits[card >> 4];
    return to_return;
}

char* handStr(int8_t* hand) {
    char* to_return = calloc(15, sizeof(char));
    for(int8_t i = 0; i < 5; i++) {
        to_return[i * 3] = numbers[hand[i] & 0xF];
        to_return[i * 3 + 1] = suits[hand[i] >> 4];
        if(i != 4) {
            to_return[i * 3 + 2] = ' ';
        }
    }
    return to_return;
}

int8_t cardValue(int8_t card) {
    int8_t number = card & 0xF;
    if(number < 10) {
        return number + 1;
    }
    return 10;
}

int8_t createCard(int8_t number, int8_t suit) {
    return (suit << 4) | number;
}

int8_t pointsFromKnobs(int8_t* hand) {
    for(int8_t i = 0; i < 4; i++) {
        if((hand[i] & 0xF) == 10 && (hand[i] >> 4) == (hand[4] >> 4)) {
            return 1;
        }
    }
    return 0;
}

int8_t pointsFromFlush(int8_t* hand) {
    int firstSuit = hand[0] >> 4;
    for(int8_t i = 1; i < 4; i++) {
        if((hand[i] >> 4) != firstSuit) {
            return 0;
        }
    }
    return (int8_t)(hand[4] >> 4 == firstSuit ? 5 : 4);
}

int8_t pointsFromFifteens(int8_t* hand) {
    int8_t total = 0;

    // Choose 2 cards
    for(int8_t first = 0; first < 5 - 1; first++) {
        for(int8_t second = (int8_t)(first + 1); second < 5; second++) {
            int8_t indices[] = {first, second};
            int8_t sum = 0;
            for(int8_t i = 0; i < 2; i++) {
                sum += cardValue(hand[indices[i]]);
            }
            total += 2 * (sum == 15);
        }
    }

    // Choose 3 cards
    for(int8_t first = 0; first < 5 - 2; first++) {
        for(int8_t second = (int8_t)(first + 1); second < 5 - 1; second++) {
            for(int8_t third = (int8_t)(second + 1); third < 5; third++) {
                int8_t indices[] = {first, second, third};
                int8_t sum = 0;
                for(int8_t i = 0; i < 3; i++) {
                    sum += cardValue(hand[indices[i]]);
                }
                total += 2 * (sum == 15);
            }
        }
    }

    // Choose 4 cards
    for(int8_t first = 0; first < 5 - 3; first++) {
        for(int8_t second = (int8_t)(first + 1); second < 5 - 2; second++) {
            for(int8_t third = (int8_t)(second + 1); third < 5 - 1; third++) {
                for(int8_t fourth = (int8_t)(third + 1); fourth < 5; fourth++) {
                    int8_t indices[] = {first, second, third, fourth};
                    int8_t sum = 0;
                    for(int8_t i = 0; i < 4; i++) {
                        sum += cardValue(hand[indices[i]]);
                    }
                    total += 2 * (sum == 15);
                }
            }
        }
    }

    // Choose 5 cards
    for(int8_t first = 0; first < 5 - 4; first++) {
        for(int8_t second = (int8_t)(first + 1); second < 5 - 3; second++) {
            for(int8_t third = (int8_t)(second + 1); third < 5 - 2; third++) {
                for(int8_t fourth = (int8_t)(third + 1); fourth < 5 - 1; fourth++) {
                    for(int8_t fifth = (int8_t)(fourth + 1); fifth < 5; fifth++) {
                        int8_t indices[] = {first, second, third, fourth, fifth};
                        int8_t sum = 0;
                        for(int8_t i = 0; i < 5; i++) {
                            sum += cardValue(hand[indices[i]]);
                        }
                        total += 2 * (sum == 15);
                    }
                }
            }
        }
    }
    return total;
}

int8_t pointsFromPairs(int8_t* hand) {
    int8_t total = 0;
    for(int8_t first = 0; first < 5 - 1; first++) {
        for(int8_t second = (int8_t)(first + 1); second < 5; second++) {
            if((hand[first] & 0xF) == (hand[second] & 0xF)) {
                total += 2;
            }
        }
    }
    return total;
}

int8_t pointsFromRuns(int8_t* hand) {
    int8_t frequencies[13];
    for(int8_t i = 0; i < 13; i++) { frequencies[i] = 0; }

    for (int8_t i = 0; i < 13; i++) {
        frequencies[i] = 0;
    }

    int8_t nUniqueNumbers = 0;
    for (int8_t i = 0; i < 5; i++) {
        int8_t currNumber = (int8_t)(hand[i] & 0xF);
        nUniqueNumbers += (frequencies[currNumber] == 0 ? 1 : 0);
        frequencies[currNumber]++;
    }

    int8_t numbers[nUniqueNumbers];
    for(int8_t i = 0; i < nUniqueNumbers; i++) { numbers[i] = 0; }

    int8_t currNumbersIndex = 0;
    for (int8_t i = 0; i < 13; i++) {
        if (frequencies[i] != 0) {
            numbers[currNumbersIndex++] = i;
        }
    }

    int8_t inRun[nUniqueNumbers];
    for(int8_t i = 0; i < nUniqueNumbers; i++) { inRun[i] = 0; }

    int8_t inRunSize = 0;
    int8_t currInRun[nUniqueNumbers];
    for(int8_t i = 0; i < nUniqueNumbers; i++) { currInRun[i] = 0; }

    int8_t currInRunSize = 0;
    for (int8_t i = 0; i < nUniqueNumbers; i++) {
        inRun[i] = -1;
        currInRun[i] = -1;
    }

    int8_t maxRunLength = 1;
    int8_t currRunLength = 1;

    for (int8_t i = 0; i < nUniqueNumbers - 1; i++) {
        if (currInRunSize == 0) {
            currInRun[currInRunSize++] = numbers[i];
        }

        if (numbers[i] + 1 == numbers[i + 1]) {
            currInRun[currInRunSize++] = numbers[i + 1];
            currRunLength++;
            if (currRunLength > maxRunLength) {
                maxRunLength = currRunLength;
            }
        } else {
            if (currInRunSize >= 3) {
                for (int8_t c = 0; c < nUniqueNumbers; c++) {
                    inRun[c] = currInRun[c];
                }
                inRunSize = currInRunSize;
                break;
            }
            for (int8_t c = 0; c < nUniqueNumbers; c++) {
                currInRun[c] = -1;
            }
            currInRunSize = 0;
            currRunLength = 1;
        }
    }
    if (currInRunSize >= 3 && inRunSize == 0) {
        for (int8_t c = 0; c < nUniqueNumbers; c++) {
            inRun[c] = currInRun[c];
        }
        inRunSize = currInRunSize;
    }

    if (inRun[0] == -1) { return 0; }

    int8_t product = 1;
    for (int8_t i = 0; i < inRunSize; i++) {
        product *= frequencies[inRun[i]];
    }

    return product * inRunSize;
}

void printScoreReport(int8_t* hand) {
    printf("FROM KNOBS:    %d\n", pointsFromKnobs(hand));
    printf("FROM FLUSH:    %d\n", pointsFromFlush(hand));
    printf("FROM FIFTEENS: %d\n", pointsFromFifteens(hand));
    printf("FROM PAIRS:    %d\n", pointsFromPairs(hand));
    printf("FROM RUNS:     %d\n", pointsFromRuns(hand));
}

int8_t score(int8_t* hand) {
    int8_t total = 0;
    total += pointsFromKnobs(hand);
    total += pointsFromFlush(hand);
    total += pointsFromFifteens(hand);
    total += pointsFromPairs(hand);
    total += pointsFromRuns(hand);
    return total;
}

int32_t* getFrequencies(int8_t verbose) {
    int8_t deck[52];

    int32_t* frequencies = malloc(30 * sizeof(int32_t));
    for(int8_t i = 0; i < 30; i++) {
        frequencies[i] = 0;
    }

    for(int8_t suit = 0, i = 0; suit < 4; suit++) {
        for(int8_t number = 0; number < 13; number++, i++) {
            deck[i] = createCard(number, suit);
        }
    }

    for(int8_t drawIndex = 0; drawIndex < 52; drawIndex++) {
        if(verbose) {
            printf("%d/52\n", drawIndex + 1);
            fflush(stdout);
        }
        for(int8_t first = 0; first < 52 - 3; first++) {
            if(first == drawIndex) { continue; }
            for(int8_t second = (int8_t)(first + 1); second < 52 - 2; second++) {
                if(second == drawIndex) { continue; }
                for(int8_t third = (int8_t)(second + 1); third < 52 - 1; third++) {
                    if(third == drawIndex) { continue; }
                    for(int8_t fourth = (int8_t)(third + 1); fourth < 52; fourth++) {
                        if(fourth == drawIndex) { continue; }
                        int8_t hand[] = {deck[first], deck[second], deck[third], deck[fourth], deck[drawIndex]};
                        frequencies[score(hand)]++;
                    }
                }
            }
        }
    }

    return frequencies;
}

int main() {
    int32_t* frequencies = getFrequencies(0);
    for(int8_t i = 0; i < 30; i++) {
        printf("Score %d:\t%d\n", i, frequencies[i]);
    }
    free(frequencies);
}
