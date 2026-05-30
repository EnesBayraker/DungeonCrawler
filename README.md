# Roguelike Dungeon Crawler

C++17 ve SFML ile geliştirilmiş grid tabanlı bir roguelike dungeon crawler oyunudur. Oyuncu prosedürel oluşturulan zindanlarda ilerler, düşmanlarla savaşır, eşya toplar, envanterini kullanır ve son kata ulaşmaya çalışır.

## Özellikler

- SFML tabanlı grafik ve ses sistemi
- BSP benzeri prosedürel oda/koridor üretimi
- Grid tabanlı hareket ve çarpışma
- Field of View ve Fog of War
- Sıra tabanlı savaş sistemi
- Goblin, Skeleton ve Orc düşmanları
- Düşman yapay zekası: wander, chase ve grup alarmı
- HP barları, item drop sistemi ve zorluk artışı
- Envanter ve eşya kullanımı
- Save / Load sistemi
- Ana menü, How to Play, Pause, Game Over ve Victory ekranları
- Arka plan müzikleri ve sprite tabanlı görseller

## Gereksinimler

- CMake 3.28 veya üzeri
- C++17 destekleyen bir derleyici
- İlk derleme için internet bağlantısı

SFML, `CMakeLists.txt` içinde `FetchContent` ile otomatik olarak indirilir. Ekstra SFML kurulumu gerekmez.

## Kurulum ve Çalıştırma

```bash
git clone https://github.com/EnesBayraker/DungeonCrawler
cd DungeonCrawler
cmake -S . -B build
cmake --build build
```

Windows:

```bash
.\build\bin\DungeonCrawler.exe
```

Linux/macOS:

```bash
./build/bin/DungeonCrawler
```

> `assets/` klasörü proje kök dizininde bulunmalıdır. CMake build sırasında bu klasörü executable yanına otomatik kopyalar.

## Kontroller

| Tuş | İşlev |
|---|---|
| Enter | Yeni oyun başlat / devam et |
| W A S D / Ok tuşları | Hareket / saldırı |
| I | Envanter aç / kapat |
| 1-9 | Envanterdeki eşyayı kullan |
| F5 | Oyunu kaydet |
| L | Ana menüden kayıtlı oyunu yükle |
| H | How to Play ekranı |
| Escape | Pause / geri dön / çıkış |

## Oynanış

Oyuncu her katta rastgele oluşturulan odalar ve koridorlar arasında ilerler. Görüş alanı dışındaki bölgeler karanlık kalır. Düşmanlar oyuncuyu gördüğünde takip eder ve yakın düşmanları alarma geçirebilir.

Oyuncu düşmanın bulunduğu kareye hareket etmeye çalışırsa saldırır. Düşmanlar öldüklerinde belli ihtimalle item düşürür. Merdivene ulaşınca bir sonraki kata geçilir. Son katta merdivene ulaşmak oyunu kazandırır.

## Save / Load

Oyun sırasında `F5` ile kayıt alınır. Ana menüde `L` ile kayıtlı oyun yüklenir.

Kaydedilen bilgiler:

- Oyuncu pozisyonu, HP, max HP ve attack değeri
- Envanter
- Harita tile verisi
- Fog of War keşif bilgisi
- Düşmanlar ve yerdeki item'lar
- Mevcut dungeon depth değeri

## Proje Yapısı

```txt
DungeonCrawler/
├── CMakeLists.txt
├── README.md
├── assets/
│   ├── audio/
│   ├── items/
│   ├── sprites/
│   ├── tiles/
│   └── ui/
└── src/
    ├── AudioManager.*
    ├── Enemy.*
    ├── Entity.*
    ├── Game.*
    ├── GameUI.*
    ├── Item.*
    ├── Map.*
    ├── MessageLog.*
    ├── Player.*
    ├── SaveSystem.*
    └── main.cpp
```

## Teknik Özet

- `Game`: ana oyun döngüsü, state yönetimi ve event handling
- `Map`: harita üretimi, tile bilgisi, FOV ve Fog of War
- `Entity`: Player ve Enemy için ortak temel sınıf
- `Player`: hareket, savaş ve envanter yönetimi
- `Enemy`: düşman türleri, AI ve saldırı davranışı
- `Item`: yerdeki item ve envanter öğeleri
- `SaveSystem`: oyun durumunu dosyaya yazma ve okuma
- `AudioManager`: menü ve oyun içi müzik yönetimi

## Asset ve Müzik Kredileri

Sprite, tile ve item görselleri proje için hazırlanmıştır.

Kullanılan müzik dosyalarının kaynak ve lisans bilgileri:

```txt
Main_Menu_Theme.mp3
- Kaynak: Pixabay.com
- Sanatçı: Kaden_Cook

Gameplay_Theme.mp3
- Kaynak: Pixabay.com
- Sanatçı: HeXXeL
```
