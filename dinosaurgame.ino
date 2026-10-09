#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD 1602
// GND ➔ GND Arduino
// VCC ➔ 5V Arduino
// SDA ➔ Pin A4 Arduino
// SCL ➔ Pin A5 Arduino

// Konfigurasi Pin Joystick Baru:
// VRx ➔ Pin A0 Arduino (Untuk deteksi Kiri/Kanan)
// VRy ➔ Pin A1 Arduino (Untuk deteksi Atas/Bawah)
// SW  ➔ Pin 2 Arduino (Untuk tombol klik/Retry)
// VCC ➔ 5V Arduino
// GND ➔ GND Arduino

// Ubah angka 16 menjadi 12 jika layar fisik Anda berukuran 12x2
LiquidCrystal_I2C lcd(0x27, 12, 2); 

int pinX = A0;           
int pinY = A1;           
int pinSW = 2;           // Pin untuk tombol klik joystick

int barisDino = 1;       // 1 = Bawah (Lari), 0 = Atas (Lompat)
int kolomKaktus = 15;    // Ubah ke 11 jika menggunakan LCD 12x2
int skor = 0;

unsigned long waktuLompat = 0;
bool sedangLompat = false;

// Gambar pixel Dinosaurus
byte dino[8] = {
  B00111, B00101, B10111, B10100,
  B01110, B00101, B00101, B00000
};

// Gambar pixel Kaktus
byte kaktus[8] = {
  B00100, B00101, B10101, B10101,
  B11111, B00100, B00100, B00000
};

void setup() {
  pinMode(pinSW, INPUT_PULLUP); // Mengaktifkan resistor internal untuk tombol

  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, dino);
  lcd.createChar(1, kaktus);
}

void loop() {
  int nilaiX = analogRead(pinX);
  int nilaiY = analogRead(pinY);
  
  // Posisi tengah joystick berada di kisaran 512.
  // Jika nilai lebih kecil dari 400 atau lebih besar dari 600, 
  // berarti joystick sedang didorong ke arah mana pun.
  bool adaGerakan = (nilaiX < 400 || nilaiX > 600 || nilaiY < 400 || nilaiY > 600);

  // Logika lompat dari segala arah
  if (adaGerakan && !sedangLompat) {
    sedangLompat = true;
    barisDino = 0;          
    waktuLompat = millis(); 
  }

  // Logika gravitasi (Jatuh setelah 450ms)
  if (sedangLompat && millis() - waktuLompat > 450) {
    sedangLompat = false;
    barisDino = 1;          
  }

  // Kaktus bergerak mendekat
  kolomKaktus--; 
  if (kolomKaktus < 0) {
    kolomKaktus = 15; // Ubah ke 11 jika menggunakan LCD 12x2
    skor++;           
  }

  // Deteksi tabrakan
  if (kolomKaktus == 2 && barisDino == 1) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER!");
    lcd.setCursor(4, 1);
    lcd.print("Skor: ");
    lcd.print(skor);
    
    // Sistem terkunci sampai tombol joystick (SW) diklik
    while (true) {
      // LOW berarti tombol sedang ditekan (karena pakai INPUT_PULLUP)
      if (digitalRead(pinSW) == LOW) { 
        skor = 0;
        kolomKaktus = 15; // Ubah ke 11 jika menggunakan LCD 12x2
        barisDino = 1;
        sedangLompat = false;
        break; // Keluar dari loop Game Over, mengulang game
      }
      delay(50); // Debounce sederhana agar tidak dobel klik
    }
  }

  // Render ulang layar
  lcd.clear(); 
  lcd.setCursor(2, barisDino);
  lcd.write((byte)0); // Diberi (byte) agar tidak terbaca sebagai null terminator pada beberapa versi library
  
  lcd.setCursor(kolomKaktus, 1);
  lcd.write((byte)1); 
  
  lcd.setCursor(13, 0); // Ubah ke posisi 9 jika menggunakan LCD 12x2
  lcd.print(skor);

  delay(150); // Kecepatan game
}