#include "util.h"
#include <SPIFFS.h>
#include <stdlib.h>

String readRandomLineFromFile(const char *filename) {
    File file = SPIFFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";
    }

    // Count the number of lines in the file
    int lineCount = 0;
    while (file.available()) {
        if (file.read() == '\n') {
            lineCount++;
        }
    }

    // Generate a random line number
    int randomLine = random(0, lineCount);

    // Read the random line
    file.seek(0);
    for (int i = 0; i < randomLine; i++) {
        file.readStringUntil('\n');
    }
    String randomLineString = file.readStringUntil('\n');
    file.close();

    return randomLineString;
}

const char *getRandomTruth() {
    static String randomTruth = readRandomLineFromFile("/truths.txt");
    return randomTruth.c_str();
}

const char *getRandomRevelation() {
    static String randomRevelation = readRandomLineFromFile("/revelations.txt");
    return randomRevelation.c_str();
}

int calculateMadHatterValue() {
    // Code to calculate the Mad Hatter value
    return 42;
}