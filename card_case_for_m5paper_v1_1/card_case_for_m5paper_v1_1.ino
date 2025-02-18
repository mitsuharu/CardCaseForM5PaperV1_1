#include <SD.h>
#include <M5Unified.h>

#define MAX_IMAGES 20  // 画像ファイルの最大数

String imageList[MAX_IMAGES];  // 画像パスを格納する配列
int imageCount = 0;            // 見つかった画像の数
int currentIndex = 0;          // 現在表示中の画像インデックス

void setup() {
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg);  // M5Unified の初期化
  M5.Lcd.setTextSize(6);
  M5.Lcd.println("CardCase For M5Paper V1.1\n");


  M5.Lcd.setTextSize(4);

  // SD の初期化
  // 10 回失敗したら、アプリを終了させる
  int beginCount = 0;
  while (false == SD.begin(GPIO_NUM_4, SPI, 25000000)) {
    Serial.println("wait to mount SD card");
    delay(500);
    beginCount++;
    if (beginCount > 10) {
      M5.Lcd.println("It failed to mount SD card.");
      Serial.println("It failed to mount SD card.");
      while (1)
        ;
    }
  }
  M5.Lcd.println("It opens SD card.");

  // SD のルートディレクトリを走査して画像ファイルをリストアップ
  File root = SD.open("/");
  if (!root) {
    M5.Lcd.println("It failed to open ROOT of SD card");
    Serial.println("It failed to open ROOT of SD card");
    while (1)
      ;
  }

  File file = root.openNextFile();
  while (file && imageCount < MAX_IMAGES) {
    if (!file.isDirectory()) {
      String filename = file.name();
      if (!filename.startsWith(".") && (filename.endsWith(".jpg") || filename.endsWith(".jpeg") || filename.endsWith(".png"))) {
        imageList[imageCount] = String("/") + filename;
        imageCount++;

        String listedImage = "(" + String(imageCount) + ") " + String(filename);
        M5.Lcd.println(listedImage);
        Serial.println(listedImage);
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  if (imageCount > 0) {
    M5.Lcd.println("Push G38 (BottonB).");
  } else {
    M5.Lcd.println("It does not found images.");
    while (1)
      ;
  }
}

void loop() {
  M5.update();

  // 左ボタン (M5.BtnB) がリリースされたら次の画像へ切替え
  if (M5.BtnB.wasPressed()) {
    displayImage(imageList[currentIndex]);
    currentIndex = (currentIndex + 1) % imageCount;
  }
}

// SDカード内の画像ファイルを描画する関数
void displayImage(String filename) {
  M5.Lcd.fillScreen(0xFFFFFF);

  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) {
    M5.Lcd.drawJpgFile(SD, filename, 0, 0);
  } else if (filename.endsWith(".png")) {
    M5.Lcd.drawPngFile(SD, filename, 0, 0);
  }

  delay(200);
}
