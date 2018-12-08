/*
  Library for queueing commands for the milight hub
  Created by Jules Huisman, November 26, 2018.
*/

#include <Arduino.h>
#include "Queue.h"

void Queue::add(uint8_t command[]) {
    set(activeIndex, command);
    increaseIndex();
}

void Queue::set(uint8_t index, uint8_t command[]) {
    for(uint8_t i = 0; i < 22; i++) {
        queue[index][i] = command[i];
    }
}

uint8_t* Queue::get() {
    // Fetch the command that needs to be send
    static uint8_t command[22];
    for(uint8_t i = 0; i < 22; i++) {
        command[i] = queue[0][i];
    }

    // Shift all the other items to the front
    if (activeIndex > 1) {
        for(uint8_t i = 0; i < activeIndex - 1; i++) {
            for(uint8_t j = 0; j < 22; j++) {
                queue[i][j] = queue[i + 1][j];
            }
        }
    }

    activeIndex -= 1;

    return command;
}

bool Queue::isEmpty() {
    return activeIndex == 0 ? true : false;
}

void Queue::increaseIndex() {
    if (activeIndex < QUEUE_SIZE) {
        activeIndex += 1;
    }
    else {
        activeIndex = QUEUE_SIZE;
    }
}

void Queue::print() {
    for(uint8_t i = 0; i < QUEUE_SIZE; i++) {
        if (i == activeIndex) Serial.print("> ");
        for(uint8_t j = 0; j < 22; j++) {
            Serial.print(queue[i][j], HEX);
            Serial.print(".");
        }
        Serial.println();
    }
    Serial.println();
}
