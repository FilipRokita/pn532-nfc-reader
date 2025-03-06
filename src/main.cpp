#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

// Initialize PN532 module
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc(pn532_i2c);

// Store last detected tag UID
String tagId = "None";

/**
 * @brief Reads and prints NFC tag UID.
 */
void readNFC() {
    Serial.println("🔍 Scanning for NFC tag...");

    if (nfc.tagPresent()) {
        NfcTag tag = nfc.read();
        tag.print();

        tagId = tag.getUidString();
        Serial.print("📌 Tag UID: ");
        Serial.println(tagId);
    } else {
        Serial.println("⚠️ No NFC tag detected.");
    }

    delay(5000); // Prevent excessive scanning
}

/**
 * @brief Initializes the NFC module and serial communication.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("📡 NFC Reader Initialized");
    nfc.begin();
}

/**
 * @brief Continuously scans for NFC tags.
 */
void loop() {
    for (int i = 0; i < 3; i++) {
        Serial.println();
    }
    readNFC();
}