#include "cribbage.h"

char suits[] = {'S', 'H', 'C', 'D'};
char numbers[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

char* card_str(int8_t card) {
    char* to_return = calloc(3, sizeof(char));
    to_return[0] = numbers[card & 0xF];
    to_return[1] = suits[card >> 4];
    return to_return;
}

char* hand_str(int8_t* hand) {
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

int8_t card_value(int8_t card) {
    int8_t number = card & 0xF;
    return number < 10 ? number + 1 : 10;
}

int8_t create_card(int8_t number, int8_t suit) {
    return (suit << 4) | number;
}

int8_t points_from_knobs(int8_t* hand) {
    for(int8_t i = 0; i < 4; i++) {
        if((hand[i] & 0xF) == 10 && (hand[i] >> 4) == (hand[4] >> 4)) {
            return 1;
        }
    }
    return 0;
}

int8_t points_from_flush(int8_t* hand) {
    int first_suit = hand[0] >> 4;
    for(int8_t i = 1; i < 4; i++) {
        if((hand[i] >> 4) != first_suit) {
            return 0;
        }
    }
    return hand[4] >> 4 == first_suit ? 5 : 4;
}

int8_t points_from_fifteens(int8_t* hand) {
    int8_t total = 0;

    // Choose 2 cards
    for(int8_t first = 0; first < 5 - 1; first++) {
        for(int8_t second = first + 1; second < 5; second++) {
            int8_t indices[] = {first, second};
            int8_t sum = 0;
            for(int8_t i = 0; i < 2; i++) {
                sum += card_value(hand[indices[i]]);
            }
            total += 2 * (sum == 15);
        }
    }

    // Choose 3 cards
    for(int8_t first = 0; first < 5 - 2; first++) {
        for(int8_t second = first + 1; second < 5 - 1; second++) {
            for(int8_t third = second + 1; third < 5; third++) {
                int8_t indices[] = {first, second, third};
                int8_t sum = 0;
                for(int8_t i = 0; i < 3; i++) {
                    sum += card_value(hand[indices[i]]);
                }
                total += 2 * (sum == 15);
            }
        }
    }

    // Choose 4 cards
    for(int8_t first = 0; first < 5 - 3; first++) {
        for(int8_t second = first + 1; second < 5 - 2; second++) {
            for(int8_t third = second + 1; third < 5 - 1; third++) {
                for(int8_t fourth = third + 1; fourth < 5; fourth++) {
                    int8_t indices[] = {first, second, third, fourth};
                    int8_t sum = 0;
                    for(int8_t i = 0; i < 4; i++) {
                        sum += card_value(hand[indices[i]]);
                    }
                    total += 2 * (sum == 15);
                }
            }
        }
    }

    // Choose 5 cards
    for(int8_t first = 0; first < 5 - 4; first++) {
        for(int8_t second = first + 1; second < 5 - 3; second++) {
            for(int8_t third = second + 1; third < 5 - 2; third++) {
                for(int8_t fourth = third + 1; fourth < 5 - 1; fourth++) {
                    for(int8_t fifth = fourth + 1; fifth < 5; fifth++) {
                        int8_t indices[] = {first, second, third, fourth, fifth};
                        int8_t sum = 0;
                        for(int8_t i = 0; i < 5; i++) {
                            sum += card_value(hand[indices[i]]);
                        }
                        total += 2 * (sum == 15);
                    }
                }
            }
        }
    }
    return total;
}

int8_t points_from_pairs(int8_t* hand) {
    int8_t total = 0;
    for(int8_t first = 0; first < 5 - 1; first++) {
        for(int8_t second = first + 1; second < 5; second++) {
            if((hand[first] & 0xF) == (hand[second] & 0xF)) {
                total += 2;
            }
        }
    }
    return total;
}

int8_t points_from_runs(int8_t* hand) {
    int8_t frequencies[13];
    for(int8_t i = 0; i < 13; i++) { frequencies[i] = 0; }

    for (int8_t i = 0; i < 13; i++) {
        frequencies[i] = 0;
    }

    int8_t n_unique_numbers = 0;
    for (int8_t i = 0; i < 5; i++) {
        int8_t curr_number = hand[i] & 0xF;
        n_unique_numbers += (frequencies[curr_number] == 0 ? 1 : 0);
        frequencies[curr_number]++;
    }

    int8_t numbers[n_unique_numbers];
    for(int8_t i = 0; i < n_unique_numbers; i++) { numbers[i] = 0; }

    int8_t currNumbersIndex = 0;
    for (int8_t i = 0; i < 13; i++) {
        if (frequencies[i] != 0) {
            numbers[currNumbersIndex++] = i;
        }
    }

    int8_t in_run[n_unique_numbers];
    for(int8_t i = 0; i < n_unique_numbers; i++) { in_run[i] = 0; }

    int8_t in_run_size = 0;
    int8_t curr_in_run[n_unique_numbers];
    for(int8_t i = 0; i < n_unique_numbers; i++) { curr_in_run[i] = 0; }

    int8_t curr_in_run_size = 0;
    for (int8_t i = 0; i < n_unique_numbers; i++) {
        in_run[i] = -1;
        curr_in_run[i] = -1;
    }

    int8_t max_run_length = 1;
    int8_t curr_run_length = 1;

    for (int8_t i = 0; i < n_unique_numbers - 1; i++) {
        if (curr_in_run_size == 0) {
            curr_in_run[curr_in_run_size++] = numbers[i];
        }

        if (numbers[i] + 1 == numbers[i + 1]) {
            curr_in_run[curr_in_run_size++] = numbers[i + 1];
            curr_run_length++;
            if (curr_run_length > max_run_length) {
                max_run_length = curr_run_length;
            }
        } else {
            if (curr_in_run_size >= 3) {
                for (int8_t c = 0; c < n_unique_numbers; c++) {
                    in_run[c] = curr_in_run[c];
                }
                in_run_size = curr_in_run_size;
                break;
            }
            for (int8_t c = 0; c < n_unique_numbers; c++) {
                curr_in_run[c] = -1;
            }
            curr_in_run_size = 0;
            curr_run_length = 1;
        }
    }
    if (curr_in_run_size >= 3 && in_run_size == 0) {
        for (int8_t c = 0; c < n_unique_numbers; c++) {
            in_run[c] = curr_in_run[c];
        }
        in_run_size = curr_in_run_size;
    }

    if (in_run[0] == -1) { return 0; }

    int8_t product = 1;
    for (int8_t i = 0; i < in_run_size; i++) {
        product *= frequencies[in_run[i]];
    }

    return product * in_run_size;
}

void print_score_report(int8_t* hand) {
    printf("FROM KNOBS:    %d\n", points_from_knobs(hand));
    printf("FROM FLUSH:    %d\n", points_from_flush(hand));
    printf("FROM FIFTEENS: %d\n", points_from_fifteens(hand));
    printf("FROM PAIRS:    %d\n", points_from_pairs(hand));
    printf("FROM RUNS:     %d\n", points_from_runs(hand));
}

int8_t score(int8_t* hand) {
    int8_t total = 0;
    total += points_from_knobs(hand);
    total += points_from_flush(hand);
    total += points_from_fifteens(hand);
    total += points_from_pairs(hand);
    total += points_from_runs(hand);
    return total;
}

int32_t* get_frequencies(int8_t verbose) {
    int8_t deck[52];

    int32_t* frequencies = malloc(30 * sizeof(int32_t));
    for(int8_t i = 0; i < 30; i++) {
        frequencies[i] = 0;
    }

    for(int8_t suit = 0, i = 0; suit < 4; suit++) {
        for(int8_t number = 0; number < 13; number++, i++) {
            deck[i] = create_card(number, suit);
        }
    }

    for(int8_t draw_index = 0; draw_index < 52; draw_index++) {
        if(verbose) {
            printf("%d/52\n", draw_index + 1);
            fflush(stdout);
        }
        for(int8_t first = 0; first < 52 - 3; first++) {
            if(first == draw_index) { continue; }
            for(int8_t second = first + 1; second < 52 - 2; second++) {
                if(second == draw_index) { continue; }
                for(int8_t third = second + 1; third < 52 - 1; third++) {
                    if(third == draw_index) { continue; }
                    for(int8_t fourth = third + 1; fourth < 52; fourth++) {
                        if(fourth == draw_index) { continue; }
                        int8_t hand[] = {deck[first], deck[second], deck[third], deck[fourth], deck[draw_index]};
                        frequencies[score(hand)]++;
                    }
                }
            }
        }
    }

    return frequencies;
}

int main() {
    int32_t* frequencies = get_frequencies(0);
    for(int8_t i = 0; i < 30; i++) {
        printf("Score %d:\t%d\n", i, frequencies[i]);
    }
    free(frequencies);
}
