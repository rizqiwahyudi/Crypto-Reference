/*
    eXtended Tiny Encryption Algorithm (XTEA)
    Archive of Reversing.ID
    Block Cipher

    Assemble:
        (gcc)
        $ gcc -m32 -S -masm=intel -o XTEA.asm XTEA.c

        (msvc)
        $ cl /c /FaBBS.asm XTEA.c
*/
#include <stdint.h>

/* ********************* INTERNAL FUNCTIONS PROTOTYPE ********************* */


/* ********************* MODE OF OPERATIONS PROTOTYPE ********************* */
/** Electronic Code Book mode **/
void xtea_encrypt_ecb(uint32_t* data, uint32_t block_count, uint32_t key[4]);
void xtea_decrypt_ecb(uint32_t* data, uint32_t block_count, uint32_t key[4]);

/** Cipher Block Chaining mode **/
void xtea_encrypt_cbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);
void xtea_decrypt_cbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);

/** Cipher Feedback mode **/
void xtea_encrypt_cfb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);
void xtea_decrypt_cfb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);

/** Counter mode **/
void xtea_encrypt_ctr(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t nonce[2]);
void xtea_decrypt_ctr(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t nonce[2]);

/** Output Feedback mode **/
void xtea_encrypt_ofb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);
void xtea_decrypt_ofb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);

/** Propagating Cipher Block Chaining mode **/
void xtea_encrypt_pcbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);
void xtea_decrypt_pcbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2]);

/* ************************ CRYPTOGRAPHY ALGORITHM ************************ */
/*
    Enkripsi sebuah block dengan XTEA.
    Sebuah block didefinisikan sebagai dua buah bilangan 32-bit atau 
    setara dengan 64-bit data.
*/
void xtea_encrypt(uint32_t val[2], uint32_t key[4])
{
    uint32_t v0 = val[0], v1 = val[1];
    uint32_t k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];
    uint32_t delta = 0x9E3779B9, sum = 0, i;

    // Round: 32
    for (i =  0; i < 32; i++)
    {
        // Round-Function
        v0  += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1  += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }

    val[0] = v0;
    val[1] = v1;
}



/*
    Dekripsi sebuah block dengan TEA.
    Sebuah block didefinisikan sebagai dua buah bilangan 32-bit atau 
    setara dengan 64-bit data.
*/
void xtea_decrypt(uint32_t val[2], uint32_t key[4])
{
    uint32_t v0 = val[0], v1 = val[1];
    uint32_t k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];
    uint32_t delta = 0x9E3779B9, sum = 0xC6EF3720, i;

    // Round: 32
    for (i =  0; i < 32; i++)
    {
        // Inverse Round-Function
        v1  -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0  -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }

    val[0] = v0;
    val[1] = v1;
}



/* ******************* INTERNAL FUNCTIONS IMPLEMENTATION ******************* */


/* ******************* MODE OF OPERATIONS IMPLEMENTATION ******************* */
/*
    Enkripsi block data dengan mode ECB.
    Enkripsi diberlakukan secara independen tanpa ada hubungan dengan block
    ebelum dan berikutnya.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_ecb(uint32_t* data, uint32_t block_count, uint32_t key[4])
{
    uint32_t i;

    for (i = 0; i < block_count; i += 2)
        xtea_encrypt(&data[i], key);
}

/*
    Dekripsi block data dengan mode ECB.
    Dekripsi diberlakukan secara independen tanpa ada hubungan dengan block
    ebelum dan berikutnya.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_ecb(uint32_t* data, uint32_t block_count, uint32_t key[4])
{
    uint32_t i;

    for (i = 0; i < block_count; i += 2)
        xtea_decrypt(&data[i], key);
}


/*
    Enkripsi block data dengan mode CBC.
    Sebelum enkripsi, plaintext akan di-XOR dengan block sebelumnya.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_cbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // XOR block plaintext dengan block ciphertext sebelumnya
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Enkripsi plaintext menjadi ciphertext
        xtea_encrypt(&data[i], key);

        // Simpan block ciphertext untuk operasi XOR berikutnya
        prev_block[0] = data[i    ];
        prev_block[1] = data[i + 1];


    }
}

/*
    Dekripsi block data dengan mode CBC.
    Setelah dekripsi, plaintext akan di-XOR dengan block sebelumnya.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_cbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];
    uint32_t cipher_block[2];

    // block cipher yang akan di-XOR
    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Simpan block ciphertext untuk operasi XOR berikutnya.
        cipher_block[0] = data[i    ];
        cipher_block[1] = data[i + 1];

        // Dekripsi ciphertext menjadi block
        xtea_decrypt(&data[i], key);

        // XOR block block dengan block ciphertext sebelumnya
        // gunakan IV bila ini adalah block pertama
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Pindahkan block ciphertext yang telah disimpan
        prev_block[0] = cipher_block[0];
        prev_block[1] = cipher_block[1];
    }
}


/*
    Enkripsi block data dengan mode CFB.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_cfb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Enkripsi block sebelumnya
        // gunakan IV bila ini block pertama
        xtea_encrypt(prev_block, key);

        // XOR dengan plaintext untuk mendapatkan ciphertext
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Simpan block ciphertext untuk operasi XOR berikutnya
        prev_block[0] = data[i    ];
        prev_block[1] = data[i + 1];
    }
}

/*
    Dekripsi block data dengan mode CFB.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_cfb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];
    uint32_t cipher_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Simpan block cipher untuk operasi
        cipher_block[0] = data[i    ];
        cipher_block[1] = data[i + 1];

        // Enkripsi block sebelumnya
        // gunakan IV bila ini block pertama
        xtea_encrypt(prev_block, key);

        // XOR dengan plaintext untuk mendapatkan ciphertext
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Simpan block ciphertext untuk operasi XOR berikutnya
        prev_block[0] = cipher_block[0];
        prev_block[1] = cipher_block[1];
    }
}


/*
    Enkripsi block data dengan mode CTR.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_ctr(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t nonce[2])
{
    uint32_t i;
    uint32_t nonce_local[2];

    nonce_local[0] = nonce[0];
    nonce_local[1] = nonce[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Enkripsi nonce + counter
        xtea_encrypt(nonce_local, key);

        // XOR nonce terenkripsi dengan plaintext untuk mendapatkan ciphertext.
        data[i    ] ^= nonce_local[0];
        data[i + 1] ^= nonce_local[1];

        // Naikkan nilai nonce dengan 1.
        nonce_local[1] ++;
    }
}

/*
    Dekripsi block data dengan mode CTR.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_ctr(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t nonce[2])
{
    uint32_t i;
    uint32_t nonce_local[2];

    nonce_local[0] = nonce[0];
    nonce_local[1] = nonce[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Enkripsi nonce + counter
        xtea_encrypt(nonce_local, key);

        // XOR nonce terenkripsi dengan ciphertext untuk mendapatkan plaintext.
        data[i    ] ^= nonce_local[0];
        data[i + 1] ^= nonce_local[1];

        // Naikkan nilai nonce dengan 1.
        nonce_local[1] ++;
    }
}


/*
    Enkripsi block data dengan mode OFB.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_ofb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Enkripsi block sebelumnya 
        // gunakan IV bila ini block pertama
        xtea_encrypt(prev_block, key);

        // XOR plaintext dengan output dari enkripsi untuk mendapatkan ciphertext.
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];
    }
}

/*
    Dekripsi block data dengan mode OFB.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_ofb(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Enkripsi block sebelumnya 
        // gunakan IV bila ini block pertama
        xtea_encrypt(prev_block, key);

        // XOR ciphertext dengan output dari enkripsi untuk mendapatkan plaintext.
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];
    }
}


/*
    Enkripsi block data dengan mode OFB.
    Pastikan jumlah block valid.
*/
void 
xtea_encrypt_pcbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];
    uint32_t ptext_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i += 2)
    {
        // Simpan plaintext untuk dioperasikan dengan block berikutnya.
        ptext_block[0] = data[i    ];
        ptext_block[1] = data[i + 1];

        // XOR plaintext dengan block sebelumnya
        // gunakan IV bila ini block pertama
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Enkripsi
        xtea_encrypt(&data[i], key);

        // Hitung block berikutnya
        prev_block[0] = ptext_block[0] ^ data[i    ];
        prev_block[1] = ptext_block[1] ^ data[i + 1];
    }
}

/*
    Dekripsi block data dengan mode OFB.
    Pastikan jumlah block valid.
*/
void 
xtea_decrypt_pcbc(uint32_t* data, uint32_t block_count, uint32_t key[4], uint32_t iv[2])
{
    uint32_t i;
    uint32_t prev_block[2];
    uint32_t ctext_block[2];

    prev_block[0] = iv[0];
    prev_block[1] = iv[1];

    for (i = 0; i < block_count; i+= 2)
    {
        // Simpan ciphertext untuk dioperasikan dengan block berikutnya.
        ctext_block[0] = data[i    ];
        ctext_block[1] = data[i + 1];

        // Dekripsi ciphertext untuk mendapatkan plaintext ter-XOR
        xtea_decrypt(&data[i], key);

        // XOR dengan block sebelumnya
        data[i    ] ^= prev_block[0];
        data[i + 1] ^= prev_block[1];

        // Hitung block berikutnya
        prev_block[0] = ctext_block[0] ^ data[i    ];
        prev_block[1] = ctext_block[1] ^ data[i + 1];
    }
}

/* ************************ CONTOH PENGGUNAAN ************************ */
#include "../testutil.h"

int main(int argc, char* argv[])
{
    int  i, length;
    char data[] = "Reversing.ID - Reverse Engineering Community";
    char encbuffer[64];
    char decbuffer[64]; 
    uint32_t key[4] = { 0x52455645, 0x5253494E, 0x472E4944, 0x31323334 };
         /* ASCII dari:   R E V E     R S I N     G . I D     1 2 3 4 */
    uint32_t iv[2]  = { 0x13510030, 0x13510030 };

    length = strlen(data);
    printf("Length: %d - Buffer: %s\n", strlen(data), data);
    printx("Original", data, length);

    /*
    Panjang plaintext: 44
    Karena block cipher mensyaratkan bahwa data harus merupakan kelipatan dari ukuran 
    block, maka harus ada padding agar panjang data mencapai kelipatan block.

    Tiap block berukuran 64-bit (32 + 32).
    Data 64-byte menghasilkan 8 block data.
    */
    memset(encbuffer, 0, sizeof(encbuffer));
    memset(decbuffer, 0, sizeof(decbuffer));

    // Tes Enkripsi -----------------------------------------------------------------
    memcpy(encbuffer, data, length);
    // Casting dari char* menjadi uint32_t*
    xtea_encrypt_ecb((uint32_t*)encbuffer, 8, key);         // ECB
    // xtea_encrypt_cbc((uint32_t*)encbuffer, 8, key, iv);     // CBC
    // xtea_encrypt_cfb((uint32_t*)encbuffer, 8, key, iv);     // CFB
    // xtea_encrypt_ctr((uint32_t*)encbuffer, 8, key, iv);     // CTR
    // xtea_encrypt_ofb((uint32_t*)encbuffer, 8, key, iv);     // OFB
    // xtea_encrypt_pcbc((uint32_t*)encbuffer, 8, key, iv);    // PCBC
    printx("Encrypted:", encbuffer, 64);

    // Tes Dekripsi -----------------------------------------------------------------
    memcpy(decbuffer, encbuffer, 64);
    // casting dari char* menjadi uint32_t*
    xtea_decrypt_ecb((uint32_t*)decbuffer, 8, key);         // ECB
    // xtea_decrypt_cbc((uint32_t*)decbuffer, 8, key, iv);     // CBC
    // xtea_decrypt_cfb((uint32_t*)decbuffer, 8, key, iv);     // CFB
    // xtea_decrypt_ctr((uint32_t*)decbuffer, 8, key, iv);     // CTR
    // xtea_decrypt_ofb((uint32_t*)decbuffer, 8, key, iv);     // OFB
    // xtea_decrypt_pcbc((uint32_t*)decbuffer, 8, key, iv);    // PCBC
    printx("Decrypted:", decbuffer, 64);

    printf("\nFinal: %s\n", decbuffer);

    return 0;
}