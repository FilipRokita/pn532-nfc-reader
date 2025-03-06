// NFC Mifare Classic Reader & Writer using PN532

#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

// Initialize PN532 module
PN532_I2C pn532I2C(Wire);
PN532 nfc(pn532I2C); // Using PN532 class directly

/**
 * @brief Initializes the NFC module and serial communication.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("📡 NFC System Initialized");

    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();

    if (!versiondata) {
        Serial.println("❌ PN532 not found! Check wiring.");
        while (1);
    }

    Serial.println("✅ PN532 detected!");
    nfc.SAMConfig(); // Configure the module for NFC

    for (int i = 0; i < 5; i++) {
      Serial.println(); // Add a new line
  }
}

/**
 * @brief Reads stored data from the NFC tag.
 */
void readNFC() {
    Serial.println("🔍 Searching for NFC tag...");

    uint8_t success;
    uint8_t uid[7] = {0}; // Buffer for UID
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.print("📖 Reading data from card UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        // Authentication key (default Mifare Classic key)
        uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        // Authenticate sector 1, block 4
        success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, key);
        if (!success) {
            Serial.println("❌ Authentication failed!");
            return;
        }

        Serial.println("🔓 Authentication successful!");

        // Read data
        uint8_t dataBlock[16];
        success = nfc.mifareclassic_ReadDataBlock(4, dataBlock);
        if (success) {
            Serial.print("📖 Data read: ");
            for (int i = 0; i < 16; i++) {
                Serial.write(dataBlock[i]); // Print stored characters
            }
            Serial.println();
        } else {
            Serial.println("❌ Read failed.");
        }
    } else {
        Serial.println("⚠️ No NFC tag detected.");
    }
}

/**
 * @brief Writes a URL to the Mifare Classic NFC tag.
 * 
 * @param url The URL to write.
 */
void writeNFC(String url) {
    Serial.println("🔍 Searching for NFC tag...");

    uint8_t success;
    uint8_t uid[7] = {0};
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.print("🔗 Card detected! UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        // Authentication key
        uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        // Authenticate sector 1, block 4
        success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, key);
        if (!success) {
            Serial.println("❌ Authentication failed!");
            return;
        }

        Serial.println("🔓 Authentication successful!");

        // Prepare data (16 bytes per block)
        uint8_t dataBlock[16] = {0};
        int length = min(url.length(), 16);

        for (int i = 0; i < length; i++) {
            dataBlock[i] = url[i];
        }

        // Write to block 4
        success = nfc.mifareclassic_WriteDataBlock(4, dataBlock);
        if (success) {
            Serial.println("✅ URL written successfully!");
        } else {
            Serial.println("❌ Write failed.");
        }
    } else {
        Serial.println("⚠️ No NFC tag detected.");
    }
}

/**
 * @brief Main loop - Reads NFC first, then writes if a tag is detected.
 */
void loop() {
    readNFC();  // Read data from the tag
    Serial.println(); // Add a new line
    delay(2000);

    writeNFC("https://x.com/");  // Write new data
    for (int i = 0; i < 5; i++) {
        Serial.println(); // Add a new line
    }
    delay(5000);
}
