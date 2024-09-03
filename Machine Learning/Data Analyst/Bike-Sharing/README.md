# Proyek Akhir Belajar Analisis Data : Bike Sharing Dataset

## Deksripsi Singkat Proyek
Proyek ini menganalisa bagian data day.csv dari bike sharing analysis. Tujuan dari analisis data ini adalah untuk menemukan insight yang dapat menjawab berbagai pertanyaan bisnis yaitu dapat mengetahui trend perentalan peminjaman sepeda, pengaruh perubahan musim terhadap rata-rata perentalan sepeda, dan pengaruh perbedaan hari kerja terhadap intensitas perentalan sepeda
## Struktur Direktori
- /data berisi data yang akan diolah dan digunakan dalam proyek
- /dashboard berisi dashboard.py yang digunakan untuk membuat menampilkan data yang telah diolah dan memvisualisasikannya
- notebook.ipynb adalah kumpulan kode yang digunakan dalam analisa data
<br><br>
## Langkah untuk menampilkan Dashboard
## Setup environment
```
conda create --name main-ds python=3.9
conda activate main-ds
pip install numpy pandas scipy matplotlib seaborn jupyter streamlit babel
```
## Run steamlit app
```
streamlit run dashboard.py
```
## Dashboard
Untuk dapat melihat dashboard yang telah dibuat dapat dilihat [disini](https://bike-sharing-dashboard-fransiskusabel.streamlit.app/)
