#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <csse2310a1.h>

#define MAX_BUFFER_LENGTH 60
#define MAX_LETTERS_LENGTH 14
#define MAX_MIN_LENGTH 5
#define MIN_LENGTH 3
#define SCORE_BONUS 10

#define ERROR10_C 10
#define ERROR4_C 4
#define ERROR16_C 16
#define ERROR18_C 18
#define ERROR11_C 11
#define ERROR17_C 17
#define ERROR3_C 3

#define ALREADY_GUESSED_C 2
#define NOT_IN_DICT_C 1
#define NOT_IN_LETTER_SET_C 3

#define WELCOME_MSG                                                            \
    "Welcome to UQunscramble!\n"                                               \
    "Enter words of length %d to %d made from the letters \"%s\"\n"

#define ERROR10                                                                \
    "Usage: uqunscramble [--lett chars]"                                       \
    " [--min-length numletters] [--dict filename]\n"

#define ERROR4                                                                 \
    "uqunscramble: minimum length"                                             \
    " must be between 3 and 5\n"

#define ERROR16 "uqunscramble: letter set is invalid\n"

#define ERROR18 "uqunscramble: too many letters - at most 13 expected\n"

#define ERROR11                                                                \
    "uqunscramble: more letters required for the"                              \
    " given minimum length (%d)\n"

#define ERROR17                                                                \
    "uqunscramble: dictionary with name"                                       \
    " \"%s\" cannot be opened\n"

#define ERROR3 "No valid guesses\n"

#define ONLY_LETTERS "Your guess must contain only letters\n"

#define TOO_LONG "Word must be no more than %d characters long\n"

#define TOO_SHORT "Too short! At least %d characters are expected\n"

#define NOT_IN_LETTER_SET "Word can't be formed from available letters\n"

#define NOT_IN_DICT "Word can't be found in the dictionary file\n"

#define ALREADY_GUESSED "You've guessed that word before\n"

struct Dictionary {
    char** ptr;
    int length;
    int maxScore;
};

struct PrsArgv {
    char letters[MAX_BUFFER_LENGTH];
    FILE* filePtr;
    int minLength;
    int errorStatus;
};

bool word_only_contains_letter_set(char* word, char* letters);
bool string_is_alpha(char* word);
void intialise_dict(struct Dictionary* dict, struct PrsArgv* newArgv);
void free_dict(struct Dictionary* dict);
int validate_word(struct Dictionary* dict, char* word, char* letters);
int word_in_dict(char** dictPtr, char* word, int dictLength);
bool strcmp_cis(char* word1, char* word2);
void check_argv(int argc, char** argv, struct PrsArgv* newArgv);
bool string_is_digit(char* word);
void print_dict(struct Dictionary* dict);
int calculate_score(
        struct PrsArgv* newArgv, struct Dictionary* dict, char* userInput);

int main(int argc, char* argv[])
{
    // Initialise the random seed to whatever the current time elapsed
    // in seconds is
    srand(time(NULL));

    int errorStatus = 0;
    int minLength = MIN_LENGTH;
    int lettersLength = 0;
    unsigned int randomLen
            = rand() % (MAX_LETTERS_LENGTH - minLength) + minLength;

    char letters[MAX_LETTERS_LENGTH] = "";
    bool gameOver = false;
    struct Dictionary dict;
    struct PrsArgv newArgv = {"", NULL, MIN_LENGTH, 0};

    // Checking argv
    check_argv(argc, argv, &newArgv);
    errorStatus = newArgv.errorStatus;

    if (errorStatus != 0) {

        if (errorStatus == ERROR10_C) {
            fprintf(stderr, ERROR10);
        }
        gameOver = true;

    } else {

        if (strcmp(newArgv.letters, "") == 0) {
            // If the user hasn't used --lett, will generate a string of random
            // letters
            strcpy(letters, get_random_letters(randomLen));
            lettersLength = (int)strlen(letters);
        } else {
            // If the user has used --lett it will copy them into the letter
            // variable
            strcpy(letters, newArgv.letters);
            lettersLength = (int)strlen(letters);
        }

        // Intialising
        strcpy(newArgv.letters, letters);
        minLength = newArgv.minLength;
        intialise_dict(&dict, &newArgv);

        printf(WELCOME_MSG, minLength, lettersLength, letters);
    }

    char userInput[MAX_BUFFER_LENGTH];
    char* stdinPtr;
    bool stdinClosed;
    int userInputLength;
    int score = 0;
    int wordScore = 0;

    while (!gameOver) {
        // Get user input from stdin, copy it into userInput
        // Save the pointer returned by fgets to see if stidn has been closed
        stdinPtr = fgets(userInput, MAX_BUFFER_LENGTH, stdin);
        userInputLength = (int)strlen(userInput);
        stdinClosed = feof(stdin) && stdinPtr == NULL;

        if (strcmp(userInput, "q\n") == 0 || stdinClosed
                || score == dict.maxScore) {
            gameOver = true;

            if (strcmp(userInput, "q\n") == 0) {
                print_dict(&dict);
                printf("Maximum possible score is %d\n", dict.maxScore);
            }

            if (score == 0) {
                errorStatus = ERROR3_C;
                printf(ERROR3);
            } else {
                printf("Game over. Your final score is %d\n", score);
            }

            free_dict(&dict);

        } else {
            if (userInput[userInputLength - 1] == '\n') {
                // If userInput has a \n charater, replace it with \0
                userInput[userInputLength -= 1] = '\0';
            } else {
                // If the user input has not been terminated by a \n
                // Replace it with a \0
                userInput[userInputLength + 1] = '\0';
            }

            if ((wordScore = calculate_score(&newArgv, &dict, userInput))
                    != 0) {
                printf("OK! Current score is %d\n", score += wordScore);
            }
        }
    }

    return errorStatus;
}

// Checks to see if word is vaild, then calulates the score of that word
int calculate_score(
        struct PrsArgv* newArgv, struct Dictionary* dict, char* userInput)
{
    int score = 0;
    int lettersLength = (int)strlen(newArgv->letters);
    int userInputLength = (int)strlen(userInput);
    int status = validate_word(dict, userInput, newArgv->letters);

    if (!string_is_alpha(userInput)) {
        printf(ONLY_LETTERS);
    } else if (userInputLength < newArgv->minLength) {
        printf(TOO_SHORT, newArgv->minLength);
    } else if (userInputLength > lettersLength) {
        printf(TOO_LONG, lettersLength);
    } else if (status == NOT_IN_LETTER_SET_C) {
        printf(NOT_IN_LETTER_SET);
    } else if (status == NOT_IN_DICT_C) {
        printf(NOT_IN_DICT);
    } else if (status == ALREADY_GUESSED_C) {
        printf(ALREADY_GUESSED);
    } else {
        score = userInputLength
                + (userInputLength == lettersLength) * SCORE_BONUS;
    }

    return score;
}

void check_argv(int argc, char* argv[], struct PrsArgv* newArgv)
{
    // Assume that argv is correct
    int errorStatus = 0;
    char fileName[MAX_BUFFER_LENGTH] = "/local/courses/csse2310/etc/words";
    // Used to make sure user cant use the same argument twice
    bool usage[3] = {false, false, false};

    // As argc will always be >= 1, if an argumemt is supplied, information
    // must be supplied with it, hence argc must always be odd
    if (argc != 1 && argc % 2 != 0) {
        for (int i = 1; i < argc; i += 2) {
            // Validates --min-length option
            if (strcmp("--min-length", argv[i]) == 0 && !usage[0]
                    && string_is_digit(argv[i + 1])) {
                if (atoi(argv[i + 1]) > MAX_MIN_LENGTH
                        || atoi(argv[i + 1]) < MIN_LENGTH) {
                    errorStatus = ERROR4_C;
                    fprintf(stderr, ERROR4);
                    break;
                }
                newArgv->minLength = atoi(argv[i + 1]);
                usage[0] = true;
                // Validates --lett option
            } else if (strcmp("--lett", argv[i]) == 0 && !usage[1]) {
                if (!string_is_alpha(argv[i + 1])) {
                    errorStatus = ERROR16_C;
                    fprintf(stderr, ERROR16);
                    break;
                }
                if ((int)strlen(argv[i + 1]) > MAX_LETTERS_LENGTH) {
                    errorStatus = ERROR18_C;
                    fprintf(stderr, ERROR18);
                    break;
                }
                if ((int)strlen(argv[i + 1]) < newArgv->minLength) {
                    errorStatus = ERROR11_C;
                    fprintf(stderr, ERROR11, newArgv->minLength);
                    break;
                }
                strcpy(newArgv->letters, argv[i + 1]);
                usage[1] = true;
                // Validate --dict option
            } else if (strcmp("--dict", argv[i]) == 0 && !usage[2]) {
                strcpy(fileName, argv[i + 1]);
                usage[2] = true;
            } else {
                errorStatus = ERROR10_C;
                break;
            }
        }
        // Incorrect use of the argyments returns the usage error
    } else if (argc != 1) {
        errorStatus = ERROR10_C;
    }
    // If the --dict has been provide, it opens it to see if the file is vaild
    newArgv->filePtr = fopen(fileName, "r");

    if (newArgv->filePtr == NULL) {
        errorStatus = ERROR17_C;
        fprintf(stderr, ERROR17, fileName);
    }
    newArgv->errorStatus = errorStatus;
}

// Checks to see if word only conains the letters in the provided letter set
bool word_only_contains_letter_set(char* word, char* letters)
{
    // Assume word does contain only the letters in the letter set
    bool status = true;

    int lettersLength = (int)strlen(letters);
    int wordLength = (int)strlen(word);

    // Create a temporary letters pointer as it will be modified later on
    char lettersTemp[lettersLength];
    strcpy(lettersTemp, letters);

    if (lettersLength < wordLength) {
        // If the word is bigger than the letter set,
        // then the word doesnt conatin the letter set
        status = false;
    } else {
        int counter = 0;

        // If the letter set contains multiple of the samee letter, it sets
        // the letter used to the character '0' so that letter cannot be used
        // again as a letter set cannot conatin numbers
        for (int i = 0; i < wordLength; i++) {
            for (int j = 0; j < lettersLength; j++) {
                if (tolower(word[i]) == tolower(lettersTemp[j])) {
                    lettersTemp[j] = '0';
                    counter++;
                    break;
                }
            }
        }
        // If the the function has exieted early,
        // then counter will not be equal to the word length, and hence
        // word does not contain letter set
        if (counter != wordLength) {
            status = false;
        }
    }

    return status;
}

// Checks if string consists of only alphabet characters
bool string_is_alpha(char* word)
{
    for (int i = 0; i < (int)strlen(word); i++) {
        if (!isalpha(word[i])) {
            return false;
        }
    }
    return true;
}

// Checks if string consists of only digits
bool string_is_digit(char* word)
{
    for (int i = 0; i < (int)strlen(word); i++) {
        if (!isdigit(word[i])) {
            return false;
        }
    }
    return true;
}

// Initialises word dictionary
void intialise_dict(struct Dictionary* dict, struct PrsArgv* newArgv)
{
    char** dictPtr = malloc(sizeof(char*));

    char buffer[MAX_BUFFER_LENGTH];
    int wordCount = 0;
    int maxScore = 0;
    int bufferLength;
    int lettersLength = (int)strlen(newArgv->letters);

    while (fgets(buffer, MAX_BUFFER_LENGTH, newArgv->filePtr) != NULL) {
        // Replace the newline character with \0
        bufferLength = (int)strlen(buffer) - 1;
        buffer[bufferLength] = '\0';
        // Check to see if word is made from the letter set,
        // word does already exist in the dictionary
        // and word is greater than the minimum length
        if (word_only_contains_letter_set(buffer, newArgv->letters)
                && (bufferLength >= newArgv->minLength)
                && (word_in_dict(dictPtr, buffer, wordCount) == -1)) {
            // If the word is in the dictionary file and this is not the first
            // the first word of the file,
            // reallocate more space in the dict pointer
            if (wordCount != 0) {
                dictPtr = realloc(dictPtr, sizeof(char*) * (wordCount + 1));
            }
            // Allocate the correct memory for the word, including and extra
            // two spaces, one for a \0, and the other for a ! to check if
            // word has been used in the future
            dictPtr[wordCount] = malloc(sizeof(char) * (bufferLength + 2));
            strcpy(dictPtr[wordCount], buffer);

            // Convert the word to upper case
            for (int i = 0; i < bufferLength; i++) {
                dictPtr[wordCount][i] = toupper(dictPtr[wordCount][i]);
            }

            // Calculate the score for that word
            maxScore += (bufferLength
                    + SCORE_BONUS * (bufferLength == lettersLength));
            wordCount++;
        }

        memset(buffer, '\0', MAX_BUFFER_LENGTH * sizeof(char));
    }
    // Close the file pointer and add the necessary data
    // into the dict struct pointer
    fclose(newArgv->filePtr);
    dict->ptr = dictPtr;
    dict->length = wordCount;
    dict->maxScore = maxScore;
}

// Free the allocated memeory for the dictionary
void free_dict(struct Dictionary* dict)
{
    for (int i = 0; i < dict->length; i++) {
        free(dict->ptr[i]);
    }

    free(dict->ptr);
}

// Checks to see if the word is in the dictionary, and can be used
int word_in_dict(char** dictPtr, char* word, int dictLength)
{
    // Assumes word is not in the dictionary
    int wordIndex = -1;

    // If dict length is equal to 0, then there is no point checking as the
    // word will not exist in the dictionary
    if (dictLength > 0) {
        for (int i = 0; i < dictLength; i++) {
            if (strcmp_cis(dictPtr[i], word)) {
                // If word is in the dictyioanry,
                // saves what index it is located at
                wordIndex = i;
                break;
            }
        }
    }

    return wordIndex;
}

// Checks to see if word is vaild (not used before, made of letterset)
int validate_word(struct Dictionary* dict, char* word, char* letters)
{
    // Assumes word is valid
    int status = 0;
    bool wcolBool = word_only_contains_letter_set(word, letters);

    int wordIndex = word_in_dict(dict->ptr, word, dict->length);
    int dictWordLength;

    if (wordIndex != -1) {
        dictWordLength = (int)strlen(dict->ptr[wordIndex]);

        // If the word is in the dictionary, check to see if has been
        // used by seeing if the last character is equal to !
        if (dict->ptr[wordIndex][dictWordLength + 1] != '!') {
            // If it is not equal to !, then word has not been used,
            // and hence change it so the word cannot be used again
            dict->ptr[wordIndex][dictWordLength + 1] = '!';
        } else {
            // Word has been used
            status = ALREADY_GUESSED_C;
        }
    } else {
        if (wcolBool) {
            // Word doesn't contain letter set
            status = NOT_IN_DICT_C;
        } else {
            // Word is not in the dictionary file
            status = NOT_IN_LETTER_SET_C;
        }
    }
    return status;
}

// Same as strcmp, excpet it returs an boolean, and char case deont matter
bool strcmp_cis(char* word1, char* word2)
{
    // Assumes the words are equal
    bool equal = true;

    int word1Length = (int)strlen(word1);
    int word2Length = (int)strlen(word2);

    if (word1Length == word2Length) {
        for (int i = 0; i < word1Length; i++) {
            if (tolower(word1[i]) != tolower(word2[i])) {
                equal = false;
                break;
            }
        }

    } else {
        equal = false;
    }

    return equal;
}

// Prints the dict in word size then alphabetical order, by calculating
// how many words, the curent word is bigger than, and how many words of the
// same size it is higher up in alphabetical order, then uses both those values
// to calculate its index in a temporary dict pointer
void print_dict(struct Dictionary* dict)
{
    char wordi[MAX_BUFFER_LENGTH] = "";
    char wordj[MAX_BUFFER_LENGTH] = "";

    // Creates a temporary dictionary on the stack
    char dictTemp[dict->length][MAX_BUFFER_LENGTH];
    int wordLengthi = 0;
    int wordLengthj = 0;

    for (int i = 0; i < dict->length; i++) {
        strcpy(wordi, dict->ptr[i]);
        wordLengthi = (int)strlen(wordi);

        // counta stores how many words the current word is more 'alphabetical'
        // than words of the same size is
        //
        // countb stores how many words the current word is bigger than
        int countb = 0;
        int counta = 0;

        for (int j = 0; j < dict->length; j++) {
            strcpy(wordj, dict->ptr[j]);
            wordLengthj = (int)strlen(wordj);

            if (wordLengthi > wordLengthj) {
                countb += 1;
            } else if (wordLengthi == wordLengthj) {
                for (int k = 0; k < wordLengthj; k++) {

                    if (wordi[k] < wordj[k]) {
                        break;
                    }

                    if (wordi[k] > wordj[k]) {
                        counta += 1;
                        break;
                    }
                }
            }
        }

        // counta + countb determins the index that word should be in dictTemp
        strcpy(dictTemp[counta + countb], wordi);
    }

    // Prints each word in dictTemp
    for (int i = 0; i < dict->length; i++) {
        printf("%s\n", dictTemp[i]);
    }
}
