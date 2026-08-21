# 🔀 Git Workflow — Panduan Kolaborasi Tim

Dokumen ini menjelaskan **cara kerja Git** di proyek ini agar kode antar divisi tidak saling tabrakan. Baca sampai selesai sebelum mulai ngoding.

---

## Aturan Emas 🏆

```
❌  JANGAN PERNAH push langsung ke branch `main`
❌  JANGAN PERNAH push langsung ke branch `integration`
✅  SELALU kerja di branch divisi masing-masing
✅  SELALU pakai Pull Request (PR) untuk menggabungkan kode
```

---

## Struktur Branch

```
main                         ← Kode final, stabil, siap lomba
  └── integration            ← Tempat Integration Engineer menggabungkan semua
        ├── vision           ← Tempat Vision Engineer kerja
        └── movement         ← Tempat Movement Engineer kerja
```

| Branch | Siapa yang kerja | Push langsung? | Merge ke mana? |
|---|---|---|---|
| `main` | **Tidak ada** | ❌ Dilarang | — |
| `integration` | Integration Engineer | ❌ Dilarang (pakai PR) | → `main` |
| `vision` | Vision Engineer | ✅ Boleh | → `integration` (via PR) |
| `movement` | Movement Engineer | ✅ Boleh | → `integration` (via PR) |

---

## Langkah Kerja Sehari-hari

### 1️⃣ Pertama Kali — Clone & Pindah ke Branch Kamu

```bash
# Clone repo (sekali saja)
git clone https://github.com/Hakaisha1/SFR-MUEZZA-Felicette-HX-.git
cd SFR-MUEZZA-Felicette-HX-

# Pindah ke branch kamu
git checkout vision      # kalau kamu Vision Engineer
git checkout movement    # kalau kamu Movement Engineer
```

### 2️⃣ Sebelum Mulai Kerja — Selalu Tarik Update Terbaru

```bash
# Pastikan kamu di branch yang benar
git checkout vision      # atau movement

# Tarik perubahan terbaru dari remote
git pull origin vision   # atau movement
```

> 💡 **Kenapa?** Supaya kamu tidak mengerjakan kode yang sudah ketinggalan. Kalau ada temanmu yang sudah push, kamu perlu tarik dulu.

### 3️⃣ Ngoding — Kerja Seperti Biasa

Edit file, tambah fitur, perbaiki bug — semua di dalam folder divisi kamu:
- **Vision**: edit file di `/vision/`
- **Movement**: edit file di `/movement/` dan `/Dynamixel/`

> ⚠️ **Jangan edit** file di luar folder kamu (misal: jangan edit `integration/`, `data_contract.json`, atau `protokol_komunikasi.json`) tanpa koordinasi dengan Integration.

### 4️⃣ Simpan Perubahan — Commit & Push

```bash
# Lihat file apa saja yang berubah
git status

# Tambahkan semua perubahan
git add .

# Commit dengan pesan yang jelas
git commit -m "feat: tambah deteksi warna oranye untuk dummy"

# Push ke remote
git push origin vision   # atau movement
```

### 5️⃣ Minta Kode Digabungkan — Buat Pull Request (PR)

Setelah fitur selesai dan sudah diuji sendiri:

1. Buka **GitHub** di browser → [Repository](https://github.com/Hakaisha1/SFR-MUEZZA-Felicette-HX-)
2. Klik tab **"Pull requests"** → **"New pull request"**
3. Atur:
   - **base**: `integration`  ← tujuan merge
   - **compare**: `vision` atau `movement`  ← branch kamu
4. Tulis judul PR yang jelas, contoh:
   - `[Vision] Deteksi dummy berbasis warna HSV`
   - `[Movement] Implementasi tripod gait jalan lurus`
5. Klik **"Create pull request"**
6. **Tunggu review dari Integration Engineer** sebelum merge

> 🔑 **Ingat**: Yang menekan tombol "Merge" di PR adalah **Integration Engineer**, bukan kamu. Ini supaya Integration bisa memastikan kode kamu tidak bentrok dengan divisi lain.

---

## Cara Menulis Pesan Commit yang Baik

Format: `tipe: deskripsi singkat`

| Tipe | Kapan dipakai | Contoh |
|---|---|---|
| `feat` | Menambah fitur baru | `feat: tambah estimasi jarak pakai focal length` |
| `fix` | Memperbaiki bug | `fix: perbaiki false positive deteksi saat backlit` |
| `refactor` | Ubah kode tanpa ubah fungsi | `refactor: pisahkan detector dan classifier` |
| `test` | Tambah/ubah testing | `test: tambah unit test klasifikasi dummy` |
| `docs` | Ubah dokumentasi | `docs: update checklist di guide.md` |
| `chore` | Hal-hal kecil (config, dll) | `chore: update .gitignore` |

Contoh **buruk** ❌:
```
git commit -m "update"
git commit -m "fix bug"
git commit -m "asdfjkl"
```

Contoh **baik** ✅:
```
git commit -m "feat: implementasi inverse kinematics 1 kaki"
git commit -m "fix: perbaiki perhitungan theta2 yang terbalik tanda"
git commit -m "test: validasi IK dengan posisi ujung kaki (100, 0, -50)"
```

---

## Sinkronisasi — Ambil Update dari Integration

Kadang Integration sudah merge perubahan dari divisi lain. Kamu perlu ambil update itu supaya tidak konflik nanti.

```bash
# Pastikan kamu di branch kamu
git checkout vision      # atau movement

# Tarik update dari integration ke branch kamu
git pull origin integration
```

Jika ada **conflict** (Git bilang "CONFLICT"):
1. Buka file yang konflik — cari tanda `<<<<<<<`, `=======`, `>>>>>>>`
2. Pilih kode mana yang benar, hapus tanda-tanda itu
3. Simpan, lalu:
```bash
git add .
git commit -m "merge: resolve conflict dari integration"
git push origin vision   # atau movement
```

> 💡 Kalau bingung soal conflict, **hubungi Integration Engineer** — itu memang tugasnya.

---

## Alur Lengkap (Diagram)

```
Vision Engineer                Integration Engineer           Movement Engineer
      │                               │                              │
      │  push ke `vision`              │          push ke `movement`  │
      │──────────────────►             │             ◄────────────────│
      │                               │                              │
      │  buat PR: vision → integration │  buat PR: movement → integration
      │──────────────────►             │             ◄────────────────│
      │                               │                              │
      │                    review & merge PR                         │
      │                         │                                    │
      │                    test integrasi                            │
      │                         │                                    │
      │                    buat PR: integration → main               │
      │                         │                                    │
      │                    merge ke main ✅                           │
      │                         │                                    │
      │   pull dari integration │  pull dari integration             │
      │◄─────────────────       │        ────────────────►           │
```

---

## FAQ (Pertanyaan yang Sering Muncul)

### "Aku tidak sengaja commit di branch `main`, gimana?"
```bash
# Pindahkan commit terakhir ke branch yang benar
git checkout main
git log --oneline -3          # catat hash commit yang salah
git checkout vision           # pindah ke branch yang benar
git cherry-pick <hash>        # pindahkan commit ke sini
git checkout main
git reset --hard HEAD~1       # hapus commit dari main
git push origin main --force  # (hanya kalau belum dipush)
```
Kalau sudah terlanjur push ke main, **hubungi Integration Engineer segera**.

### "Aku mau edit file di luar folder divisi aku, boleh?"
Tidak boleh langsung. Hubungi Integration dulu dan diskusikan. Jika disetujui, ikuti **prosedur perubahan kontrak** di [`data_contract.json`](data_contract.json).

### "PR aku ada conflict, siapa yang harus resolve?"
- Kalau conflict ada di **file divisi kamu** → kamu yang resolve
- Kalau conflict ada di **file bersama** (README, data_contract, dll) → Integration yang resolve

### "Seberapa sering harus push?"
Minimal **sekali sehari** kalau kamu aktif ngoding. Jangan menumpuk banyak perubahan — makin banyak perubahan, makin besar risiko conflict.

### "Seberapa sering harus buat PR?"
Buat PR **per fitur yang selesai**, bukan per hari. Contoh:
- ✅ `[Vision] Deteksi warna HSV untuk dummy` — 1 PR
- ✅ `[Movement] Tripod gait jalan lurus` — 1 PR
- ❌ Jangan gabung banyak fitur tidak terkait dalam 1 PR

---

## Checklist Sebelum Buat PR

- [ ] Kode sudah diuji sendiri (jalan tanpa error)
- [ ] Hanya mengedit file di folder divisi sendiri
- [ ] Pesan commit jelas dan sesuai format
- [ ] Sudah `git pull origin integration` untuk ambil update terbaru
- [ ] Tidak ada conflict yang belum di-resolve
- [ ] Judul PR jelas: `[Divisi] Deskripsi singkat fitur`
