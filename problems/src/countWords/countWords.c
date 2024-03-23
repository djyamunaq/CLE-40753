/*
 * Compilation command: gcc -Wall -O3 -o countWords countWords.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define UNDEF_CHAR '@'

void printBinary(unsigned char c);
char normChar(char buffer[], int numBytes);
int isWhiteSpace(char buffer[]);
int isSeparationSymbol(char buffer[]);
int isPunctuationSymbol(char buffer[]);
int isMergeSymbol(char buffer[]);
int getFileMaxWordSize(char fileName[]);
char getBaseInstance(char buffer[]);
char toLowerCase(char c);
int repeatConsonant(char word[]);
int isVowel(char c);
int isLetter(char c);

int main(int argc, char* argv[]) { 
    /* Check if there are arguments in command line */
    if(argc < 2) {
        /* TODO: add usage message */

        printf("No files to read\n");
        exit(0);
    }

    /* Open and read each file */
    FILE *filePtr;
    for(int i=1; i<argc; i++) {
        // printf("=======================\n");

        /* Get max word size */
        int maxWordSize = getFileMaxWordSize(argv[i]);

        if(maxWordSize == -1) {
            printf("Could not read max size of word in file %s\n", argv[i]);
            continue;
        }

        /* Alloc memory for word */
        char *wordBuffer = (char*) malloc((maxWordSize+1)*sizeof(char));

        /* Open file */
        filePtr = fopen(argv[i], "r");

        /* File read SUCCESSFUL */
        if(filePtr != NULL) {
            printf("File name: %s\n", argv[i]);
            
            int wordCount, sameConstantWordCounter;
            wordCount = sameConstantWordCounter = 0;

            char c, utf8CharBuffer[4];
            unsigned int numBytes, charCount;
            numBytes = charCount = 0;

            while((c=fgetc(filePtr)) != EOF) {
                // printf("%c\t", c);
                // printBinary(c);
                // printf("\t");
                // continue;

                /* Clear utf8CharBuffer */
                memset(utf8CharBuffer, 0, sizeof(utf8CharBuffer));

                /* 4-byte sequence */
                if ((unsigned char) c >= 0xF0) { 
                    // printf("4-byte sequence\n");
                    /* Following bytes */
                    numBytes = 3; 
                    utf8CharBuffer[0] = c;
                } 
                /* 3-byte sequence */
                else if ((unsigned char) c >= 0xE0) { 
                    // printf("3-byte sequence\n");
                    numBytes = 2;
                    utf8CharBuffer[0] = c;
                } 
                /* 2-byte sequence */
                else if ((unsigned char) c >= 0xC0) { 
                    // printf("2-byte sequence\n");
                    numBytes = 1;
                    utf8CharBuffer[0] = c;
                } 
                /* Continuation byte or invalid (should not occur as first byte) */
                else if ((unsigned char) c >= 0x80) { 
                    /* TODO: Error message */
                    printf("Invalid UTF-8 sequence.\n");
                    exit(1);
                } 
                /* ASCII character */
                else { 
                    // printf("ASCII\n");
                    numBytes = 0;
                    utf8CharBuffer[0] = c;
                }

                // printf("%c", c);
                /* Read remaining bytes */
                for (int i=0; i<numBytes; i++) {
                    c = fgetc(filePtr);
                    /* Check for proper continuation byte */
                    if (c == EOF || (c & 0xC0) != 0x80) { 
                        break;
                    }
                    // printf("%c", c);
                    utf8CharBuffer[i+1] = c;
                }

                // printf("%s\t%d\t", utf8CharBuffer, strlen(utf8CharBuffer));
                
                if(isMergeSymbol(utf8CharBuffer)) {
                    // printf("0\n");
                    continue;
                }

                /* Normalize char from utf8 to ASCII / to lower case */
                char normalizedChar = normChar(utf8CharBuffer, numBytes);
                // printf("%c\n", normalizedChar);
                
                /* Consume spaces */
                if(normalizedChar == ' ') {
                    /* Process word */
                    if(charCount) {
                        // printf("%c", normalizedChar);
                        wordCount++;
                        wordBuffer[charCount] = '\0';
                        
                        charCount = 0;

                        // printf("%s\n", wordBuffer);

                        if(repeatConsonant(wordBuffer)) {
                            sameConstantWordCounter++;
                        }

                        wordBuffer[0] = '\0';
                    } 
                    /* Discard empty spaces */
                    else {
                        charCount = 0;
                        continue;
                    }
                } 
                /* Discard undefined char */
                else if(normalizedChar == UNDEF_CHAR) {
                    continue;
                }
                /* Add char to word */
                else {
                    // printf("%c", normalizedChar);
                    wordBuffer[charCount++] = normalizedChar;
                }

                // printf("%s", buffer);
            }

            /* Print word and repeat consonant counter */
            printf("Total number of words = %d\n", wordCount);
            printf("Total number of words with at least two instances of the same consonant = %d\n\n", sameConstantWordCounter);

            /* Close file */
            fclose(filePtr);
        } 
        /* File read ERROR */
        else {
            printf("ERROR READING FILE: %s\n", argv[i]);
            continue;
        }
    }

    return 0;
}

/**
 * \brief Print binary of a char
 */
void printBinary(unsigned char value) {
    for (int i = 7; i >= 0; i--) {
        putchar((value & (1 << i)) ? '1' : '0');
    }
}

/**
 * \brief transform utf-8 character to lowercase ASCII character
 * 
 * \param buffer utf-8 character represented as array of chars
 * \param numBytes number of bytes in utf-8 representation
 * 
 * \return character in ASCII lowercase as char
 */
char normChar(char buffer[], int numBytes) {
    if(isWhiteSpace(buffer) || isSeparationSymbol(buffer) || isPunctuationSymbol(buffer) ) {
        return ' ';
    }

    char baseInstance = getBaseInstance(buffer);
    // printf("%04X\n", baseInstance);

    if(baseInstance != UNDEF_CHAR) {
        char lowerCaseChar = toLowerCase(baseInstance);

        if(lowerCaseChar != UNDEF_CHAR) {
            return lowerCaseChar;
        }
    }
    // printf("%04X\n", lowerCaseChar);

    return UNDEF_CHAR;
}

/**
 * \brief Get base instance from utf-8 character
 *  
 * \param buffer utf-8 character represented as array of chars
 * 
 * \return first element of buffer if it's already in ASCII OR base instance of
 * char if it has a sign OR default undefined char if character is not ASCII nor 
 * predicted as signed character 
 */
char getBaseInstance(char buffer[]) {
    /* Char in ASCII */
    if(strlen(buffer) == 1) {
        return buffer[0];
    }

    /* Base instante: a */
    char *aInstance[] = {"\x61", "\xc3\xa1", "\xc3\xa0", "\xc3\xa2", "\xc3\xa3"};
    for(int i=0; i<5; i++) {
        if(strcmp(buffer, aInstance[i]) == 0) return 'a';
        // printf("%s\n", aInstance[i]);
    }
    /* Base instante: A */
    char *AInstance[] = {"\x41", "\xc3\x81", "\xc3\x80", "\xc3\x82", "\xc3\x83"};
    for(int i=0; i<5; i++) {
        if(strcmp(buffer, AInstance[i]) == 0) return 'A';
        // printf("%s\n", AInstance[i]);
    }
    /* Base instante: e */
    char *eInstance[] = {"\x65", "\xc3\xa9", "\xc3\xa8", "\xc3\xaa"};
    for(int i=0; i<4; i++) {
        if(strcmp(buffer, eInstance[i]) == 0) return 'e';
        // printf("%s\n", eInstance[i]);
    }
    /* Base instante: E */
    char *EInstance[] = {"\x45", "\xc3\x89", "\xc3\x88", "\xc3\x8a"};
    for(int i=0; i<4; i++) {
        if(strcmp(buffer, EInstance[i]) == 0) return 'E';
        // printf("%s\n", EInstance[i]);
    }
    /* Base instante: i */
    char *iInstance[] = {"\x69", "\xc3\xad", "\xc3\xac"};
    for(int i=0; i<3; i++) {
        if(strcmp(buffer, iInstance[i]) == 0) return 'i';
        // printf("%s\n", iInstance[i]);
    }
    /* Base instante: I */
    char *IInstance[] = {"\x49", "\xc3\x8d", "\xc3\x8c"};
    for(int i=0; i<3; i++) {
        if(strcmp(buffer, IInstance[i]) == 0) return 'I';
        // printf("%s\n", IInstance[i]);
    }
    /* Base instante: o */
    char *oInstance[] = {"\x6f", "\xc3\xb3", "\xc3\xb2", "\xc3\xb4", "\xc3\xb5"};
    for(int i=0; i<5; i++) {
        if(strcmp(buffer, oInstance[i]) == 0) return 'o';
        // printf("%s\n", oInstance[i]);
    }
    /* Base instante: O */
    char *OInstance[] = {"\x4f", "\xc3\x93", "\xc3\x92", "\xc3\x94", "\xc3\x95"};
    for(int i=0; i<5; i++) {
        if(strcmp(buffer, OInstance[i]) == 0) return 'O';
        // printf("%s\n", OInstance[i]);
    }
    /* Base instante: u */
    char *uInstance[] = {"\x75", "\xc3\xba", "\xc3\xb9"};
    for(int i=0; i<3; i++) {
        if(strcmp(buffer, uInstance[i]) == 0) return 'u';
        // printf("%s\n", uInstance[i]);
    }
    /* Base instante: U */
    char *UInstance[] = {"\x55", "\xc3\x9a", "\xc3\x99"};
    for(int i=0; i<3; i++) {
        if(strcmp(buffer, UInstance[i]) == 0) return 'U';
        // printf("%s\n", UInstance[i]);
    }
    /* Base instante: c */
    char *cInstance[] = {"\x63", "\xc3\xa7"};
    for(int i=0; i<2; i++) {
        if(strcmp(buffer, cInstance[i]) == 0) return 'c';
        // printf("%s\n", cInstance[i]);
    }
    /* Base instante: C */
    char *CInstance[] = {"\x43", "\xc3\x87"};
    for(int i=0; i<2; i++) {
        if(strcmp(buffer, CInstance[i]) == 0) return 'C';
        // printf("%s\n", CInstance[i]);
    }

    /* Unexpected char */
    return UNDEF_CHAR;
}

/**
 * \brief transform character from uppercase to lowercase
 * 
 * \param c character in ASCII as char
 * 
 * \return lowercase character if the character is alphabetic OR the same
 * character if it is numeric or _ OR default undefined char otherwise
 */
char toLowerCase(char c) {
    /* Uppercase letters */
    if(c >= 65 && c <= 90) {
        return (char) (c + 32);
    } 
    /* Lowercase letters */
    if(c >= 97 && c <= 122) {
        return c;
    } 
    /* Numbers */
    if(c >= '0' && c <= '9') {
        return c;
    } 
    /* Underscore */
    if(c == '\x5f') {
        return c;
    }

    return UNDEF_CHAR;
}

/**
 * \brief check if a word has repeated consonants
 * 
 * \param word word represented as an array of char
 * 
 * \return TRUE if the word has repeated consonants, FALSE otherwise
 */
int repeatConsonant(char word[]) {
    char letters[26];
    /* Fill with zeros */
    memset(letters, 0, sizeof(letters)*sizeof(char));

    for(int i=0; i<strlen(word); i++) {
        char c = word[i];

        if(isLetter(c) && !isVowel(c)) {
            int index = (int) c - 97;
            
            if(letters[index] > 0) {
                return TRUE;
            }

            letters[index]++;
        }
    }

    return FALSE;
}

/**
 * \brief check if a given character is a letter
 * 
 * \param c character to be verified
 * 
 * \return TRUE if it's a letter, FALSE otherwise
 */
int isLetter(char c) {
    if(c >= 97 && c <= 122) {
        return TRUE;
    }

    return FALSE;
}

/**
 * \brief check if a given character is a vowel
 * 
 * \param c character to be verified
 * 
 * \return TRUE if it's a vowel, FALSE otherwise
 */
int isVowel(char c) {
    if(c == 'a' || c == 'e' ||  c == 'i' ||  c == 'o' ||  c == 'u') {
        return TRUE;
    }

    return FALSE;
}

/**
 * \brief check if a given utf-8 character is a whitespace
 * 
 * \param buffer utf-8 character to be verified as an array of chars
 * 
 * \return TRUE if it's a whitespace, FALSE otherwise
 */
int isWhiteSpace(char buffer[]) {
    /* Space character */
    char c1[] = "\x20";
    if(strcmp(buffer, c1) == 0) return TRUE;

    /* Tab character */
    char c2[] = "\x9";
    if(strcmp(buffer, c2) == 0) return TRUE;

    /* Newline character */
    char c3[] = "\xA";
    if(strcmp(buffer, c3) == 0) return TRUE;

    /* Carriage return character */
    char c4[] = "\xD";
    if(strcmp(buffer, c4) == 0) return TRUE;

    return FALSE;
}

/**
 * \brief check if a given character is a separation symbol
 * 
 * \param buffer utf-8 character to be verified as an array of chars
 * 
 * \return TRUE if it's a separation symbol, FALSE otherwise
 */
int isSeparationSymbol(char buffer[]) {
    /* Hyphen */
    char c1[] = "-";
    if(strcmp(buffer, c1) == 0) return TRUE;

    /* Double quotation mark case 1 */
    char c2[] = "\x22";
    if(strcmp(buffer, c2) == 0) return TRUE;

    /* Double quotation mark case 2 */
    char c3[] = "\xE2\x80\x9C";
    if(strcmp(buffer, c3) == 0) return TRUE;

    /* Double quotation mark case 3 */
    char c4[] = "\xe2\x80\x9D";
    if(strcmp(buffer, c4) == 0) return TRUE;

    /* Opening brackets */
    char c5[] = "[";
    if(strcmp(buffer, c5) == 0) return TRUE;

    /* Closing brackets */
    char c6[] = "]";
    if(strcmp(buffer, c6) == 0) return TRUE;
    
    return FALSE;
}

/**
 * \brief check if a given character is a punctuation symbol
 * 
 * \param buffer utf-8 character to be verified as an array of chars
 * 
 * \return TRUE if it's a punctuation symbol, FALSE otherwise
 */
int isPunctuationSymbol(char buffer[]) {
    /* Full point */
    char c1[] = ".";
    if(strcmp(buffer, c1) == 0) return TRUE;

    /* Comma */
    char c2[] = ",";
    if(strcmp(buffer, c2) == 0) return TRUE;

    /* Ccolon */
    char c3[] = ":";
    if(strcmp(buffer, c3) == 0) return TRUE;

    /* Semi-colon */
    char c4[] = ";";
    if(strcmp(buffer, c4) == 0) return TRUE;

    /* Question mark */
    char c5[] = "?";
    if(strcmp(buffer, c5) == 0) return TRUE;

    /* Exclamation point */
    char c6[] = "!";
    if(strcmp(buffer, c6) == 0) return TRUE;

    /* Dash */
    char c7[] = "\xE2\x80\x93";
    if(strcmp(buffer, c7) == 0) return TRUE;

    /* Ellipsis */
    char c8[] = "\xE2\x80\xA6";
    if(strcmp(buffer, c8) == 0) return TRUE;

    return FALSE;
}

/**
 * \brief check if a given character is a merge symbol
 * 
 * \param buffer utf-8 character to be verified as an array of chars
 * 
 * \return TRUE if it's a merge symbol, FALSE otherwise
 */
int isMergeSymbol(char buffer[]) {
    /* Apostrophe */
    char c1[] = "\x27";
    if(strcmp(buffer, c1) == 0) return TRUE;

    /* Left single quotation mark */
    char c2[] = "\xE2\x80\x98";
    if(strcmp(buffer, c2) == 0) return TRUE;
    
    /* Right single quotation mark */
    char c3[] = "\xE2\x80\x99";
    if(strcmp(buffer, c3) == 0) return TRUE;

    return FALSE;
}

/**
 * \brief get the max size of the words inside a file text
 * 
 * \param fileName name of the file
 * 
 * \return value of the max size of a word inside the file text 
 */
int getFileMaxWordSize(char fileName[]) {
    FILE *filePtr;
    filePtr = fopen(fileName, "r");
    char c;
    int maxSize, currSize;
    maxSize = currSize = 0;

    if(filePtr != NULL) {
        while((c=fgetc(filePtr)) != EOF) {
            char buffer[] = {c, '\0'};
            if(isWhiteSpace(buffer) || isSeparationSymbol(buffer) || isPunctuationSymbol(buffer)) {
                // printf("Space: %s", buffer);
                if(currSize > maxSize) {
                    maxSize = currSize;
                }
                currSize = 0;
            } else {
                // printf("Char: %s", buffer);
                currSize++;
            }
        }
    } else {
        return -1;
    }
    
    fclose(filePtr);

    return maxSize;
}